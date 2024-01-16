#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include "TinyMqtt.h"    
#include "TinyStreaming.h" 

static const uint8_t PUMP_1 = 13;
static const uint8_t PUMP_STATE_1 = 12;
static const uint8_t PUMP_CONTROL_1 = 14;

static const uint8_t PUMP_2 = 26;
static const uint8_t PUMP_STATE_2 = 25;
static const uint8_t PUMP_CONTROL_2 = 27;

static const uint8_t ALERT = 32;

static const int8_t LEVEL_LOW = 1;
static const int8_t LEVEL_MIN = 2;
static const int8_t LEVEL_MED = 3;
static const int8_t LEVEL_MAX = 4;

static const int8_t PUMP_ON = 1;
static const int8_t PUMP_OFF = 2;

static const int8_t PUMP_STATE_NOK = 0;
static const int8_t PUMP_STATE_OK = 1;

const char* ssid     = "MyPhone";
const char* password = "iseprules";
const uint16_t port = 4545;
const char * host = "172.20.10.13";

const char* BROKER = "172.20.10.3";
const uint16_t BROKER_PORT = 1883;

// Task1: TCP client
// Task2: Data processing and pump commands
// Task3: MQTT client
TaskHandle_t Task1, Task2, Task3;
xSemaphoreHandle mutex;
WiFiMulti WiFiMulti;
static MqttClient mqttClient;

// Current system's status
volatile uint8_t wps_id = 1;
volatile uint8_t curr_water_level = LEVEL_LOW;
volatile uint8_t curr_pump1_status = PUMP_OFF;
volatile uint8_t curr_pump2_status = PUMP_OFF;

bool alert = false;

volatile uint8_t curr_pump1_state = PUMP_STATE_OK;
volatile uint8_t curr_pump2_state = PUMP_STATE_OK;


void setup() {
  Serial.begin(115200);

  pinMode(ALERT, OUTPUT);
  pinMode(PUMP_1, OUTPUT);
  pinMode(PUMP_STATE_1, OUTPUT);
  pinMode(PUMP_CONTROL_1, INPUT);
  pinMode(PUMP_2, OUTPUT);
  pinMode(PUMP_STATE_2, OUTPUT);
  pinMode(PUMP_CONTROL_2, INPUT);

  digitalWrite(ALERT, LOW);
  delay(10);

  wifiConnection();
  mqttClient.connect(BROKER, BROKER_PORT);

  delay(500);

  mutex = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(
                  tcpClient,   /* Task function. */
                  "Task1",     /* name of task. */
                  10000,       /* Stack size of task */
                  NULL,        /* parameter of the task */
                  1,           /* priority of the task */
                  &Task1,      /* Task handle to keep track of created task */
                  1);          /* pin task to core 1 */
  
  xTaskCreatePinnedToCore(
                  controlPumps,   /* Task function. */
                  "Task2",        /* name of task. */
                  10000,          /* Stack size of task */
                  NULL,           /* parameter of the task */
                  2,              /* priority of the task */
                  &Task2,         /* Task handle to keep track of created task */
                  0);             /* pin task to core 0 */

  xTaskCreatePinnedToCore(
                  publish,        /* Task function. */
                  "Task3",        /* name of task. */
                  10000,          /* Stack size of task */
                  NULL,           /* parameter of the task */
                  3,              /* priority of the task */
                  &Task3,         /* Task handle to keep track of created task */
                  0);             /* pin task to core 0 */
}

void publish(void * parameters)
{
	//client.loop();	
	static auto next_send = millis();
	
	if (millis() > next_send)
	{
		next_send += 1000;

		if (not mqttClient.connected())
		{
      Serial.println("Not connected to broker");
			return;
		}

    Serial.println("Publishing a new wps1/status value");
		mqttClient.publish("wps1/status", getWpsStatus());
	}
}

