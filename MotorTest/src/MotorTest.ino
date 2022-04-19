/*
 * Project MotorTest
 * Description:
 * Author:
 * Date:
 */
#include <math.h>
#include "TM1637.h"

SYSTEM_MODE(SEMI_AUTOMATIC);
const int LEDSEGCLKPIN = D5;  //  7-Segment Display Clock pin
const int LEDSEGDATPIN = D6;  //  7-Segment Display data pin
const int LEDPIN = D7;
const int MOTORPIN = A3;
int speed;
float t;
  TM1637 LEDSegOne(LEDSEGCLKPIN, LEDSEGDATPIN);

void setup() {
  // put your setup code here, to run once:
pinMode (LEDPIN, OUTPUT);
    pinMode(MOTORPIN, OUTPUT);
    Serial.begin(9600);
  LEDSegOne.clearDisplay();
  LEDSegOne.set(7);
  LEDSegOne.displayNum(8888);
  delay(2000);
  LEDSegOne.displayNum(0);
  LEDSegOne.display(3, '0');
}

void loop() {
  // put your main code here, to run repeatedly:
    t = millis() / 1000.0;
    speed = 127 * sin(2*M_PI*t*0.1) + 127;
    analogWrite(MOTORPIN, 100);
    analogWrite(MOTORPIN, speed);
  LEDSegOne.displayNum(speed);
//   while(1) {
// digitalWrite(LEDPIN, HIGH);
// delay(250);
// digitalWrite(LEDPIN, LOW);
// delay(1000);
    
//   }
}
