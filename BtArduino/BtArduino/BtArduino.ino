/*   
HC05 - Bluetooth AT-Command mode  
modified on 10 Feb 2019 
by Saeed Hosseini 
https://electropeak.com/learn/ 
*/ 
#include <SoftwareSerial.h> 
SoftwareSerial MyBlue(5, 4); // RX | TX 
int flag = 0; 
int LED = 8; 
bool ATFlag = true;

void setup() {
     
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  Serial.begin(9600); 
  if(ATFlag) {
    MyBlue.begin(38400);  //Baud Rate for AT-command Mode.  
  } else {
    MyBlue.begin(9600); 
  }
  while(!Serial){}
  while(!MyBlue){}
  delay(5000);
  digitalWrite(LED, LOW);
  if(ATFlag) {
    Serial.println("Ready for AT Commands\r\n"); 
  } else {
    Serial.println("Ready to connect\r\nDefault password is 1234 or 0000\r\n"); 
  }
} 
void loop() { 
  
 if(ATFlag) {
   //from bluetooth to Terminal. 
   if (MyBlue.available()){ 
     Serial.write(MyBlue.read()); 
   }
   //from termial to bluetooth 
   if (Serial.available()) {
     MyBlue.write(Serial.read());
   }
 } else {
  // From Bluetooth to phone  
   if (MyBlue.available()) 
     flag = MyBlue.read(); 
   if (flag == 1) { 
     digitalWrite(LED, HIGH); 
     Serial.println("LED On"); 
   } 
   else if (flag == 0) { 
     digitalWrite(LED, LOW); 
     Serial.println("LED Off"); 
   } 
 }


}  
