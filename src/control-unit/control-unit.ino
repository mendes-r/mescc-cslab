#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoMqttClient.h>

//------------
#define WIFI_NETWORK    "MyPhone"
#define WIFI_PASSWORD   "iseprules"
#define WIFI_TIMEOUT    25000

#define SENSOR_1_PORT 4545
#define SENSOR_1 "172.20.10.13"

#define BROKER "172.20.10.3"
#define BROKER_PORT 1883
//------------

#define PUMP_1 13
#define PUMP_STATE_1 12
#define PUMP_CONTROL_1 14

#define PUMP_2 26
#define PUMP_STATE_2 25
#define PUMP_CONTROL_2 27

#define ALERT 32

#define LEVEL_LOW 1
#define LEVEL_MIN 2
#define LEVEL_MED 3
#define LEVEL_MAX 4

#define PUMP_ON 1
#define PUMP_OFF 2

#define PUMP_STATE_NOK 0
#define PUMP_STATE_OK 1

TaskHandle_t Task1, Task2, Task3;
xSemaphoreHandle xMutex;

typedef struct{
  volatile bool alert;
  volatile uint8_t id;
  volatile uint8_t curr_water_level;
  volatile uint8_t curr_pump1_status;
  volatile uint8_t curr_pump2_status;  
  volatile uint8_t curr_pump1_state;
  volatile uint8_t curr_pump2_state;
} WPS;

