#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

//------------
#define WIFI_NETWORK    "MyPhone"
#define WIFI_PASSWORD   "iseprules"
#define WIFI_TIMEOUT    15000

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

// Current system's status
volatile uint8_t wps_id = 1;
volatile uint8_t curr_water_level = LEVEL_LOW;
volatile uint8_t curr_pump1_status = PUMP_OFF;
volatile uint8_t curr_pump2_status = PUMP_OFF;
volatile bool alert = false;
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
                    controlPumps,       /* Task function. */
                    "Task2",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    2,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */

  xTaskCreatePinnedToCore(
                    publishStatus,   /* Task function. */
                    "Task3",     /* name of task. */
                    20000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    3,           /* priority of the task */
                    &Task3,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                                                                 
}

void publishStatus (void * parameters)
{
  for (;;)
  {
    Serial.println("\nTask1 running");
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void controlPumps (void * parameters)
{
  for (;;)
  {
    Serial.println("\nTask2 running");

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

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void requestSensorData (void * parameters)
{
  for (;;)
  {  
    if( WiFi.status() == WL_CONNECTED ){
      WiFiClient client;
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
        curr_water_level = line[0] - '0';
      } else {
        Serial.println("Client.available() timed out ");
      }
      client.stop(); 
    } else{
      connect_to_wifi();
    }
    Serial.println("\nTask3 running");  
    vTaskDelay(1000 / portTICK_PERIOD_MS);
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
    digitalWrite(PUMP_STATE_1, HIGH);
    curr_pump1_state = PUMP_STATE_OK;
  } else {
    digitalWrite(PUMP_STATE_1, LOW);
    turnPump_1_OFF();
    curr_pump1_state = PUMP_STATE_NOK;;
  }

  if (digitalRead(PUMP_CONTROL_2)){
    digitalWrite(PUMP_STATE_2, HIGH);
    curr_pump2_state = PUMP_STATE_OK;;
  } else {
    digitalWrite(PUMP_STATE_2, LOW);
    turnPump_2_OFF();
    curr_pump2_state = PUMP_STATE_NOK;;
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
  return String(alert) + "," + String(wps_id) + "," + String(curr_water_level) + "," + String(curr_pump1_status) + "," + String(curr_pump2_status);
}

void sendAlert() { digitalWrite(ALERT, HIGH); alert = true; Serial.println("Send ALERT!"); }
void stopAlert() { digitalWrite(ALERT, LOW); alert = false; Serial.println("Stop ALERT!"); }

void turnPump_1_ON() { digitalWrite(PUMP_1, HIGH); curr_pump1_status = PUMP_ON;}
void turnPump_1_OFF() { digitalWrite(PUMP_1, LOW); curr_pump1_status = PUMP_OFF;}
void turnPump_2_ON() { digitalWrite(PUMP_2, HIGH); curr_pump2_status = PUMP_ON;}
void turnPump_2_OFF() { digitalWrite(PUMP_2, LOW); curr_pump2_status = PUMP_OFF;}

void loop() {}