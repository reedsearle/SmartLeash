/*
 * Project       LoadCellTest
 * Description:  Test Load Cells and 7seg
 * Author:       Reed Searle
 * Date:         14 april 2022
 */

SYSTEM_MODE(SEMI_AUTOMATIC);

#include <HX711.h>
#include "TM1637.h"

int i;



//****************************
// Constants
//****************************
  const int DIOPIN        = D7;  //  7 Segment Display Data Pin
  const int CLKPIN        = D6;  //  7 Segment Display Clock Pin
  const int LC1DATAPIN    = A4;     //  Load cell 1 data is on Pin  D3
  const int LC1SCLKPIN    = A3;     //  Load cell 1 data is on Pin  D4
  const int LC2DATAPIN    = D13;     //  Load cell 2 data is on Pin  D3
  const int LC2SCLKPIN    = D12;     //  Load cell 2 data is on Pin  D4
  const int SCALE_FACTOR1 = -1000 * (-110.65/500.0); // calibration factor for scale
  const int SCALE_FACTOR2 = -1000; // calibration factor for scale
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

  float weight1;    //  calibrated weight from load cell
  float weight1Max;    //  calibrated maximum weight from load cell
  float weight1Avg;    //  calibrated average weight from load cell
  float rawData1;
  float calibration1;

  float weight2;    //  calibrated weight from load cell
  float weight2Max;    //  calibrated maximum weight from load cell
  float weight2Avg;    //  calibrated average weight from load cell
  float rawData2;
  float calibration2;

//****************************
// Constructors
//****************************
  HX711 scaleOne(LC1DATAPIN, LC1SCLKPIN);
//  HX711 scaleTwo(LC2DATAPIN, LC2SCLKPIN);
  TM1637 LEDSegOne(CLKPIN, DIOPIN);


//****************************
//****************************
//      SETUP
//****************************
//****************************  
void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected, 5000);

//  Initialize 7-Segment Display
  LEDSegOne.clearDisplay();
  LEDSegOne.set(7);
  LEDSegOne.displayNum(8888);
  delay(2000);
  LEDSegOne.displayNum(0);
  // for(i = 0; i <=100; i++) {
  //   LEDSegOne.displayNum(i);
  //   delay(250);
  // }


//  Initialize the Load Cell 1
  scaleOne.set_scale(); // Initialize sscale
  delay(5000); //  let scale settle
  scaleOne.tare();  //  Initialize the tare weight
  scaleOne.set_scale(SCALE_FACTOR1);
  weight1Max = 0;  //  Initialize the max weight finder
  weight1Avg = 0;  //  Initialize the average weight counter

//  Initialize the Load Cell 2
  // scaleTwo.set_scale(); // Initialize sscale
  // delay(5000); //  let scale settle
  // scaleTwo.tare();  //  Initialize the tare weight
  // scaleTwo.set_scale(SCALE_FACTOR2);
  // weight2Max = 0;  //  Initialize the max weight finder
  // weight2Avg = 0;  //  Initialize the average weight counter

  //  Get current time
  Time.zone(-6);                  //  Set time zone to MDT -6 from UTC
  Particle.syncTime();
  dateTime = Time.timeStr();                          //  get current value of date and time
  timeOnlyOld = dateTime.substring(11,19);            //  Initialize timeOnlyOld

  endTime = millis();
}




//****************************
//****************************
//      LOOP
//****************************
//****************************
void loop() {
  //  Get current time
  // dateTime = Time.timeStr();                          //  get current value of date and time
  // timeOnly = dateTime.substring(11,19);               //  Extract value of time from dateTime
  weight1 = scaleOne.get_units(SAMPLE_NUM);
  Serial.printf("weight: %0.1f\n", weight1);
  if(weight1 < 1.0) {
    LEDSegOne.displayNum(0);
  } else {
    LEDSegOne.displayNum((int) weight1);
  }

  // if(weight > weightMax) {
  //   weightMax = weight;
  // }
}

