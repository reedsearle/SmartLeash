/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/reed_/Documents/IoT/SmartLeash/LoRaArgonTX/src/LoRaArgonTX.ino"
/*
 * Project       LoRaArgonTX
 * Description:  transmit side of Smart Leash
 * Author:       Reed Searle
 * Date:         11 April 2022
 * Preliminary RYLR896_Transmitter code by Christian Chavez
 */

#include <HX711.h>
#include "TM1637.h"
#include <math.h>

void setup();
void loop();
void loadCellSetup(HX711 scaleName, int scaleFactor);
void LEDDisplaySetup(TM1637 displayName);
void LoRaTXByte(int sendValue);
void BLE_Setup();
void reyaxSetup();
void BLE_TX(int sendValue);
void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context);
void intToAcsii(uint8_t *array, int elementStart, int intValue);
#line 13 "c:/Users/reed_/Documents/IoT/SmartLeash/LoRaArgonTX/src/LoRaArgonTX.ino"
SYSTEM_MODE(SEMI_AUTOMATIC);

//****************************
// Constants
//****************************
const int LEDSEGCLKPIN = D7;      //  7-Segment Display Clock pin
const int LEDSEGDATPIN = D8;      //  7-Segment Display data pin
const int LC1SCLKPIN   = A1;      //  Load Cell One Clock pin
const int LC1DATAPIN   = A0;      //  Load Cell One data pin
const int LC2SCLKPIN   = D13;     //  Load Cell Two Clock pin
const int LC2DATAPIN   = D12;     //  Load Cell Two data pin

const int SCALE_FACTOR1 = 30706;  //  Calibration value for Load Cell One
const int SCALE_FACTOR2 = 41271;  //  Calibration value for Load Cell Two
const int SAMPLE_NUM    = 10;     //  Number of averaged points per data read
const int BLE_SEND_TIME = 10000;  //  Send data to phone every 10 seconds
const int LoRa_SEND_TIME = 1000;  //  Send data to collar every 1 seconds


//****************************
// Variables
//****************************
//  Time & timing Variables
unsigned long BLETime;      //  Timer for Bluetooth transmission
unsigned long LoRaTime;     //  Timer for LoRa transmission

//  Load Cell Variables
float         weight1;      //  calibrated weight from load cell one
float         weight2;      //  calibrated weight from load cell Two
unsigned int  weight;       //  Total weight full scale = 255
unsigned int  weightMax;    //  Max leash pull in ten second invterval

String password = "FABC1232EEDCAA90FABC0002EEDCAA92"; //  Password for LoRa encryption
String network = "15";                                //  Network address for this leash/collar set
String address = "66";                                //  Address of Leash (for potential two-way comm


//****************************
// Setup BLE UART
//****************************
const size_t UART_TX_BUF_SIZE = 20;
uint8_t      txBuf[UART_TX_BUF_SIZE];
uint8_t      i;

