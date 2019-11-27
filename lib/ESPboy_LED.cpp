/*
ESPboy LED class
for www.ESPboy.com project by RomanS
*/

#include "ESPboy_LED.h"


void ESPboyLED::begin(){
  pinMode(LEDPIN, OUTPUT);
  LEDflagOnOff = 1;
  LEDr = 0; 
  LEDg = 0; 
  LEDb = 0;
  ledset(LEDr, LEDg, LEDb);
}


void ESPboyLED::off(){
  LEDflagOnOff = 0;
  ledset(0, 0, 0);
}


void ESPboyLED::on(){
  LEDflagOnOff = 1;
  ledset(LEDr, LEDg, LEDb);
}


uint8_t ESPboyLED::getState(){
  return (LEDflagOnOff);
}

void ESPboyLED::setRGB (uint8_t red, uint8_t green, uint8_t blue){
  LEDr = red;
  LEDg = green;
  LEDb = blue;
  if (LEDflagOnOff) ledset(LEDr, LEDg, LEDb);
}


void ESPboyLED::setR (uint8_t red){
  LEDr = red;
  if (LEDflagOnOff) ledset(LEDr, LEDg, LEDb);
}


void ESPboyLED::setG (uint8_t green){
  LEDg = green;
  if (LEDflagOnOff) ledset(LEDr, LEDg, LEDb);
}


void ESPboyLED::setB (uint8_t blue){
  LEDb = blue;
  if (LEDflagOnOff) ledset(LEDr, LEDg, LEDb);
}


uint32_t ESPboyLED::getRGB(){
  return (((uint32_t)LEDb<<16) + ((uint32_t)LEDg<<8) + ((uint32_t)LEDr) );
}


uint8_t ESPboyLED::getR(){
  return (LEDr);
}


uint8_t ESPboyLED::getG(){
  return (LEDg);
}


uint8_t ESPboyLED::getB(){
  return (LEDb);
}


void ICACHE_RAM_ATTR ESPboyLED::ledset(uint8_t rled, uint8_t gled, uint8_t bled) {
 static uint_fast32_t i, t, c, startTime, pixel, mask;
 static uint8_t cpuFreq;
 static const uint32_t pinMask = 1<<LEDPIN;
 static uint32_t t0h, t1h, ttot;
  
 cpuFreq = ESP.getCpuFreqMHz()/80;
 t0h  = 32*cpuFreq;  // 0.4us
 t1h  = 64*cpuFreq;  // 0.8us
 ttot = 100*cpuFreq; // 1.25us
  
  pixel = (gled<<16) + (rled<<8) + bled;
  mask = 0x800000; 
  startTime = 0;
  GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, pinMask);
  delay(1);
  os_intr_lock();
  for (i=0; i<24; i++){
    if (pixel & mask) t = t1h;
    else t = t0h;
    while (((c=ESP.getCycleCount()) - startTime) < ttot);    // Wait for the previous bit to finish
    GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, pinMask);      // digitalWrite HIGH 
    startTime = c;   
    while (((c=ESP.getCycleCount()) - startTime) < t);       // Wait for high time to finish
    GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, pinMask);      // digitalWrite LOW
    mask>>=1;
  }
  os_intr_unlock();
  delay(1);
  GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, pinMask);
}