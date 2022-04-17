#include <Arduino.h>
/*
 * Project: RYLR896_Receive
 * Description: Receive data via LoRa using a Particle Gen 3 microcontroller and REYAX RYLR896
 * Author: Christian Chavez
 * Date: 1646685874 UNIX
 *
 * REYAX RYLR896 Datasheet:
 * https://reyax.com/tw/wp-content/uploads/2019/12/RYLR896_EN.pdf
 *
 * AT Command Manual:
 * https://reyax.com/wp-content/uploads/2020/01/Lora-AT-Command-RYLR40x_RYLR89x_EN.pdf
 */
#include "Adafruit_NeoPixel.h"

//settings for RYLR896 module
String password = "FABC1232EEDCAA90FABC0002EEDCAA92"; //(optional) module will only read data from modules with matching password. Using reyaxReset() removes the password.
String address = "0"; //must match address set on transmit module - range from 0 to 16
String network = "0"; //must match network set on transmit module - range from 0 to 65535

int intValue;
String incomingString;
unsigned char inBuf[20];
int i;

Adafruit_NeoPixel pixel(1, 10, NEO_GRB + NEO_KHZ800);

void reyaxSetup();
void reyaxReset();
void controlRGB();

void setup(){
    pixel.begin();
    pixel.show();

    Serial.begin(9600);
    Serial1.begin(115200);
    delay(5000);
    Serial.printf("Serial done\r\n");
//    RGB.control(true); //allows control of onboard RGB LED
    //reyaxReset(); use to reset module to factory settings
    reyaxSetup(); //sets module preferences
}

void loop(){
    if (Serial1.available()){
        incomingString = Serial1.readString();
        incomingString.getBytes(inBuf, 13);
        Serial.println(incomingString);
        for(i=0; i<13; i++){
            Serial.printf("0x%02X  ", inBuf[i]);
        }
            Serial.println(" ");
        String receivedValue;
        String subLength = incomingString.substring(7,8);
        receivedValue = incomingString.substring(9,(9+subLength.toInt())); //checks length of incoming value to determine range for .subString() command
        Serial.printf("subLength: %i receivedValue: %i\n",subLength.toInt(), receivedValue.toInt());
        intValue = receivedValue.toInt();
        controlRGB();   
    }

}

void reyaxReset(){
    Serial1.printf("AT+FACTORY\r\n"); //set all parameters to factory defaults
    delay(50);
    Serial1.printf("AT+RESET\r\n"); //resets all parameters including CPIN password--to run this command, comment out all other AT commands in void setup.
}

void reyaxSetup(){ //delays allow time to confirm AT command
    String cmdNet = "AT+NETWORKID="+network+"\r\n";
    Serial1.println(cmdNet);
    Serial.println(cmdNet);
//    Serial1.printf("AT+NETWORKID="+network+"\r\n"); //set network id from 0 to 16
    delay(50);
//    String cmdCpin = "AT+CPIN="+password+"\r\n";
//    Serial1.println(cmdCpin);
//    Serial.println(cmdCpin);
//    Serial1.printf("AT+CPIN="+password+"\r\n"); //set password (comment out if not using password)
    delay(50);
    String cmdAddr = "AT+ADDRESS="+address+"\r\n";
    Serial1.println(cmdAddr);
    Serial.println(cmdAddr);
//    Serial1.printf("AT+ADDRESS="+address+"\r\n"); //set device address from 0 to 65535
    delay(50);
    Serial1.printf("AT\r\n"); //check AT status
    Serial.printf("AT\r\n"); //check AT status
    delay(50);
    Serial1.printf("AT+PARAMETER?\r\n"); //check the RF parameters
    Serial.printf("AT+PARAMETER?\r\n"); //check the RF parameters
    delay(50);
    Serial1.printf("AT+BAND?\r\n"); //check module band
    Serial.printf("AT+BAND?\r\n"); //check module band
    delay(50);
    Serial1.printf("AT+ADDRESS?\r\n"); //check module address
    Serial.printf("AT+ADDRESS?\r\n"); //check module address
    delay(50);
    Serial1.printf("AT+NETWORKID?\r\n"); //check module network id
    Serial.printf("AT+NETWORKID?\r\n"); //check module network id
    delay(50);
    Serial1.printf("AT+CPIN?\r\n"); //check the AES128 password
    Serial.printf("AT+CPIN?\r\n"); //check the AES128 password
    delay(50);
    Serial1.printf("AT+VER?\r\n"); //check the firmware version
    Serial.printf("AT+VER?\r\n"); //check the firmware version
    delay(50);
    Serial1.printf("AT+UID?\r\n"); //check the unique ID of the module
    Serial.printf("AT+UID?\r\n"); //check the unique ID of the module
    delay(50);
}

void controlRGB(){ //checks case and sets onboard RGB LED to set color
    switch (intValue) {
        case 0:
            pixel.setPixelColor(0,255,0,0);
            pixel.show();
        break;
        case 51:
            pixel.setPixelColor(0,0,255,0);
            pixel.show();
        break;
        case 102:
            pixel.setPixelColor(0,0,0,255);
            pixel.show();
        break;
        case 153:
            pixel.setPixelColor(0,90,153,255);
            pixel.show();
        break;
        case 204:
            pixel.setPixelColor(0,0,255,255);
            pixel.show();
        break;
        case 255:
            pixel.setPixelColor(0,204,0,204);
            pixel.show();
        break;
        default:
            pixel.setPixelColor(0,255,255,255);
            pixel.show();
        break;
      }
}