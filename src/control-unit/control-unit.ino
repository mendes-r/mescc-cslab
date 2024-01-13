static const uint8_t PUMP_1 = 13;
static const uint8_t PUMP_STATE_1 = 12;
static const uint8_t PUMP_CONTROL_1 = 14;

static const uint8_t PUMP_2 = 26;
static const uint8_t PUMP_STATE_2 = 25;
static const uint8_t PUMP_CONTROL_2 = 27;


void setup() {
  Serial.begin(9600);

  pinMode(PUMP_1, OUTPUT);
  pinMode(PUMP_STATE_1, OUTPUT);
  pinMode(PUMP_CONTROL_1, INPUT);

  pinMode(PUMP_2, OUTPUT);
  pinMode(PUMP_STATE_2, OUTPUT);
  pinMode(PUMP_CONTROL_2, INPUT);
}

void loop() {
  digitalWrite(PUMP_1, HIGH);
  digitalWrite(PUMP_2, HIGH);

  Serial.println("Checking pumps");
  updatePumpFailureState();
  delay(1000);

  digitalWrite(PUMP_1, LOW);
  digitalWrite(PUMP_2, LOW);
  delay(1000);  
}

void updatePumpFailureState()
{
  if (digitalRead(PUMP_CONTROL_1)){
    Serial.println("Pump 1 is functional");
    digitalWrite(PUMP_STATE_1, HIGH);
  } else {
    Serial.println("Pump 1 FAILURE");
    digitalWrite(PUMP_STATE_1, LOW);
  }

    if (digitalRead(PUMP_CONTROL_2)){
    Serial.println("Pump 2 is functional");
    digitalWrite(PUMP_STATE_2, HIGH);
  } else {
    Serial.println("Pump 2 FAILURE");
    digitalWrite(PUMP_STATE_2, LOW);
  }
}
