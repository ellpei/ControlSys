//display lib
#include "U8glib.h"
const int delayT = 1000;
int lineHeight = 0; 
int movingLine = 0;
int pumpDuration = 10;  //number of seconds to pump 
int soilMoisture = 0;

U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NONE);

void writeString() {
  String str = String(soilMoisture);

  oled.drawStr(0, 12, "moisture: ");
  oled.drawStr(70, 12, str.c_str());

  if(soilMoisture > 600) {  //over 600 when dry
    oled.drawStr(0, 25,"pumping");
  } else {
    oled.drawStr(0, 25,"not pumping");
  }

  oled.drawLine(0, 60, 0 + movingLine*4, 60);
}


void setup() {
   oled.setFont(u8g_font_helvB10);  //using the oled display: p. 192
   lineHeight = oled.getFontAscent() - oled.getFontDescent(); 
   //soil moisture sensor: Initialize serial communication at 9600 bits/second
   Serial.begin(9600);
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
}

void loop() {
 
 refreshScreen(); 
 writeString();
 soilMoisture = analogRead(A0); 

}
