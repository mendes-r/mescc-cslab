# Python 3 server example
from http.server import BaseHTTPRequestHandler, HTTPServer
from paho.mqtt import client as mqtt_client_lib
from bs4 import BeautifulSoup
import time

HOSTNAME = "localhost"
SERVER_PORT = 8080

# MQTT params
MQTT_BROKER = 'localhost'
MQTT_PORT = 1883
MQTT_TOPIC_WPS1 = "wps1/status"
MQTT_TOPIC_WPS2= "wps2/status"

# Params for MQTT reconnection
FIRST_RECONNECT_DELAY = 1
RECONNECT_RATE = 2
MAX_RECONNECT_COUNT = 12
MAX_RECONNECT_DELAY = 60

# Class that handles the MQTT connection
class MQTTClient:

    def __init__(self):
        mqtt_client = mqtt_client_lib.Client()
        mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)
        mqtt_client.loop_start()
        mqtt_client.subscribe(MQTT_TOPIC_WPS1)
        mqtt_client.subscribe(MQTT_TOPIC_WPS2)

        mqtt_client.on_message = self.on_message
        mqtt_client.on_disconnect = self.on_disconnect


    # Parsers the message received and update the web page
    def on_message(self, client, userdata, msg):

        if msg.topic == MQTT_TOPIC_WPS1:
            self.on_message_topic_1(msg)
        elif msg.topic == MQTT_TOPIC_WPS2:
            self.on_message_topic_2(msg)


    def on_message_topic_1(self, msg):

        print(f"Received `{msg.payload.decode()}` from `{msg.topic}` topic")

        message = msg.payload.decode('utf-8')
        values = [int(num) for num in message.split(',')]

        if len(values) == 5:
            alarm, id, level, pump1, pump2 = values

        with open('index.html', 'r') as file:
            html_content = file.read()

        # Parse the HTML content
        soup = BeautifulSoup(html_content, 'html.parser')

        # Find the p tag with id "dynamic_status" and update its text
        dynamic_status_tag = soup.find('p', {'id': 'dynamic_wps1_alarm'})
        if dynamic_status_tag:
            dynamic_status_tag.string = f'Alarm: {status_toString(alarm)}'
        
        dynamic_status_tag = soup.find('p', {'id': 'dynamic_wps1_level'})
        if dynamic_status_tag:
            dynamic_status_tag.string = f'Water Level: {level_toString(level)}'

        dynamic_status_tag = soup.find('p', {'id': 'dynamic_wps1_pump1'})
        if dynamic_status_tag:
            dynamic_status_tag.string = f'Pump 1: {status_toString(pump1)}'

        dynamic_status_tag = soup.find('p', {'id': 'dynamic_wps1_pump2'})
        if dynamic_status_tag:
            dynamic_status_tag.string = f'Pump 2: {status_toString(pump2)}'

        # Write the modified content back to the file
        with open('index.html', 'w') as file:
            file.write(str(soup))    



    def on_message_topic_2(self, msg):

        print(f"Received `{msg.payload.decode()}` from `{msg.topic}` topic")

        message = msg.payload.decode('utf-8')
        values = [int(num) for num in message.split(',')]

        if len(values) == 5:
            alarm, id, level, pump1, pump2 = values

        with open('index.html', 'r') as file:
            html_content = file.read()

        # Parse the HTML content
        soup = BeautifulSoup(html_content, 'html.parser')

        # Find the p tag with id "dynamic_status" and update its text
        dynamic_status_tag = soup.find('p', {'id': 'dynamic_wps2_alarm'})
        if dynamic_status_tag:
            dynamic_status_tag.string = f'Alarm: {status_toString(alarm)}'
        
        dynamic_status_tag = soup.find('p', {'id': 'dynamic_wps2_level'})
        if dynamic_status_tag:
            dynamic_status_tag.string = f'Water Level: {level_toString(level)}'

        dynamic_status_tag = soup.find('p', {'id': 'dynamic_wps2_pump1'})
        if dynamic_status_tag:
            dynamic_status_tag.string = f'Pump 1: {status_toString(pump1)}'

        dynamic_status_tag = soup.find('p', {'id': 'dynamic_wps2_pump2'})
        if dynamic_status_tag:
            dynamic_status_tag.string = f'Pump 2: {status_toString(pump2)}'

        # Write the modified content back to the file
        with open('index.html', 'w') as file:
            file.write(str(soup))    


    def on_disconnect(self, client, userdata, rc):
        print("Disconnected with result code: %s", rc)
        reconnect_count, reconnect_delay = 0, FIRST_RECONNECT_DELAY
        while reconnect_count < MAX_RECONNECT_COUNT:
            print("Reconnecting in %d seconds...", reconnect_delay)
            time.sleep(reconnect_delay)

            try:
                client.reconnect()
                print("Reconnected successfully!")
                client.subscribe(MQTT_TOPIC_WPS1)
                client.subscribe(MQTT_TOPIC_WPS2)
                #TODO: Fix no message received after reconnection
                return
            except Exception as err:
                print("%s. Reconnect failed. Retrying...", err)

            reconnect_delay *= RECONNECT_RATE
            reconnect_delay = min(reconnect_delay, MAX_RECONNECT_DELAY)
            reconnect_count += 1
        print("Reconnect failed after %s attempts. Exiting...", reconnect_count)

# Class that handles the HTTP requests
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

# Functions for decoding the values received
def status_toString(value):
    if value == 1:
        return "ON"
    elif value == 0:
        return "OFF"
    else:
        return ""
    
def level_toString(value):
    if value == 1:
        return "LOW"
    elif value == 2:
        return "MIN"
    elif value == 3:
        return "MED"
    elif value == 4:
        return "MAX"

if __name__ == "__main__":        
    webServer = HTTPServer((HOSTNAME, SERVER_PORT), MyServer)
    print("Server started http://%s:%s" % (HOSTNAME, SERVER_PORT))

    try:
        webServer.serve_forever()
    except KeyboardInterrupt:
        pass

    webServer.server_close()
    print("Server stopped.")





##### keep following here https://www.emqx.com/en/blog/how-to-use-mqtt-in-python
    
    # mosquitto_pub -h localhost -t "wps1/status" -m "1,1,2,2"
    # mosquitto_sub -h localhost -i sub_test -t "wps1/status"




