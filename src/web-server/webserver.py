# Python 3 server example
from http.server import BaseHTTPRequestHandler, HTTPServer
from paho.mqtt import client as mqtt_client_lib
from bs4 import BeautifulSoup

hostName = "localhost"
serverPort = 8080

#mosquitto_sub -h 172.20.10.3 -t wps1/status
mqtt_broker = 'localhost'
mqtt_port = 1883
mqtt_topic = "wps1/status"
client_id = 'StatusServer'

class MQTTClient:
    def __init__(self):
        mqtt_client = mqtt_client_lib.Client()
        mqtt_client.connect(mqtt_broker, mqtt_port, 60)
        mqtt_client.loop_start()
        mqtt_client.subscribe(mqtt_topic)
        mqtt_client.on_message = self.on_message
    
    def on_message(self, client, userdata, msg):
        print(f"Received `{msg.payload.decode()}` from `{msg.topic}` topic")

        message = msg.payload.decode('utf-8')
        values = [int(num) for num in message.split(',')]
        if len(values) == 4:
            alert, level, pump1, pump2 = values


        with open('index.html', 'r') as file:
            html_content = file.read()

        # Parse the HTML content
        soup = BeautifulSoup(html_content, 'html.parser')

        # Find the p tag with id "dynamic_status" and update its text
        dynamic_status_tag = soup.find('p', {'id': 'dynamic_alarm'})
        if dynamic_status_tag:
            dynamic_status_tag.string = f'Alarm: {alert}'
        
        dynamic_status_tag = soup.find('p', {'id': 'dynamic_level'})
        if dynamic_status_tag:
            dynamic_status_tag.string = f'Water Level: {level}'

        dynamic_status_tag = soup.find('p', {'id': 'dynamic_pump1'})
        if dynamic_status_tag:
            dynamic_status_tag.string = f'Pump 1: {pump1}'

        dynamic_status_tag = soup.find('p', {'id': 'dynamic_pump2'})
        if dynamic_status_tag:
            dynamic_status_tag.string = f'Pump 2: {pump2}'

        # Write the modified content back to the file
        with open('index.html', 'w') as file:
            file.write(str(soup))


class MyServer(BaseHTTPRequestHandler):

    mqtt_client = MQTTClient()

    def do_GET(self):
        if self.path == "/":
            self.send_response(200)
            self.send_header("Content-type", "text/html")
            self.end_headers()

            # Read the content of index.html
            with open("index.html", "r") as file:
                html_content = file.read()

            self.wfile.write(html_content.encode('utf-8'))

if __name__ == "__main__":        
    webServer = HTTPServer((hostName, serverPort), MyServer)
    print("Server started http://%s:%s" % (hostName, serverPort))

    try:
        webServer.serve_forever()
    except KeyboardInterrupt:
        pass

    webServer.server_close()
    print("Server stopped.")



##### keep following here https://www.emqx.com/en/blog/how-to-use-mqtt-in-python
    
    # mosquitto_pub -h localhost -t "wps1/status" -m "1,1,2,2"
    # mosquitto_sub -h localhost -i sub_test -t "wps1/status"




