/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/reed_/Documents/IoT/SmartLeash/BluetoothArgon/src/BluetoothArgon.ino"
/*
 * Project BluetoothArgon
 * Description:
 * Author:
 * Date:
 */

/*
 * Project L17_00_UART
 * Description: Starter Code for sending / recieving data from Bluefruit Connect app
 * Author: Brian Rashap
 * Date: 28-JUL-20212
 */

#include "math.h"

void setup();
void loop();
void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context);
#line 17 "c:/Users/reed_/Documents/IoT/SmartLeash/BluetoothArgon/src/BluetoothArgon.ino"
const int PIXELPIN = D6;  //  NeoPixel output pin
const int ENCAPIN  = D2;  //  Encoder A pin
const int ENCBPIN  = D3;  //  Encoder B Pin
const int PIXELNUM = 60;  //  NeoPixel ring has 60 pixels
const int PIXELBR  = 15;  //  inrement/decrement brightness every 15 ms

bool UARTFlag;
int ringPosUART;
uint8_t *ptrTxBuf;
int controlCase; //  variable for control pad case statement


// Setup BLE UART

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

//  CONSTRUCTORS

SYSTEM_MODE(SEMI_AUTOMATIC); //Using BLE and not Wifi



void setup() { 

    Serial.begin();
    waitFor(Serial.isConnected, 15000);  

    BLE.on();

    Serial.printf("Starting up BLE Connection \n");

    BLE.addCharacteristic(txCharacteristic);
    BLE.addCharacteristic(rxCharacteristic);
    data.appendServiceUUID(serviceUuid);
    BLE.advertise(&data);

    Serial.printf("Argon BLE Address: %s\n",BLE.address().toString().c_str());

    UARTFlag    = false;
    ptrTxBuf    = &txBuf[0];
}



void loop() {
  txBuf[0] = 0;
  txCharacteristic.setValue(txBuf, 16);
  delay(2000);
  txBuf[0] = 1;
  txCharacteristic.setValue(txBuf, 16);
  delay(2000);



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


// void intToAcsii(uint8_t *array, int elementStart, int intValue) {
//     array[elementStart+2] = (intValue % 10) + 0x30;  //  Populate ones portion of value
//     intValue = (intValue - (intValue%10)) / 10;      //  Decimal right shift
//     array[elementStart+1] = (intValue % 10) + 0x30;  //  populate tens portion of value
//     intValue = (intValue - (intValue%10) ) /10;      //  Decimal right shift
//     array[elementStart+0] = (intValue % 10) + 0x30;  //  Populate hundreds portion of value
//     return;
// }
 
    // intToAcsii(ptrTxBuf, 0, ringPos);
    // txBuf[3] = 0x2C;
    // intToAcsii(ptrTxBuf, 4, pixelRed);
    
    // txBuf[7] = 0x2C;
    // intToAcsii(ptrTxBuf, 8, pixelGreen);
    // txBuf[11] = 0x2C;
    // intToAcsii(ptrTxBuf, 12, pixelBlue);
    // txBuf[15] = 0x0A;
    // txCharacteristic.setValue(txBuf, 16);

