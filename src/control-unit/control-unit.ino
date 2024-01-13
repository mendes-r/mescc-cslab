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

volatile uint8_t pump_1 = 1;
volatile uint8_t pump_2 = 1;

#include <WiFi.h>
#include <WiFiMulti.h>

const char* ssid     = "MyPhone";
const char* password = "iseprules";
const uint16_t port = 4545;
const char * host = "172.20.10.13"; 

WiFiMulti WiFiMulti;

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

  delay(500);
}

void loop() {

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

  //wait for the server's reply to become available
  while (!client.available() && maxloops < 1000)
  {
    maxloops++;
    delay(1); //delay 1 msec
  }
  if (client.available() > 0)
  {
    //read back one line from the server
    String line = client.readStringUntil('\r');
    Serial.println(line);
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


    //read back one line from the server
    //String line = client.readStringUntil('\r');
    //Serial.println(line);
    //int8_t level = line[0] - '0';
    //controlPumps(level);
    
void controlPumps(int8_t level)
{
  updatePumpFailureState();
  uint8_t pumps_state = pump_1 + pump_2;
  
  switch (pumps_state) {
    case 2: // all pumps are working
      allRight(level);
      stopAlert();
      break;
    case 1: // one pump is malfunction
      halfRight(level);
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
      digitalWrite(PUMP_1, LOW);
      digitalWrite(PUMP_2, LOW);
      break;
    case LEVEL_MIN: 
      digitalWrite(PUMP_1, HIGH);
      digitalWrite(PUMP_2, LOW);
      break;
    case LEVEL_MED: 
      digitalWrite(PUMP_1, HIGH);
      digitalWrite(PUMP_2, HIGH);
      break;
    case LEVEL_MAX: 
      digitalWrite(PUMP_1, HIGH);
      digitalWrite(PUMP_2, HIGH);
      sendAlert();
  }
}

void halfRight(int8_t level)
{
  switch (level) {
    case LEVEL_LOW: 
      digitalWrite(PUMP_1, LOW);
      digitalWrite(PUMP_2, LOW);
      break;
    case LEVEL_MIN:
      if (pump_1) {
        digitalWrite(PUMP_1, HIGH);
      } else {
        digitalWrite(PUMP_2, HIGH);
      }
      break;
    case LEVEL_MED:
    case LEVEL_MAX:
      if (pump_1) {
        digitalWrite(PUMP_1, HIGH);
      } else {
        digitalWrite(PUMP_2, HIGH);
      } 
      sendAlert();
  }
}

void updatePumpFailureState()
{
  if (digitalRead(PUMP_CONTROL_1)){
    Serial.println("Pump 1 is functional");
    digitalWrite(PUMP_STATE_1, HIGH);
    pump_1 = 1;
  } else {
    Serial.println("Pump 1 FAILURE");
    digitalWrite(PUMP_STATE_1, LOW);
    digitalWrite(PUMP_1, LOW);
    pump_1 = 0;
  }

    if (digitalRead(PUMP_CONTROL_2)){
    Serial.println("Pump 2 is functional");
    digitalWrite(PUMP_STATE_2, HIGH);
    pump_2 = 1;
  } else {
    Serial.println("Pump 2 FAILURE");
    digitalWrite(PUMP_STATE_2, LOW);
    digitalWrite(PUMP_2, LOW);
    pump_2 = 0;
  }
}

void sendAlert() { digitalWrite(ALERT, HIGH); Serial.println("Send ALERT!"); }
void stopAlert() { digitalWrite(ALERT, LOW); Serial.println("Stop ALERT!"); }
