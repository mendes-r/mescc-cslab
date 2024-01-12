static const int LED_BUILTIN = 2;

// put function declarations here:
extern "C" int	getMatrix(int a);

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  Serial.println("Hello World");
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  volatile uint32_t e = 0;

  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second

  Serial.print("Valor de e: ");
  Serial.println(e);

  e = getMatrix(4);
  Serial.print("Resultado: ");
  Serial.println(e);
  Serial.println();

  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);                      // wait for a second
}
