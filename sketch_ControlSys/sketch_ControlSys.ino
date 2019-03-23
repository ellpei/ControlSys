//display lib
#include "U8glib.h"
const int delayT = 1000;
int lineHeight = 0; 
int movingLine = 0;

uint8_t tankPort = 1;
int fullTankPressure; 
int tankPressure = 0; //current tank pressure, updated every loop iteration 

uint8_t pumpPort = 2;
int pumpDuration = 100;  //number of 1/10 seconds to pump 
bool pumping = false;
int timePumped = 0;

uint8_t moisturePort = 0; //A0
int soilMoisture = 0; //updated every loop iteration 

long lightDuration = 360000 ;  //the time the lights should be continuously on (in 1/10 seconds) = 10 hours
long timeLightsOn = 0;
bool lightsOn = false;
long darkDuration = 504000;//12 hours
long timeDarkness = 0; 

int temperature = 0; 

U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NONE);

void setup() {
   oled.setFont(u8g_font_helvB10);  //using the oled display: p. 192
   lineHeight = oled.getFontAscent() - oled.getFontDescent(); 
   //soil moisture sensor: Initialize serial communication at 9600 bits/second
   Serial.begin(9600);
   
   pinMode(tankPort, INPUT);
   fullTankPressure = analogRead(tankPort); //read what the water pressure was from beginning 
   
   pinMode(A2, OUTPUT); //set A2 to output for the pump (relay)

   pinMode(A3, OUTPUT);   //set A3 to output for the lights (relay)
   analogWrite(A3, 255);
   lightsOn = true;

   pinMode(A4, INPUT);
   pinMode(2, INPUT);
}

/*
 * writeString 
 * called by refreshScreen, writes debug printouts o the LCD screen 
*/
void writeString() {

  String str = String(soilMoisture);

  oled.drawStr(0, 12, "moisture: ");
  oled.drawStr(70, 12, str.c_str());
  /*
  if(pumping) {  
    oled.drawStr(0, 25,"pumping");
  } else {
    oled.drawStr(0, 25,"not pumping");
  }
  */
  String tempStr = String(temperature);

  oled.drawStr(0, 25,"temperature: ");
  oled.drawStr(90, 25, tempStr.c_str());


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
  soilMoisture = analogRead(moisturePort); 
  if(!pumping && soilMoisture > 670) {
    pumping = true;
    //start the pump 
    analogWrite(A2, 255);
  } 
  if(pumping) {
    timePumped++;
    if(timePumped >= pumpDuration) {
      if(analogRead(moisturePort) < 670) {
        //stop the pump
        analogWrite(A2, 0);
        pumping = false; 
        timePumped = 0;
      } 
       
    }
  }
}
/* tankHandler
 * Reads the water tank pressure
 * Alerts user when water level becomes too low 
 * called every loop iteration  
 */
void tankHandler() {
  tankPressure = analogRead(tankPort);
  if(tankPressure < fullTankPressure/10) {  //less than 10% of water left in tank 
    //alert user to add more water 
    
  }
}
/* lightHandler
 * called every loop iteration  
 */
void lightHandler() {
  if(lightsOn) {
    timeLightsOn++;
    if(timeLightsOn >= lightDuration) {
      //turn off lights
      lightsOn = false;
      analogWrite(A3, 0);
      timeLightsOn = 0;
      return;
    }
  } else {
    timeDarkness++;
    if(timeDarkness >= darkDuration) {
      lightsOn = true; 
      analogWrite(A3, 255);
      timeDarkness = 0;
    }
  }
}

void loop() {
 
 refreshScreen(); 
 pumpHandler();
 tankHandler();
 lightHandler();
 //temperature = analogRead(A4);
 temperature = digitalRead(2);
 delay(100);  
}
