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

void setup() {
  Serial.begin(9600);

  pinMode(ALERT, OUTPUT);

  pinMode(PUMP_1, OUTPUT);
  pinMode(PUMP_STATE_1, OUTPUT);
  pinMode(PUMP_CONTROL_1, INPUT);

  pinMode(PUMP_2, OUTPUT);
  pinMode(PUMP_STATE_2, OUTPUT);
  pinMode(PUMP_CONTROL_2, INPUT);

  digitalWrite(ALERT, LOW);
}

void loop() {
  controlPumps(LEVEL_MED);
  delay(2000);  
}

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
