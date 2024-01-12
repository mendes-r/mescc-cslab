static const int8_t LED = 18;

static const int8_t LEVEL_LOW = 1;
static const int8_t LEVEL_MIN = 2;
static const int8_t LEVEL_MED = 3;
static const int8_t LEVEL_MAX = 4;

extern "C" int	getMask(int a);

void setup() {
  Serial.begin(9600);

  // initialize digital pin LED as an output.
  pinMode(LED, OUTPUT);
}

void loop() {
  volatile uint8_t mask = 1;

  digitalWrite(LED, HIGH);  
  delay(1000);                      

  mask = getMask(LEVEL_MED);
  implementMask(mask);

  digitalWrite(LED, LOW);   
  delay(1000);                      
}

void implementMask(int8_t mask)
{
  int n_bit = 0;
  while (n_bit < 8) {
    if (mask & 0x01) {
      Serial.print("1");
    }
    else {
      Serial.print("0");
    }

    n_bit++;
    mask = mask >> 1;
  }
  Serial.println();
}
