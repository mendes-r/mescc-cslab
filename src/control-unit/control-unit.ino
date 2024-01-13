static const uint8_t PUMP_1 = 18;
static const uint8_t PUMP_CONTROL_1 = 19;
static const uint8_t PUMP_STATE_1 = 21;


void setup() {
  Serial.begin(9600);

  pinMode(PUMP_1, OUTPUT);
  pinMode(PUMP_STATE_1, OUTPUT);
  pinMode(PUMP_CONTROL_1, INPUT);
}

void loop() {
  digitalWrite(PUMP_1, HIGH);

  Serial.println("Checking pumps");
  updatePumpFailureState();
  delay(1000);

  digitalWrite(PUMP_1, LOW);
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
}
