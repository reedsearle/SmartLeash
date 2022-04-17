/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/reed_/Documents/IoT/SmartLeash/LoRaArgonTX/src/LoRaArgonTX.ino"
/*
 * Project LoRaArgonTX
 * Description:
 * Author:
 * Date:
 */

/*
 * Project: RYLR896_Transmitter
 * Description: Transmit data via LoRa using a Particle Gen 3 microcontroller and REYAX RYLR896
 * Author: Christian Chavez
 * Date: 1646685010 UNIX
 */

void setup();
void loop();
void LoRaTXByte(int sendValue);
void controlRGB(byte flashColor);
#line 15 "c:/Users/reed_/Documents/IoT/SmartLeash/LoRaArgonTX/src/LoRaArgonTX.ino"
SYSTEM_MODE(SEMI_AUTOMATIC);

const int LEDPIN = D7;
unsigned long lastTransmission;
const int interval = 5000;
char cmd[16];

byte value;


String password = "FABC1232EEDCAA90FABC0002EEDCAA92"; //must match password set on receiving module...best security practice to define password in seprate .h file

void setup(){
    Serial.begin(9600);
    Serial1.begin(115200);
    delay(5000);
    pinMode(LEDPIN,OUTPUT);
    value = 0xFF;
    RGB.control(true);
    delay(50);

 //   Serial1.printf("AT+CPIN="+password+"\r\n"); //correct
    //Serial1.printf("AT+CPIN=FABC4442EEDCAA90AABC0002EEDCBB92\r\n"); //incorrect
    delay(50);
}

void loop(){
    if (millis() > lastTransmission + interval){
        controlRGB(value);
        LoRaTXByte(value);
        Serial.printf("Sending value: 0x%02x\n", value);
        value = (value + 0x33) % 0xFF;
        lastTransmission = millis();
        }
    if(Serial1.available()) {
        Serial.println(Serial1.readString());
    }
}

void LoRaTXByte(int sendValue) {
    sprintf(cmd, "AT+SEND=0,1,%c\r\n", sendValue);
    Serial1.println(cmd);
    Serial.println(cmd);
    return;
}

void controlRGB(byte flashColor) {
            switch (flashColor) {
        case 0:
            RGB.color(255,0,0);
        break;
        case 51:
            RGB.color(0,255,0);
        break;
        case 102:
            RGB.color(0,0,255);
        break;
        case 153:
            RGB.color(90,153,255);
        break;
        case 204:
            RGB.color(0,255,255);
        break;
        case 255:
            RGB.color(204,0,204);
        break;
        default:
            RGB.color(255,255,255);
        break;
        }
}

//void setup(){
//     Serial.begin(9600);
//     Serial1.begin(115200);
//     pinMode(LEDPIN,OUTPUT);
//     value = 255;
//     RGB.control(true);
//     Serial1.printf("AT+CPIN="+password+"\r\n"); //correct
//     //Serial1.printf("AT+CPIN=FABC4442EEDCAA90AABC0002EEDCBB92\r\n"); //incorrect
//     delay(50);
// }

// void loop(){
//     if (millis() > lastTransmission + interval){
//         controlRGB();
//         String sendValue = String(value);
// //        sprintf(cmd, "AT+SEND=0,1,%c\r\n", value);
//         String cmd = "AT+SEND=0,"+String(sendValue.length())+","+ sendValue+"\r";
//     //    String cmd = "AT+SEND=0,"+String(sendValue.length())+","+ String(value)+"\r";
//      //   Serial1.printf("%c", cmd.c_str());
//         Serial1.printf("%s",cmd);
//         Serial.printf("%s",cmd);
//         lastTransmission = millis();
//         }
// }