// These UUIDs were defined by Nordic Semiconductor and are now the defacto standard for
// UART-like services over BLE. Many apps support the UUIDs now, like the Adafruit Bluefruit app.
const BleUuid      serviceUuid("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid      rxUuid     ("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid      txUuid     ("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");

BleCharacteristic  txCharacteristic("tx", BleCharacteristicProperty::NOTIFY, txUuid, serviceUuid);
BleCharacteristic  rxCharacteristic("rx", BleCharacteristicProperty::WRITE_WO_RSP, rxUuid, serviceUuid, onDataReceived, NULL);
BleAdvertisingData data;



//****************************
// Constructors
//****************************
  HX711  scaleOne(LC1DATAPIN, LC1SCLKPIN);
  HX711  scaleTwo(LC2DATAPIN, LC2SCLKPIN);
  TM1637 LEDSegOne(LEDSEGCLKPIN, LEDSEGDATPIN);




//****************************
//****************************
//      SETUP
//****************************
//****************************  
void setup(){
  RGB.control(true);                  //  Turn on control of onboard Argon RGB LED
  RGB.color(0xff0000);                //  Set RGB to Red indicating beginning of setup

  //  Serial Ports Init
  Serial.begin(9600);                 //  Start serial port to user
  Serial1.begin(115200);              //  Start Serial1 port to LoRa
  delay(5000);                        //  Let Serial ports finish initializing and scales to settle
  Serial.printf("Serial Begin\n");

  // Initialize scales
  scaleOne.set_scale();               // Initialize scale One
  delay(5000);                        //  Wait for scale One to settle
  scaleOne.tare();                    //  Initialize the tare weight of Scale One
  scaleOne.set_scale(SCALE_FACTOR1);  //  Set calibration value for Scale One
  Serial.printf("finished Scale 1\n");

  scaleTwo.set_scale();               // Initialize scaleTwo
  delay(5000);                        //  Wait for scaleTwo to settle
  scaleTwo.tare();                    //  Initialize the tare weight of scaleTwo
  scaleTwo.set_scale(SCALE_FACTOR2);  //  Set calibration value scaleTwo
  Serial.printf("finished Scale 1\n");

 //  Initialize 7-Segment Display
  LEDDisplaySetup(LEDSegOne);
  Serial.printf("finished 7-Seg \n");

  //  Initialize BlueTooth
  BLE_Setup();

  //  Reyax RVLR890 Setup
  reyaxSetup();

  // Initialize global variables
  weightMax = 0;
  RGB.color(0x00FF00);        //  Set RGB to Green indicating end of setup

}




//****************************
//****************************
//      loop
//****************************
//****************************  
void loop(){
    //  Send correction to collar via LoRa
    if (millis() > LoRaTime + LoRa_SEND_TIME){             //  Check for transmit interval
        LoRaTXByte(weight);                                //  Send correction value
        Serial.printf("Sending value: 0x%02x\n", weight);
        LoRaTime = millis();                               //  Reset LoRa transmit timer
        }
    if(Serial1.available()) {                              //  Check for reply from LoRa TXRX
        Serial.println(Serial1.readString());              //  Print reply
    }

    //  find pull force on leash and create correction
    weight1 = scaleOne.get_units(SAMPLE_NUM);              //  Get pull value from load cell one
    Serial.printf("Got Scale 1: %0.2f\n", weight1);
    weight2 = scaleTwo.get_units(SAMPLE_NUM);              //  Get pull value from load cell two
    Serial.printf("Got Scale 2: %0.2f\n", weight2);
    weight = abs(weight1) + abs(weight2);                  //  Determine total pull
    if(weight > weightMax) {                               //  Update Maximum pull value
        weightMax = weight;
    }
    Serial.printf("Weight: %i Weight1: %0.2f Weight2: %0.2f \n", weight, weight1, weight2);

    if (millis() > BLETime + BLE_SEND_TIME) {              //  Check for BLER interval
      BLE_TX(weightMax * 40 / 255);                        //  Send maximum leash pull to bluetooth in pounds force
      weightMax = 0;                                       //  Reset max weight
      BLETime = millis();                                  //  Reset timer
    }

  if(abs(weight) < 1.0) {
    LEDSegOne.displayNum(0);
    LEDSegOne.display(3, '0');  //  Display character "0" because TM1637.cpp suppresses leading zeros, including when value is zero
  } else {
    LEDSegOne.displayNum(weight);
  }
    delay(1000);
}



//****************************
//****************************
//      loadCellSetup
//****************************
//****************************  
void loadCellSetup(HX711 scaleName, int scaleFactor) {
  scaleName.set_scale();               // Initialize scaleName
  delay(5000);                         //  Wait for scaleName to settle
  scaleName.tare();                    //  Initialize the tare weight of scaleName
  scaleName.set_scale(SCALE_FACTOR2);  //  Set calibration value scaleName
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




//****************************
//****************************
//      LoRaTXByte
//****************************
//****************************  
void LoRaTXByte(int sendValue) {
    char cmd[16];                                    //  Create temp char array
    sprintf(cmd, "AT+SEND=59,1,%c\r\n", sendValue);  //  Create string to send
    Serial1.println(cmd);                            //  Send string to LoRa
    Serial.println(cmd);
    return;
}




//****************************
//****************************
//      BLE_Setup
//****************************
//****************************  
void BLE_Setup() {
  BLE.on();
  Serial.printf("Starting up BLE Connection \n");
  BLE.addCharacteristic(txCharacteristic);
  BLE.addCharacteristic(rxCharacteristic);
  data.appendServiceUUID(serviceUuid);
  BLE.advertise(&data);
  Serial.printf("Argon BLE Address: %s\n",BLE.address().toString().c_str());
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
//      BLE_TX
//****************************
//****************************  
void BLE_TX(int sendValue) {
    sprintf((char *) txBuf, "Max pull: ");      //  Create send String
    intToAcsii(txBuf, 10, sendValue);           //  Append integer value
    txBuf[13] = 0x0A;                           //  Append end value
    txCharacteristic.setValue(txBuf, 16);       //  Send to Bluetooth
    return;
}





//****************************
//****************************
//      onDataReceived
//****************************
//****************************  
void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) {
    uint8_t i;

    //  Print address of Bluetooth sender
    Serial.printf("Received data from: %02X:%02X:%02X:%02X:%02X:%02X \n", peer.address()[0], peer.address()[1],peer.address()[2], peer.address()[3], peer.address()[4], peer.address()[5]);
    //  Print actual characters sent
    Serial.printf("Bytes: ");
    for (i = 0; i < len; i++) {
        Serial.printf("%02X ",data[i]);
    }
    Serial.printf("\n");
    //  Print as message
    Serial.printf("Message: %s\n",(char *)data);
    
}




//****************************
//****************************
//      intToAcsii
//****************************
//****************************  
void intToAcsii(uint8_t *array, int elementStart, int intValue) {
    array[elementStart+2] = (intValue % 10) + 0x30;  //  Populate ones portion of value
    intValue = (intValue - (intValue%10)) / 10;      //  Decimal right shift
    array[elementStart+1] = (intValue % 10) + 0x30;  //  populate tens portion of value
    intValue = (intValue - (intValue%10) ) /10;      //  Decimal right shift
    array[elementStart+0] = (intValue % 10) + 0x30;  //  Populate hundreds portion of value
    return;
}
