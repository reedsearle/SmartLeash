/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/reed_/Documents/IoT/SmartLeash/WinniePullTest/src/WinniePullTest.ino"
/*
 * Project WinniePullTest
 * Description:
 * Author:
 * Date:
 */

void setup();
void loop();
void write_SIII_ToOLED(String item1, int item2, int item3, int item4);
#line 8 "c:/Users/reed_/Documents/IoT/SmartLeash/WinniePullTest/src/WinniePullTest.ino"
SYSTEM_MODE(SEMI_AUTOMATIC);

#include "Adafruit_SSD1306.h"
#include "Adafruit_GFX.h"
#include <HX711.h>


//****************************
// Constants
//****************************
  const int SCREENWIDTH  = 128;    //  Width of screen in pixels
  const int SCREENHEIGHT = 64;     //  Height of screen in pixels
  const int OLEDRESET    = -1;     //  OLED Reset shared with Teensy
  byte      OLEDADDRESS  = 0x3C;   //  Address of the OLED on the IIC bus
  const int SAMPLETIME   = 60000;  //  Sample rate = 1 min
  const int MOISTPIN     = A1;     //  Moisture sensor is on Pin  A0
  const int BUTTONPIN    = D5;     //  Button  is on Pin  D5
  const int LCDATAPIN    = D3;     //  Load cell data is on Pin  D3
  const int LCSCLKPIN    = D4;     //  Load cell data is on Pin  D4
  const int SD_CS_PIN    = SS;     //  uSD chip select on the SS pin
  const int SCALE_FACTOR = -1000 * (21.5 / 0.5); // calibration factor for scale
  const int SAMPLE_NUM = 10;       //  Number of averaged points per data read

  
//****************************
// Variables
//****************************
  //  Time & timing Variables
  String     dateTime, timeOnly, timeOnlyOld;   
  int endTime;

  //  Load Cell Variables
  float weight;    //  calibrated weight from load cell
  float weightMax;    //  calibrated maximum weight from load cell
  float weightAvg;    //  calibrated average weight from load cell
  float rawData;
  float calibration;

//****************************
// Constructors
//****************************
  Adafruit_SSD1306 displayOne(OLEDRESET);
  HX711 scaleOne(LCDATAPIN, LCSCLKPIN);


//****************************
//****************************
//      SETUP
//****************************
//****************************  
void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected, 5000);

//  Initialize the Load Cell
  scaleOne.set_scale(); // Initialize sscale
  delay(5000); //  let scale settle
  scaleOne.tare();  //  Initialize the tare weight
  scaleOne.set_scale(SCALE_FACTOR);
  weightMax = 0;  //  Initialize the max weight finder
  weightAvg = 0;  //  Initialize the average weight counter

//  Initialize the uSD Card
  // initialize uSD card module CS to off
  pinMode     (SD_CS_PIN,OUTPUT); 
  digitalWrite(SD_CS_PIN,HIGH);

  //  Get current time
  Time.zone(-6);                  //  Set time zone to MDT -6 from UTC
  Particle.syncTime();
  dateTime = Time.timeStr();                          //  get current value of date and time
  timeOnlyOld = dateTime.substring(11,19);            //  Initialize timeOnlyOld

  //  Setup OLED
  displayOne.begin(SSD1306_SWITCHCAPVCC, OLEDADDRESS);  // initialize with the I2C addr above
  displayOne.setTextSize(1);
  displayOne.setTextColor(WHITE);
  displayOne.setRotation(2);  //  Rotate OLED 180 for easier viewing
  write_SIII_ToOLED(timeOnlyOld.c_str(), 111, 222, 333);

  endTime = millis();
}




//****************************
//****************************
//      LOOP
//****************************
//****************************
void loop() {
  //  Get current time
  dateTime = Time.timeStr();                          //  get current value of date and time
  timeOnly = dateTime.substring(11,19);               //  Extract value of time from dateTime

  weight = scaleOne.get_units(SAMPLE_NUM);
  if(weight > weightMax) {
    weightMax = weight;
  }
  if (timeOnly.compareTo(timeOnlyOld)) {
  Serial.printf("Time is: %s, Weight: %0.2f, WeightMax: %0.2f \n", timeOnly.c_str(), weight, weightMax);

  //  Write values to OLED coninuously
  write_SIII_ToOLED(timeOnly.c_str(), weight, weightMax, 0);
  timeOnlyOld = timeOnly;            //  reset timeOnlyOld
  }
}







//****************************
//****************************
//      write_SIII_ToOLED
//****************************
//****************************
void write_SIII_ToOLED(String item1, int item2, int item3, int item4) {
    // Write to the OLED display
    displayOne.clearDisplay();//  Clear the display before going further
    displayOne.drawRect(0,0,SCREENWIDTH,SCREENHEIGHT,WHITE);
    displayOne.setCursor(7,5);
    displayOne.printf("Time is: %s\n", item1.c_str());
    displayOne.setCursor(7,15);
    displayOne.printf("Current load:: %i\n", item2);
    displayOne.setCursor(7,35);
    displayOne.printf("Max Load: %i\n", item3);
    displayOne.setCursor(7,45);
    displayOne.printf("Ave Load: %i\n", item4);
    displayOne.display(); // Force display
}
