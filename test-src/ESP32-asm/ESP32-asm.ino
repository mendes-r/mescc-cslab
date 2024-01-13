static const int8_t LEVEL_LOW = 1;
static const int8_t LEVEL_MIN = 2;
static const int8_t LEVEL_MED = 3;
static const int8_t LEVEL_MAX = 4;

static const int8_t LED_LOW = 27;
static const int8_t LED_MIN = 14;
static const int8_t LED_MED = 12;
static const int8_t LED_MAX = 13;

static const int8_t MAX_SATES = 4;
static const int8_t STATES[MAX_SATES] = {LED_LOW, LED_MIN, LED_MED, LED_MAX};

extern "C" int	getMask(int a);
volatile uint8_t mask = 1;

void setup() {
  Serial.begin(9600);

  // initialize digital pin LED as an output.
  pinMode(2, OUTPUT); // debug
  pinMode(LED_LOW, OUTPUT);
  pinMode(LED_MIN, OUTPUT);
  pinMode(LED_MED, OUTPUT);
  pinMode(LED_MAX, OUTPUT);
}

void loop() {
  digitalWrite(2, HIGH); // debug
  delay(1000);

  mask = getMask(LEVEL_LOW);
  implementMask(mask);

  digitalWrite(2, LOW); // debug
  delay(1000);                      
}

void implementMask(int8_t mask)
{
  int n_bit = 0;
  while (n_bit < MAX_SATES) {
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