// Current system's status
WPS _wps = {false, 1, LEVEL_LOW, PUMP_OFF, PUMP_OFF, PUMP_STATE_OK, PUMP_STATE_OK};

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

  xMutex = xSemaphoreCreateMutex(); 

  delay(500);

  xTaskCreatePinnedToCore(
                    requestSensorData,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */    

  xTaskCreatePinnedToCore(
                    controlPumps,   /* Task function. */
                    "Task2",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    2,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */  

  xTaskCreatePinnedToCore(
                    publishStatus,   /* Task function. */
                    "Task3",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    3,           /* priority of the task */
                    &Task3,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                                                
}

void publishStatus (void * parameters)
{
  
  const char topic[]  = "wps1/status";
  unsigned long previousMillis = 0;
  const long interval = 8000;

  for (;;)
  {

    if( WiFi.status() == WL_CONNECTED ){

      WiFiClient client;
      MqttClient mqttClient(client);
      
      if (!mqttClient.connect(BROKER, BROKER_PORT)) {
        Serial.print("Task3 MQTT connection failed! Error code = ");
        Serial.println(mqttClient.connectError());
        vTaskDelay(1000 / portTICK_PERIOD_MS);
      } else {

        mqttClient.poll();
        unsigned long currentMillis = millis();

        if (currentMillis - previousMillis >= interval) {
          // save the last time a message was sent
          previousMillis = currentMillis;

          Serial.print("Task3 Sending message to topic: ");
          Serial.println(topic);

          mqttClient.beginMessage(topic);
          mqttClient.print(getWpsStatus());
          mqttClient.endMessage();

          Serial.println();
        }
      }

      mqttClient.stop();
      client.stop();
    } else{
      connect_to_wifi();
    }

    Serial.println("\nTask3 Running");
    vTaskDelay(1500 / portTICK_PERIOD_MS);
  }
}

void controlPumps (void * parameters)
{
  for (;;)
  {
    updatePumpFailureState();
    uint8_t pumps_state = _wps.curr_pump1_state + _wps.curr_pump2_state;
    
    switch (pumps_state) {
      case 2: // all pumps are working
        allRight(_wps.curr_water_level);
        break;
      case 1: // one pump is malfunction
        halfRight(_wps.curr_water_level);
        sendAlert();
        break;
      case 0: // all pumps failed
        sendAlert();
        break;
    }

    Serial.println("\nTask2 Running");
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void requestSensorData (void * parameters)
{
  for (;;)
  {  
    if( WiFi.status() == WL_CONNECTED ){

      Serial.print("Task1 Connecting to ");
      Serial.println(SENSOR_1);

      // Use WiFiClient class to create TCP connections
      WiFiClient client;

      if (!client.connect(SENSOR_1, SENSOR_1_PORT)) {
          Serial.println("Task1 Connection failed.");
          Serial.println("Task1 Waiting 5 seconds before retrying...");
          vTaskDelay(5000 / portTICK_PERIOD_MS);
      } else {
        Serial.println("Task1 Sending request");

        client.print("GET me the current water level\n\n");
        int maxloops = 0;

        while (!client.available() && maxloops < 1000) {
          maxloops++;
          delay(1); 
        }

        if (client.available() > 0) {
          //read back one line from the server
          String line = client.readStringUntil('\r');
          // save water level
          set_curr_water_level(line[0] - '0');
          Serial.print("Task1 Current water level: ");
          Serial.println(_wps.curr_water_level);
        } else {
          Serial.println("Task1 Client timed out ");
        }
      }
      
      client.stop(); 
    } else{
      connect_to_wifi();
    }
    Serial.println("\nTask1 Running");  
    vTaskDelay(2500 / portTICK_PERIOD_MS);
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
      if (_wps.curr_pump1_status) {
        turnPump_1_ON();
      } else {
        turnPump_2_ON();
      }
      break;
    case LEVEL_MED:
    case LEVEL_MAX:
      if (_wps.curr_pump1_status) {
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
    digitalWrite(PUMP_STATE_1, HIGH);
    set_curr_pump1_state(PUMP_STATE_OK);
  } else {
    digitalWrite(PUMP_STATE_1, LOW);
    turnPump_1_OFF();
    set_curr_pump1_state(PUMP_STATE_NOK);
  }

  if (digitalRead(PUMP_CONTROL_2)){
    digitalWrite(PUMP_STATE_2, HIGH);
    set_curr_pump2_state(PUMP_STATE_OK);
  } else {
    digitalWrite(PUMP_STATE_2, LOW);
    turnPump_2_OFF();
    set_curr_pump2_state(PUMP_STATE_NOK);
  }
}

void connect_to_wifi (void)
{
  Serial.println("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);
  unsigned long start_attempt = millis();

  while( ( WiFi.status() != WL_CONNECTED ) && ( millis() - start_attempt < WIFI_TIMEOUT) )
  {
    Serial.println("Connecting to WiFi");
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }

  if( WiFi.status() != WL_CONNECTED )
  {
    Serial.println("Connecting to WiFi failed"); 
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
  else
  {
    Serial.println("WiFi Connected");
    Serial.println(WiFi.localIP());
  }
}

String getWpsStatus(){
    xSemaphoreTake(xMutex, portMAX_DELAY);
    String message = String(_wps.alert) + "," + String(_wps.id) + "," + String(_wps.curr_water_level) + "," + String(_wps.curr_pump1_status) + "," + String(_wps.curr_pump2_status);
    xSemaphoreGive(xMutex);
    return message;
}

void sendAlert() { digitalWrite(ALERT, HIGH); set_alert(true); Serial.println("Send ALERT!"); }
void stopAlert() { digitalWrite(ALERT, LOW); set_alert(false); Serial.println("Stop ALERT!"); }

void turnPump_1_ON() { digitalWrite(PUMP_1, HIGH); set_curr_pump1_status(PUMP_ON);}
void turnPump_1_OFF() { digitalWrite(PUMP_1, LOW); set_curr_pump1_status(PUMP_OFF);}
void turnPump_2_ON() { digitalWrite(PUMP_2, HIGH); set_curr_pump2_status(PUMP_ON);}
void turnPump_2_OFF() { digitalWrite(PUMP_2, LOW); set_curr_pump2_status(PUMP_OFF);}


void set_alert(bool value) {
  xSemaphoreTake(xMutex, portMAX_DELAY);
  _wps.alert = value;
  xSemaphoreGive(xMutex);
}

void set_curr_water_level(uint8_t value) {
  xSemaphoreTake(xMutex, portMAX_DELAY);
  _wps.curr_water_level = value;
  xSemaphoreGive(xMutex);
}

void set_curr_pump1_status(uint8_t value) {
  xSemaphoreTake(xMutex, portMAX_DELAY);  
  _wps.curr_pump1_status = value;
  xSemaphoreGive(xMutex);
}

void set_curr_pump2_status(uint8_t value) {
  xSemaphoreTake(xMutex, portMAX_DELAY);
  _wps.curr_pump2_status = value;
  xSemaphoreGive(xMutex);
}

void set_curr_pump1_state(uint8_t value) {
  xSemaphoreTake(xMutex, portMAX_DELAY);
  _wps.curr_pump1_state = value;
  xSemaphoreGive(xMutex);
}

void set_curr_pump2_state(uint8_t value) {
  xSemaphoreTake(xMutex, portMAX_DELAY);
  _wps.curr_pump1_state = value;
  xSemaphoreGive(xMutex);
}

void loop() {}