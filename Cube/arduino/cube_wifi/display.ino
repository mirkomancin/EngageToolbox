/*
DISPLAY
*/
#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment mR = Adafruit_7segment();
Adafruit_7segment mG = Adafruit_7segment();
Adafruit_7segment mB = Adafruit_7segment();

void setupDisplay(){
  mR.begin(0x70);
  mG.begin(0x71);
  mB.begin(0x72);

  mR.blinkRate(0);
  mR.setBrightness(3);
  
  mG.blinkRate(0);
  mG.setBrightness(3);
  
  mB.blinkRate(0);
  mB.setBrightness(3);
  
}

void setDisplayR(uint16_t value){
  mR.println(value);
  mR.writeDisplay();
}

void setDisplayError(){
  mR.printError();
  mR.writeDisplay();
}

void setDisplayG(uint16_t value){
  mG.println(value);
  mG.writeDisplay();
}

void setDisplayGHour(uint16_t value){
  mG.print(value);
  if(value<1000)
    mG.writeDigitNum(0,0);
  mG.writeDisplay();
}

void setDisplayGColon(bool set){
  mG.drawColon(set);
  mG.writeDisplay();
}

void setDisplayB(uint16_t value){
  mB.println(value);
  mB.writeDisplay();
}

void turnOffAllDisplay(){
  mR.println();  
  mR.clear();  
  mR.writeDisplay();
  mG.println();  
  mG.clear();
  mG.writeDisplay();
  mB.println();  
  mB.clear();
  mB.writeDisplay();
}
