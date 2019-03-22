//display lib
#include "U8glib.h"
const int delayT = 1000;
int lineHeight = 0; 
int movingLine = 0;
int pumpDuration = 100;  //number of 1/10 seconds to pump 
int soilMoisture = 0; //updated every loop iteration 
int tankPressure = 0; //current tank pressure, updated every loop iteration 
bool pumping = false;
int timePumped = 0;
int fullTankPressure; 

U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NONE);

void setup() {
   oled.setFont(u8g_font_helvB10);  //using the oled display: p. 192
   lineHeight = oled.getFontAscent() - oled.getFontDescent(); 
   //soil moisture sensor: Initialize serial communication at 9600 bits/second
   Serial.begin(9600);
   
   pinMode(A1, INPUT);
   fullTankPressure = analogRead(A1); //read what the water pressure was from beginning 
   
   pinMode(A2, OUTPUT); //set A2 to output for the pump
}

/*
 * writeString 
 * called by refreshScreen, writes debug printouts o the LCD screen 
*/
void writeString() {

  String str = String(soilMoisture);

  oled.drawStr(0, 12, "moisture: ");
  oled.drawStr(70, 12, str.c_str());

  if(pumping) {  
    oled.drawStr(0, 25,"pumping");
  } else {
    oled.drawStr(0, 25,"not pumping");
  }
  
  oled.drawStr(0, 40, "pressure: ");
  oled.drawStr(70, 40, String(tankPressure).c_str());
  oled.drawLine(0, 60, 0 + movingLine*4, 60);

}

//refreshes the LCD screen. Called every loop iteration 
void refreshScreen() {
   //refresh picture
  oled.firstPage();

  do {
    writeString();
    printf("%d", movingLine);
    movingLine++;
  } while (oled.nextPage());

  if(movingLine < 30) {
    movingLine++;
  } else {
    movingLine = 0;
  }
  //oled.clear();
}
//called every loop iteration 
void pumpHandler() {
  //analogWrite(A2, 255);
  soilMoisture = analogRead(A0); 
  if(!pumping && soilMoisture > 670) {
    pumping = true;
    //start the pump 
    analogWrite(A2, 255);
  } 
  if(pumping) {
    timePumped++;
    if(timePumped >= pumpDuration) {
      //stop the pump
      analogWrite(A2, 0);
      pumping = false; 
      timePumped = 0; 
    }
  }
}
/* tankHandler
 * Reads the water tank pressure
 * Alerts user when water level becomes too low 
 * called every loop iteration  
 */
void tankHandler() {
  tankPressure = analogRead(A1);
  if(tankPressure < fullTankPressure/10) {  //less than 10% of water left in tank 
    //alert user to add more water 
    
  }
}

void loop() {
 
 refreshScreen(); 
 pumpHandler();
 tankHandler();
 delay(100);  
}