void tcpClient (void * parameters) {
  Serial.print("Connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (!client.connect(host, port)) {
      Serial.println("Connection failed.");
      Serial.println("Waiting 5 seconds before retrying...");
      delay(5000);
      return;
  }

  client.print("GET me the current water level\n\n");
  int maxloops = 0;

  while (!client.available() && maxloops < 1000)
  {
    maxloops++;
    delay(1); 
  }
  if (client.available() > 0)
  {
    //read back one line from the server
    String line = client.readStringUntil('\r');
    // save water level
    curr_water_level = line[0] - '0';
  }
  else
  {
    Serial.println("Client.available() timed out ");
  }

  Serial.println("Closing connection.");
  client.stop();

  Serial.println("Waiting 5 seconds before restarting...");
  delay(1000);
}

void controlPumps(void * parameters)
{
  updatePumpFailureState();
  uint8_t pumps_state = curr_pump1_state + curr_pump2_state;
  
  switch (pumps_state) {
    case 2: // all pumps are working
      allRight(curr_water_level);
      break;
    case 1: // one pump is malfunction
      halfRight(curr_water_level);
      sendAlert();
      break;
    case 0: // all pumps failed
      sendAlert();
      break;
  }
}

void allRight(int8_t level)
{
  switch (level) {
    case LEVEL_LOW: 
      turnPump_1_OFF();
      turnPump_2_OFF();
      stopAlert();
      break;
    case LEVEL_MIN: 
      turnPump_1_ON();
      turnPump_2_OFF();
      stopAlert();
      break;
    case LEVEL_MED: 
      turnPump_1_ON();
      turnPump_2_ON();
      stopAlert();
      break;
    case LEVEL_MAX: 
      turnPump_1_ON();
      turnPump_2_ON();
      sendAlert();
  }
}

void halfRight(int8_t level)
{
  switch (level) {
    case LEVEL_LOW: 
      turnPump_1_OFF();
      turnPump_2_OFF();
      break;
    case LEVEL_MIN:
      if (curr_pump1_status) {
        turnPump_1_ON();
      } else {
        turnPump_2_ON();
      }
      break;
    case LEVEL_MED:
    case LEVEL_MAX:
      if (curr_pump1_status) {
        turnPump_1_ON();
      } else {
        turnPump_2_ON();
      } 
      sendAlert();
  }
}

void updatePumpFailureState()
{
  if (digitalRead(PUMP_CONTROL_1)){
    Serial.println("Pump 1 is functional");
    digitalWrite(PUMP_STATE_1, HIGH);
    curr_pump1_state = PUMP_STATE_OK;
  } else {
    Serial.println("Pump 1 FAILURE");
    digitalWrite(PUMP_STATE_1, LOW);
    turnPump_1_OFF();
    curr_pump1_state = PUMP_STATE_NOK;;
  }

  if (digitalRead(PUMP_CONTROL_2)){
    Serial.println("Pump 2 is functional");
    digitalWrite(PUMP_STATE_2, HIGH);
    curr_pump2_state = PUMP_STATE_OK;;
  } else {
    Serial.println("Pump 2 FAILURE");
    digitalWrite(PUMP_STATE_2, LOW);
    turnPump_2_OFF();
    curr_pump2_state = PUMP_STATE_NOK;;
  }
}

void wifiConnection() {
  // start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFiMulti.addAP(ssid, password);

  while(WiFiMulti.run() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

String getWpsStatus(){
  return String(alert) + "," + String(wps_id) + "," + String(curr_water_level) + "," + String(curr_pump1_status) + "," + String(curr_pump2_status);
}

void sendAlert() { digitalWrite(ALERT, HIGH); alert = true; Serial.println("Send ALERT!"); }
void stopAlert() { digitalWrite(ALERT, LOW); alert = false; Serial.println("Stop ALERT!"); }

void turnPump_1_ON() { digitalWrite(PUMP_1, HIGH); curr_pump1_status = PUMP_ON;}
void turnPump_1_OFF() { digitalWrite(PUMP_1, LOW); curr_pump1_status = PUMP_OFF;}
void turnPump_2_ON() { digitalWrite(PUMP_2, HIGH); curr_pump2_status = PUMP_ON;}
void turnPump_2_OFF() { digitalWrite(PUMP_2, LOW); curr_pump2_status = PUMP_OFF;}

void loop() {}
