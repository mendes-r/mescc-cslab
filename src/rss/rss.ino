#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoMqttClient.h>

#define ALARM_PIN 12

#define PUMP_1_PIN 14
#define PUMP_2_PIN 27

#define WATER_LEVEL_LOW_PIN 26
#define WATER_LEVEL_MIN_PIN 25
#define WATER_LEVEL_MED_PIN 33
#define WATER_LEVEL_MAX_PIN 32

#define BUZZ_PIN  18
#define BUZZ_BOTTON_PIN  19

//------------
#define WIFI_NETWORK    "MyPhone"
#define WIFI_PASSWORD   "iseprules"
#define WIFI_TIMEOUT    25000

#define BROKER "172.20.10.3"
#define BROKER_PORT 1883
//------------

#define LEVEL_LOW 1
#define LEVEL_MIN 2
#define LEVEL_MED 3
#define LEVEL_MAX 4

#define PUMP_OFF 0
#define PUMP_ON 1

WiFiClient client;
MqttClient mqttClient(client);

const char topic[]  = "wps1/status";

uint8_t alert_lost_broker = 0;
const uint8_t MAX_NO_MESSAGE = 10;

bool alert_checked = false;
bool alert = false;

void setup() {
  Serial.begin(115200);

  pinMode(ALARM_PIN, OUTPUT);
  pinMode(BUZZ_PIN, OUTPUT);
  pinMode(PUMP_1_PIN, OUTPUT);
  pinMode(PUMP_2_PIN, OUTPUT);
  pinMode(WATER_LEVEL_LOW_PIN, OUTPUT);
  pinMode(WATER_LEVEL_MIN_PIN, OUTPUT);
  pinMode(WATER_LEVEL_MED_PIN, OUTPUT);
  pinMode(WATER_LEVEL_MAX_PIN, OUTPUT);
  pinMode(BUZZ_BOTTON_PIN, INPUT);

  connect_to_broker();
}

void loop() {
  
  if( WiFi.status() == WL_CONNECTED ){
    mqttClient.poll();  
    if (alert_lost_broker >= MAX_NO_MESSAGE) {
      alarm_on();
      Serial.println("During the last 10 seconds, no message was received.");
    }
    alert_lost_broker++;
  } else{
    connect_to_broker();
  }

  check_alert_button();
  delay(1000);           
}

void check_alert_button() {
  int button_state = digitalRead(BUZZ_BOTTON_PIN);
  if ( button_state == HIGH && alert){ 
    Serial.println("Button triggered");
    alert_checked = true;
    digitalWrite(BUZZ_PIN, LOW);
  }  
}

void alarm_on(){
  digitalWrite(ALARM_PIN, HIGH);
  alert = true;
  if (!alert_checked) {
    digitalWrite(BUZZ_PIN, HIGH);
  }
}

void alarm_off(){
  alert = false;
  digitalWrite(ALARM_PIN, LOW);
  digitalWrite(BUZZ_PIN, LOW);
  alert_checked = false;
}

void onMqttMessage(int messageSize) {
  alert_lost_broker = 0;
  // example: 1,1,4,1,1
  const uint8_t max_len = 10;
  char *message = (char *) malloc(max_len);
  uint8_t i = 0; 

  // we received a message, print out the topic and contents
  Serial.println("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    if (i < max_len) {
      message[i] = (char)mqttClient.read();
      Serial.print(message[i]);
    } else {
      Serial.println("\nnot catched by string buffer:");
      Serial.print((char)mqttClient.read());
    }
    i++;
  }

  Serial.println();

  int alarm = message[0] - '0';
  int wps_id = message[2] - '0';
  int level = message[4] - '0';
  int pump_1 = message[6] - '0';
  int pump_2 = message[8] - '0';

  Serial.print("\nAfter conversion: ");
  Serial.print(alarm);
  Serial.print(wps_id);
  Serial.print(level);
  Serial.print(pump_1);
  Serial.print(pump_2);
  Serial.println();

  do_alarm(alarm);
  do_level(level);
  do_pump_1(pump_1);
  do_pump_2(pump_2);
}

void do_pump_1(int value) {
  switch (value) {
  case PUMP_OFF:
    digitalWrite(PUMP_1_PIN, LOW);
    break;
  case PUMP_ON:
    digitalWrite(PUMP_1_PIN, HIGH);
    break;
  }
}

void do_pump_2(int value) {
  switch (value) {
  case PUMP_OFF:
    digitalWrite(PUMP_2_PIN, LOW);
    break;
  case PUMP_ON:
    digitalWrite(PUMP_2_PIN, HIGH);
    break;
  }
}

void do_alarm(int value){
  switch (value) {
  case 0:
    alarm_off();
    break;
  case 1:
    alarm_on();
    break;
  }
}

void do_level(int value){
  
  digitalWrite(WATER_LEVEL_LOW_PIN, LOW);
  digitalWrite(WATER_LEVEL_MIN_PIN, LOW);
  digitalWrite(WATER_LEVEL_MED_PIN, LOW);
  digitalWrite(WATER_LEVEL_MAX_PIN, LOW);
  
  switch (value) {
  case LEVEL_LOW:
    digitalWrite(WATER_LEVEL_LOW_PIN, HIGH);
    break;
  case LEVEL_MIN:
    digitalWrite(WATER_LEVEL_MIN_PIN, HIGH);
    break;
  case LEVEL_MED:
    digitalWrite(WATER_LEVEL_MED_PIN, HIGH);
    break;
  case LEVEL_MAX:
    digitalWrite(WATER_LEVEL_MAX_PIN, HIGH);
    break;
  }
}

void connect_to_broker (void) {
  Serial.println("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);
  unsigned long start_attempt = millis();

  while( ( WiFi.status() != WL_CONNECTED ) && ( millis() - start_attempt < WIFI_TIMEOUT) )
  {
    Serial.println("Connecting to WiFi");
    delay(500);
  }

  if( WiFi.status() != WL_CONNECTED )
  {
    Serial.println("Connecting to WiFi failed"); 
    delay(5000);
  }
  else
  {
    Serial.println("WiFi Connected");
    Serial.println(WiFi.localIP());
  }

  if (!mqttClient.connect(BROKER, BROKER_PORT)) {
    Serial.print("Task3 MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    delay(1000);
  } 

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  // set the message receive callback
  mqttClient.onMessage(onMqttMessage);

  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  Serial.println();

  // subscribe to a topic
  mqttClient.subscribe(topic);
}
