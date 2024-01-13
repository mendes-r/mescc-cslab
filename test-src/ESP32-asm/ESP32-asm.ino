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

void setup() {
  Serial.begin(9600);

  // initialize digital pin LED as an output.
  pinMode(LED_LOW, OUTPUT);
  pinMode(LED_MIN, OUTPUT);
  pinMode(LED_MED, OUTPUT);
  pinMode(LED_MAX, OUTPUT);

  pinMode(BUTTON_LOW, INPUT);
  pinMode(BUTTON_MIN, INPUT);
  pinMode(BUTTON_MED, INPUT);
  pinMode(BUTTON_MAX, INPUT);

  implementMask(mask);
}

void loop() {
  int level = getLevel();

  if (level > 0)
  {
    mask = getMask(level);
    implementMask(mask);
    level_state = level;
    Serial.print("Changed level to state ");
    Serial.println(level_state);
  } 

  delay(1000);                      
}

int getLevel()
{
  int8_t n_button = 0;

  while (n_button < MAX_LEVELS){
    int button_state = digitalRead(BUTTONS[n_button]);
    
    Serial.print("button ");
    if ( button_state == HIGH ){ 
      Serial.print(n_button);
      Serial.println(" is high");
      return LEVELS[n_button];
    } else {
      Serial.print(n_button);
      Serial.println(" is low");
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
