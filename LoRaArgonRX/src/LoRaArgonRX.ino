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
 *
 * REYAX RYLR896 Datasheet:
 * https://reyax.com/tw/wp-content/uploads/2019/12/RYLR896_EN.pdf
 *
 * AT Command Manual:
 * https://reyax.com/wp-content/uploads/2020/01/Lora-AT-Command-RYLR40x_RYLR89x_EN.pdf
 */

SYSTEM_MODE(SEMI_AUTOMATIC); //keep device from connecting to internet automatically

//settings for RYLR896 module
String password = "FABC1232EEDCAA90FABC0002EEDCAA92"; //(optional) module will only read data from modules with matching password. Using reyaxReset() removes the password.
String address = "0"; //must match address set on transmit module - range from 0 to 16
String network = "0"; //must match network set on transmit module - range from 0 to 65535

char value;

void setup(){
    Serial.begin(9600);
    Serial1.begin(115200);
    RGB.control(true); //allows control of onboard RGB LED
    //reyaxReset(); use to reset module to factory settings
    reyaxSetup(); //sets module preferences
}

void loop(){
    if (Serial1.available()){
        value = LoRaRXByte();
        controlRGB(value);   
    }

}

byte LoRaRXByte() {
        byte byteValue;
        unsigned char inBuf[20];

        (Serial1.readString()).getBytes(inBuf, 18);
        byteValue = inBuf[9];
        return byteValue;
}

void reyaxReset(){
    Serial1.printf("AT+FACTORY\r\n"); //set all parameters to factory defaults
    delay(50);
    Serial1.printf("AT+RESET\r\n"); //resets all parameters including CPIN password--to run this command, comment out all other AT commands in void setup.
}

void reyaxSetup(){ //delays allow time to confirm AT command
    Serial1.printf("AT+NETWORKID="+network+"\r\n"); //set network id from 0 to 16
    delay(50);
    // Serial1.printf("AT+CPIN="+password+"\r\n"); //set password (comment out if not using password)
    delay(50);
    Serial1.printf("AT+ADDRESS="+address+"\r\n"); //set device address from 0 to 65535
    delay(50);
    Serial1.printf("AT\r\n"); //check AT status
    delay(50);
    Serial1.printf("AT+PARAMETER?\r\n"); //check the RF parameters
    delay(50);
    Serial1.printf("AT+BAND?\r\n"); //check module band
    delay(50);
    Serial1.printf("AT+ADDRESS?\r\n"); //check module address
    delay(50);
    Serial1.printf("AT+NETWORKID?\r\n"); //check module network id
    delay(50);
    Serial1.printf("AT+CPIN?\r\n"); //check the AES128 password
    delay(50);
    Serial1.printf("AT+VER?\r\n"); //check the firmware version
    delay(50);
    Serial1.printf("AT+UID?\r\n"); //check the unique ID of the module
    delay(50);
}

void controlRGB(byte flashColor){ //checks case and sets onboard RGB LED to set color
    switch (flashColor) {
        case 0:
            RGB.color(255,0,0);
        break;
        case 0x33:
            RGB.color(0,255,0);
        break;
        case 0x66:
            RGB.color(0,0,255);
        break;
        case 0x99:
            RGB.color(90,153,255);
        break;
        case 0xCC:
            RGB.color(0,255,255);
        break;
        case 0xFF:
            RGB.color(204,0,204);
        break;
        default:
            RGB.color(255,255,255);
        break;
      }
}