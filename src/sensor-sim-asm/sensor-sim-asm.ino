static const int8_t MAX_LEVELS = 4;

static const int8_t LEVEL_LOW = 1;
static const int8_t LEVEL_MIN = 2;
static const int8_t LEVEL_MED = 3;
static const int8_t LEVEL_MAX = 4;
static const int8_t LEVELS[MAX_LEVELS] = {LEVEL_LOW, LEVEL_MIN, LEVEL_MED, LEVEL_MAX};

static const int8_t LED_LOW = 27;
static const int8_t LED_MIN = 14;
static const int8_t LED_MED = 12;
static const int8_t LED_MAX = 13;
static const int8_t STATES[MAX_LEVELS] = {LED_LOW, LED_MIN, LED_MED, LED_MAX};

static const int8_t BUTTON_LOW = 32;
static const int8_t BUTTON_MIN = 33;
static const int8_t BUTTON_MED = 25;
static const int8_t BUTTON_MAX = 26;
static const int8_t BUTTONS[MAX_LEVELS] = {BUTTON_LOW, BUTTON_MIN, BUTTON_MED, BUTTON_MAX};

extern "C" int	getMask(int a);
volatile uint8_t mask = 1;
volatile int8_t level_state = 1;

#include <WiFi.h>

const char* ssid     = "MyPhone";
const char* password = "iseprules";

WiFiServer server(4545);

void setup() {
  Serial.begin(115200);

  // initialize digital pin LED as an output.
  pinMode(LED_LOW, OUTPUT);
  pinMode(LED_MIN, OUTPUT);
  pinMode(LED_MED, OUTPUT);
  pinMode(LED_MAX, OUTPUT);

  pinMode(BUTTON_LOW, INPUT);
  pinMode(BUTTON_MIN, INPUT);
  pinMode(BUTTON_MED, INPUT);
  pinMode(BUTTON_MAX, INPUT);

  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.begin();
  implementMask(mask);
}

void loop() {

  if (WiFi.status() != WL_CONNECTED){
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  }

  WiFiClient client = server.accept();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          if (currentLine.length() == 0) {
            Serial.print("Sending response: ");
            Serial.println(level_state);

            client.println(level_state);
            client.println();
            // the content of the HTTP response follows the header:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }

  doWork();
  delay(100);
}


void doWork()
{
  int level = getLevel();

  if (level > 0)
  {
    mask = getMask(level);
    implementMask(mask);
    level_state = level;
    Serial.print("Changed level to state ");
    Serial.println(level_state);
  } 
}

int getLevel()
{
  int8_t n_button = 0;

  while (n_button < MAX_LEVELS){
    int button_state = digitalRead(BUTTONS[n_button]);
    
    //Serial.print("button ");
    if ( button_state == HIGH ){ 
      //Serial.print(n_button);
      //Serial.println(" is high");
      return LEVELS[n_button];
    } else {
      //Serial.print(n_button);
      //Serial.println(" is low");
    }

    n_button++;
  }
  return 0;
}

void implementMask(int8_t mask)
{
  int8_t n_bit = 0;
  while (n_bit < MAX_LEVELS) {
    if (mask & 0x01) {
      digitalWrite(STATES[n_bit], HIGH);
      Serial.print("1");
    }
    else {
      digitalWrite(STATES[n_bit], LOW);
      Serial.print("0");
    }
    n_bit++;
    mask = mask >> 1;
  }
  Serial.println();
}
