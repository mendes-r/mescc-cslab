#define ALARM_PIN 12

#define PUMP_1_PIN 14
#define PUMP_2_PIN 27

#define WATER_LEVEL_LOW_PIN 26
#define WATER_LEVEL_MIN_PIN 25
#define WATER_LEVEL_MED_PIN 33
#define WATER_LEVEL_MAX_PIN 32

#define BUZZ_PIN  18
#define BUZZ_BOTTON_PIN  19

void setup() {

  pinMode(ALARM_PIN, OUTPUT);
  pinMode(BUZZ_PIN, OUTPUT);
  pinMode(PUMP_1_PIN, OUTPUT);
  pinMode(PUMP_2_PIN, OUTPUT);
  pinMode(WATER_LEVEL_LOW_PIN, OUTPUT);
  pinMode(WATER_LEVEL_MIN_PIN, OUTPUT);
  pinMode(WATER_LEVEL_MED_PIN, OUTPUT);
  pinMode(WATER_LEVEL_MAX_PIN, OUTPUT);
  pinMode(BUZZ_BOTTON_PIN, OUTPUT);

  pinMode(2, OUTPUT);
}

void loop() {

  digitalWrite(ALARM_PIN, HIGH);
  digitalWrite(BUZZ_PIN, HIGH);
  digitalWrite(PUMP_1_PIN, HIGH);
  digitalWrite(PUMP_2_PIN, HIGH);
  digitalWrite(WATER_LEVEL_LOW_PIN, HIGH);
  digitalWrite(WATER_LEVEL_MIN_PIN, HIGH);
  digitalWrite(WATER_LEVEL_MED_PIN, HIGH);
  digitalWrite(WATER_LEVEL_MAX_PIN, HIGH);
  digitalWrite(BUZZ_BOTTON_PIN, HIGH);

  digitalWrite(2, HIGH);
  delay(1000);                      
  
  digitalWrite(ALARM_PIN, LOW);
  digitalWrite(BUZZ_PIN, LOW);
  digitalWrite(PUMP_1_PIN, LOW);
  digitalWrite(PUMP_2_PIN, LOW);
  digitalWrite(WATER_LEVEL_LOW_PIN, LOW);
  digitalWrite(WATER_LEVEL_MIN_PIN, LOW);
  digitalWrite(WATER_LEVEL_MED_PIN, LOW);
  digitalWrite(WATER_LEVEL_MAX_PIN, LOW);
  digitalWrite(BUZZ_BOTTON_PIN, LOW);

  digitalWrite(2, LOW);
  delay(1000);                      
}
