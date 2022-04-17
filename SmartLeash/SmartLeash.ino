const int LEDPIN = PB6;


void setup() {
  // put your setup code here, to run once:
pinMode (LED_BUILTIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  while(1) {
digitalWrite(LED_BUILTIN, HIGH);
delay(250);
digitalWrite(LED_BUILTIN, LOW);
delay(1000);
    
  }
}
