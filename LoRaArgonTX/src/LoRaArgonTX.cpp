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
const int LEDSEGCLKPIN = D7;  //  7-Segment Display Clock pin
const int LEDSEGDATPIN = D8;  //  7-Segment Display data pin
const int LC1SCLKPIN   = A1;  //  Load Cell One Clock pin
const int LC1DATAPIN   = A0;  //  Load Cell One data pin
const int LC2SCLKPIN   = D13;  //  Load Cell Two Clock pin
const int LC2DATAPIN   = D12;  //  Load Cell Two data pin

// const int SCALE_FACTOR1 = 195748;  //  Calibration value for Load Cell One
// const int SCALE_FACTOR2 = 263101;  //  Calibration value for Load Cell Two
const int SCALE_FACTOR1 = 30706;  //  Calibration value for Load Cell One
const int SCALE_FACTOR2 = 41271;  //  Calibration value for Load Cell Two
const int SAMPLE_NUM    = 10;    //  Number of averaged points per data read
const int BLE_SEND_TIME = 10000;  //  Send data to phone every 10 seconds


//****************************
// Variables
//****************************
  //  Time & timing Variables
  String     dateTime, timeOnly, timeOnlyOld;   
  int endTime;

  //  Load Cell Variables
  float weight1;    //  calibrated weight from load cell
  float weight2;    //  calibrated weight from load cell
  unsigned int weight;    //  calibrated weight from load cell
  unsigned int weightMax;    //  calibrated weight from load cell

  unsigned long lastTransmission;
  const int interval = 1000;

  byte value;

  String password = "FABC1232EEDCAA90FABC0002EEDCAA92"; //must match password set on receiving module...best security practice to define password in seprate .h file
  String network = "15"; //must match address set on transmit module - range from 0 to 16
  String address = "66"; //must match network set on transmit module - range from 0 to 65535


//****************************
// Setup BLE UART
//****************************
const size_t UART_TX_BUF_SIZE = 20;
uint8_t txBuf[UART_TX_BUF_SIZE];
uint8_t i;

// These UUIDs were defined by Nordic Semiconductor and are now the defacto standard for
// UART-like services over BLE. Many apps support the UUIDs now, like the Adafruit Bluefruit app.
const BleUuid serviceUuid("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid rxUuid("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid txUuid("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");

BleCharacteristic txCharacteristic("tx", BleCharacteristicProperty::NOTIFY, txUuid, serviceUuid);
BleCharacteristic rxCharacteristic("rx", BleCharacteristicProperty::WRITE_WO_RSP, rxUuid, serviceUuid, onDataReceived, NULL);
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
    RGB.control(true);
    RGB.color(0xff0000);

  //  Serial Ports Init
  Serial.begin(9600);
  Serial1.begin(115200);
  delay(5000);  //  Let Serial ports finish initializing and scales to settle
  Serial.printf("Serial Begin\n");

  // Initialize scales
  scaleOne.set_scale(); // Initialize sscale
  delay(5000);
  scaleOne.tare();  //  Initialize the tare weight
  scaleOne.set_scale(SCALE_FACTOR1);
  // loadCellSetup(&scaleOne, SCALE_FACTOR1);
  Serial.printf("finished Scale 1\n");
  scaleTwo.set_scale(); // Initialize sscale
  delay(5000);
  scaleTwo.tare();  //  Initialize the tare weight
  scaleTwo.set_scale(SCALE_FACTOR1);
  // loadCellSetup(&scaleTwo, SCALE_FACTOR2);
  Serial.printf("finished Scale 2\n");

 //  Initialize 7-Segment Display
  LEDDisplaySetup(LEDSegOne);
  Serial.printf("finished 7-Seg \n");

  //  Initialize BlueTooth
  BLE_Setup();

  //  Reyax RVLR890 Setup
  reyaxSetup();

  weightMax = 0;

  RGB.color(0x00FF00);

}

void loop(){
    if (millis() > lastTransmission + interval){
        LoRaTXByte(weight);
        Serial.printf("Sending value: 0x%02x\n", weight);
        lastTransmission = millis();
        }
    if(Serial1.available()) {
        Serial.println(Serial1.readString());

    }

    weight1 = scaleOne.get_units(SAMPLE_NUM);
    Serial.printf("Got Scale 1: %0.2f\n", weight1);
    weight2 = scaleTwo.get_units(SAMPLE_NUM);
    Serial.printf("Got Scale 2: %0.2f\n", weight2);
    weight = abs(weight1) + abs(weight2);
    if(weight > weightMax) {
        weightMax = weight;
    }
    Serial.printf("Weight: %i Weight1: %0.2f Weight2: %0.2f \n", weight, weight1, weight2);

    if (millis() > endTime + BLE_SEND_TIME) {
      BLE_TX(weightMax);  
      weightMax = 0;
      endTime = millis(); 
    }

  if(abs(weight) < 1.0) {
    LEDSegOne.displayNum(0);
    LEDSegOne.display(3, '0');  //  Display character "0" because TM1637.cpp suppresses leading zeros, including when value is zero
  } else {
    LEDSegOne.displayNum(weight);
  }
    delay(1000);
}



void loadCellSetup(HX711 scaleName, int scaleFactor) {
  scaleName.set_scale(); // Initialize sscale
  delay(5000);
  scaleName.tare();  //  Initialize the tare weight
  scaleName.set_scale(scaleFactor);
}



void LEDDisplaySetup(TM1637 displayName) {
  displayName.clearDisplay();
  displayName.set(7);
  displayName.displayNum(8888);
  delay(2000);
  displayName.displayNum(0);
  displayName.display(3, '0');
}



void LoRaTXByte(int sendValue) {
    char cmd[16];
    sprintf(cmd, "AT+SEND=59,1,%c\r\n", sendValue);
    Serial1.println(cmd);
    Serial.println(cmd);
    return;
}

void BLE_Setup() {
  BLE.on();
  Serial.printf("Starting up BLE Connection \n");
  BLE.addCharacteristic(txCharacteristic);
  BLE.addCharacteristic(rxCharacteristic);
  data.appendServiceUUID(serviceUuid);
  BLE.advertise(&data);
  Serial.printf("Argon BLE Address: %s\n",BLE.address().toString().c_str());
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


void BLE_TX(int sendValue) {
    uint8_t TxBufTemp[3];
    sprintf((char *) txBuf, "Max pull: ");
    intToAcsii(txBuf, 10, sendValue);
    txBuf[13] = 0x0A;
    txCharacteristic.setValue(txBuf, 16);
    return;
}


//onDataReceived is used to recieve data from Bluefruit Connect App
void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) {
    uint8_t i;

    Serial.printf("Received data from: %02X:%02X:%02X:%02X:%02X:%02X \n", peer.address()[0], peer.address()[1],peer.address()[2], peer.address()[3], peer.address()[4], peer.address()[5]);
    Serial.printf("Bytes: ");
    for (i = 0; i < len; i++) {
        Serial.printf("%02X ",data[i]);
    }
    Serial.printf("\n");
    Serial.printf("Message: %s\n",(char *)data);
    
}

void intToAcsii(uint8_t *array, int elementStart, int intValue) {
    array[elementStart+2] = (intValue % 10) + 0x30;  //  Populate ones portion of value
    intValue = (intValue - (intValue%10)) / 10;      //  Decimal right shift
    array[elementStart+1] = (intValue % 10) + 0x30;  //  populate tens portion of value
    intValue = (intValue - (intValue%10) ) /10;      //  Decimal right shift
    array[elementStart+0] = (intValue % 10) + 0x30;  //  Populate hundreds portion of value
    return;
}
