/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/reed_/Documents/IoT/SmartLeash/LoRaArgonRX/src/LoRaArgonRX.ino"
/*
 * Project LoRaArgonRX
 * Description:
 * Author:
 * Date:
 */
/*
 * Project: RYLR896_Receive
 * Description: Receive data via LoRa using a Particle Gen 3 microcontroller and REYAX RYLR896
 * Author: Christian Chavez
 * Date: 1646685874 UNIX
 * Modified by Reed Searle to receive raw data instead of converting from characters
 *
 * REYAX RYLR896 Datasheet:
 * https://reyax.com/tw/wp-content/uploads/2019/12/RYLR896_EN.pdf
 *
 * AT Command Manual:
 * https://reyax.com/wp-content/uploads/2020/01/Lora-AT-Command-RYLR40x_RYLR89x_EN.pdf
 */
#include <math.h>
#include "TM1637.h"

void setup();
void loop();
byte LoRaRXByte();
void reyaxReset();
void reyaxSetup();
void LEDDisplaySetup(TM1637 displayName);
#line 23 "c:/Users/reed_/Documents/IoT/SmartLeash/LoRaArgonRX/src/LoRaArgonRX.ino"
SYSTEM_MODE(SEMI_AUTOMATIC); //keep device from connecting to internet automatically

//****************************
// Constants
//****************************
const int LEDSEGCLKPIN = D6;  //  7-Segment Display Clock pin
const int LEDSEGDATPIN = D5;  //  7-Segment Display data pin
const int MOTORPIN     = A3;

//settings for RYLR896 module
String password = "FABC1232EEDCAA90FABC0002EEDCAA92"; //(optional) module will only read data from modules with matching password. Using reyaxReset() removes the password.
String network = "15"; //must match address set on transmit module - range from 0 to 16
String address = "59"; //must match network set on transmit module - range from 0 to 65535

char value;

byte motorSpeed;

//****************************
// Constructors
//****************************
  TM1637 LEDSegOne(LEDSEGCLKPIN, LEDSEGDATPIN);




//****************************
//****************************
//      SETUP
//****************************
//****************************  
void setup(){
    RGB.control(true);
    RGB.color(0xff0000);

    pinMode(MOTORPIN, OUTPUT);
    Serial.begin(9600);
    Serial1.begin(115200);
    delay(5000);
    Serial.printf("Serial begin\n");

      //  Initialize 7-Segment Display
    LEDDisplaySetup(LEDSegOne);
    Serial.printf("finished 7-Seg \n");

    reyaxSetup(); //sets module preferences
         RGB.color(0x00FF00);

}

void loop(){
    if (Serial1.available()){
        motorSpeed = LoRaRXByte();
        analogWrite(MOTORPIN, motorSpeed);
          if(abs(motorSpeed) < 1.0) {
            LEDSegOne.displayNum(0);
            LEDSegOne.display(3, '0');  //  Display character "0" because TM1637.cpp suppresses leading zeros, including when value is zero
          } else {
            LEDSegOne.displayNum(motorSpeed);
          }

    }
}

byte LoRaRXByte() {
        byte byteValue;
        unsigned char inBuf[20];

        (Serial1.readString()).getBytes(inBuf, 18);
        byteValue = inBuf[10];
        return byteValue;
}

void reyaxReset(){
    Serial1.printf("AT+FACTORY\r\n"); //set all parameters to factory defaults
    delay(50);
    Serial1.printf("AT+RESET\r\n"); //resets all parameters including CPIN password--to run this command, comment out all other AT commands in void setup.
}

void reyaxSetup(){ //delays allow time to confirm AT command
    Serial1.printf("AT+NETWORKID="+network+"\r\n"); //set network id from 0 to 16
    delay(200);
    if(Serial1.available()) {
        Serial.write(Serial1.readString());
    }
    Serial1.printf("AT+CPIN="+password+"\r\n"); //set password (comment out if not using password)
    delay(200);
    if(Serial1.available()) {
        Serial.write(Serial1.readString());
    }
    Serial1.printf("AT+ADDRESS="+address+"\r\n"); //set device address from 0 to 65535
    delay(200);
    if(Serial1.available()) {
        Serial.write(Serial1.readString());
    }
    Serial1.printf("AT\r\n"); //check AT status
    delay(200);
    if(Serial1.available()) {
        Serial.write(Serial1.readString());
    }
    Serial1.printf("AT+PARAMETER?\r\n"); //check the RF parameters
    delay(200);
    if(Serial1.available()) {
        Serial.write(Serial1.readString());
    }
    Serial1.printf("AT+BAND?\r\n"); //check module band
    delay(200);
    if(Serial1.available()) {
        Serial.write(Serial1.readString());
    }
    Serial1.printf("AT+ADDRESS?\r\n"); //check module address
    delay(200);
    if(Serial1.available()) {
        Serial.write(Serial1.readString());
    }
    Serial1.printf("AT+NETWORKID?\r\n"); //check module network id
    delay(200);
    if(Serial1.available()) {
        Serial.write(Serial1.readString());
    }
    Serial1.printf("AT+CPIN?\r\n"); //check the AES128 password
    delay(200);
    if(Serial1.available()) {
        Serial.write(Serial1.readString());
    }
    Serial1.printf("AT+VER?\r\n"); //check the firmware version
    delay(200);
    if(Serial1.available()) {
        Serial.write(Serial1.readString());
    }
    Serial1.printf("AT+UID?\r\n"); //check the unique ID of the module
    delay(200);
    if(Serial1.available()) {
        Serial.write(Serial1.readString());
    }
}



void LEDDisplaySetup(TM1637 displayName) {
  displayName.clearDisplay();
  displayName.set(7);
  displayName.displayNum(8888);
  delay(2000);
  displayName.displayNum(0);
  displayName.display(3, '0');
}

