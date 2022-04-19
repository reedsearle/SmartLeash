/*
 * Project       LoRaArgonRX
 * Description:  Receive side of SmartLeash (collar)
 * Author:       Reed Searle
 * Date:         11 April 2022
  * Preliminary RYLR896_Transmitter code by Christian Chavez
*/

#include <math.h>
#include "TM1637.h"

SYSTEM_MODE(SEMI_AUTOMATIC); //  keep device from connecting to internet automatically

//****************************
// Constants
//****************************
const int LEDSEGCLKPIN = D6;  //  7-Segment Display Clock pin
const int LEDSEGDATPIN = D5;  //  7-Segment Display data pin
const int MOTORPIN     = A3;  //  Output to motor control circuit

//settings for RYLR896 module
String password = "FABC1232EEDCAA90FABC0002EEDCAA92"; //  Password for LoRa encryption
String network = "15";                                //  Network address for this leash/collar set
String address = "59";                                //  Address of Leash (for potential two-way comm

byte motorSpeed;               //  Received correction value for controlling motor speed

//****************************
// Constructors
//****************************
  TM1637 LEDSegOne(LEDSEGCLKPIN, LEDSEGDATPIN);  //  7-segment display




//****************************
//****************************
//      SETUP
//****************************
//****************************  
void setup(){
  RGB.control(true);                  //  Turn on control of onboard Argon RGB LED
  RGB.color(0xff0000);                //  Set RGB to Red indicating beginning of setup

    pinMode(MOTORPIN, OUTPUT);        //  Set motor control pin as output

  //  Serial Ports Init
  Serial.begin(9600);                 //  Start serial port to user
  Serial1.begin(115200);              //  Start Serial1 port to LoRa
  delay(5000);                        //  Let Serial ports finish initializing and scales to settle
  Serial.printf("Serial Begin\n");

      //  Initialize 7-Segment Display
    LEDDisplaySetup(LEDSegOne);
    Serial.printf("finished 7-Seg \n");

  //  Reyax RVLR890 Setup
  reyaxSetup();

  RGB.color(0x00FF00);                 //  Set RGB to Green indicating end of setup

}




//****************************
//****************************
//      loop
//****************************
//****************************  
void loop(){
    if (Serial1.available()){                //  Check for incoming transmission
        motorSpeed = LoRaRXByte();           //  Read the message and get the correction factor
        analogWrite(MOTORPIN, motorSpeed);   //  Send correction factor to the motor
          if(abs(motorSpeed) < 1.0) {
            LEDSegOne.displayNum(0);
            LEDSegOne.display(3, '0');  //  Display character "0" because TM1637.cpp suppresses leading zeros, including when value is zero
          } else {
            LEDSegOne.displayNum(motorSpeed);
          }

    }
}




//****************************
//****************************
//      LoRaRXByte
//****************************
//****************************  
byte LoRaRXByte() {
        byte byteValue;             //  Returned byte value
        unsigned char inBuf[20];    //  Temp char array

        (Serial1.readString()).getBytes(inBuf, 18);  //  Fill temp array with incoming message
        byteValue = inBuf[10];      //  Separate out data
        return byteValue;
}




//****************************
//****************************
//      reyaxSetup
//****************************
//****************************  
void reyaxSetup(){ //delays allow time to confirm AT command
    Serial1.printf("AT+NETWORKID="+network+"\r\n");  //set network id from 0 to 16
    delay(200);
    if(Serial1.available()) {                        //  Check for reply from LoRa TXRX
        Serial.write(Serial1.readString());          //  Print reply
    }
    Serial1.printf("AT+CPIN="+password+"\r\n");      //set password (comment out if not using password)
    delay(200);
    if(Serial1.available()) {                        //  Check for reply from LoRa TXRX
        Serial.write(Serial1.readString());          //  Print reply
    }
    Serial1.printf("AT+ADDRESS="+address+"\r\n");    //set device address from 0 to 65535
    delay(200);
    if(Serial1.available()) {                        //  Check for reply from LoRa TXRX
        Serial.write(Serial1.readString());          //  Print reply
    }
    Serial1.printf("AT\r\n");                        //check AT status
    delay(200);
    if(Serial1.available()) {                        //  Check for reply from LoRa TXRX
        Serial.write(Serial1.readString());          //  Print reply
    }
    Serial1.printf("AT+PARAMETER?\r\n");             //check the RF parameters
    delay(200);
    if(Serial1.available()) {                        //  Check for reply from LoRa TXRX
        Serial.write(Serial1.readString());          //  Print reply
    }
    Serial1.printf("AT+BAND?\r\n");                  //check module band
    delay(200);
    if(Serial1.available()) {                        //  Check for reply from LoRa TXRX
        Serial.write(Serial1.readString());          //  Print reply
    }
    Serial1.printf("AT+ADDRESS?\r\n");               //check module address
    delay(200);
    if(Serial1.available()) {                        //  Check for reply from LoRa TXRX
        Serial.write(Serial1.readString());          //  Print reply
    }
    Serial1.printf("AT+NETWORKID?\r\n");             //check module network id
    delay(200);
    if(Serial1.available()) {                        //  Check for reply from LoRa TXRX
        Serial.write(Serial1.readString());          //  Print reply
    }
    Serial1.printf("AT+CPIN?\r\n");                  //check the AES128 password
    delay(200);
    if(Serial1.available()) {                        //  Check for reply from LoRa TXRX
        Serial.write(Serial1.readString());          //  Print reply
    }
    Serial1.printf("AT+VER?\r\n");                   //check the firmware version
    delay(200);
    if(Serial1.available()) {                        //  Check for reply from LoRa TXRX
        Serial.write(Serial1.readString());          //  Print reply
    }
    Serial1.printf("AT+UID?\r\n");                   //check the unique ID of the module
    delay(200);
    if(Serial1.available()) {                        //  Check for reply from LoRa TXRX
        Serial.write(Serial1.readString());          //  Print reply
    }
}




//****************************
//****************************
//      LEDDisplaySetup
//****************************
//****************************  
void LEDDisplaySetup(TM1637 displayName) {
  displayName.clearDisplay();
  displayName.set(7);
  displayName.displayNum(8888);
  delay(2000);
  displayName.displayNum(0);
  displayName.display(3, '0');
}

