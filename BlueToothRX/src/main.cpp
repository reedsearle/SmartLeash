/*
 * Project       LoadCellTest
 * Description:  Test Load Cells and 7seg
 * Author:       Reed Searle
 * Date:         14 april 2022
 */

#include <Arduino.h>
#include "TM1637.h"
#include "HC05.h"
#include <SoftwareSerial.h>

const int BTCMDPIN     = A9;  //  HC-05 Enable Pin
const int BTSTATEPIN   = A8;  //  HC-05 State Pin
const int BTRXPIN      = A7;  //  HC-05 Rx Pin
const int BTTXPIN      = A6;  //  HC-05 Tx Pin
const int CLKPIN       = A3;  //  7 Segment Display Clock Pin
const int DIOPIN       = A2;  //  7 Segment Display Data Pin

  int i;

//****************************
// Constructors
//****************************
  TM1637 LEDSegOne(CLKPIN, DIOPIN);
//  HC05 btSerial = HC05(A2, A5, A3, A4);  // cmd, state, rx, tx
  SoftwareSerial BTXiao(BTRXPIN, BTTXPIN);


void setup() {
  pinMode(BTCMDPIN, OUTPUT);
  digitalWrite(BTCMDPIN, HIGH);
  // Initialize Serial port
  Serial.begin(9600);
  BTXiao.begin(38400);
  delay(5000);
  Serial.printf("Serial port alive\n\a");

//  Initialize 7-Segment Display
  LEDSegOne.clearDisplay();
  LEDSegOne.set(7);
  LEDSegOne.displayNum(8888);
  delay(2000);
  LEDSegOne.displayNum(0);

// Initialize Bluetooth
  // DEBUG_BEGIN(57600);
  // btSerial.findBaud();

  for(i = 0; i <=100; i++) {
    LEDSegOne.displayNum(i);
    delay(250);
  }
    Serial.printf("Setup done\n");

}

void loop() {
  while (Serial.available()) {
      delay(1);
      BTXiao.write(Serial.read());
  }
  while (BTXiao.available()) {
       Serial.write(BTXiao.read());
  }
}