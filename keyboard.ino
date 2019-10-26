#include <Wire.h>
#include "settings.h"

void geti2cAdress(){
  byte error,address;
  i2c_adress=0;
  for(address = 1; address < 127; address++ ){
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0){
      i2c_adress=address;
      return;
    }
    yield();
  }
}

void scani2c(){
  byte error, address;
  int nDevices;
  nDevices = 0;
  for(address = 1; address < 127; address++ ){
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0){
      Serial.print(F("I2C device found at address 0x"));
      if (address<16)
        Serial.print(F("0"));
      Serial.print(address,HEX);
      Serial.println(F("  !"));
      nDevices++;
    }
    else if (error==4){
      Serial.print(F("Unknown error at address 0x"));
      if (address<16)
        Serial.print(F("0"));
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println(F("No I2C devices found\n"));
}

#ifdef ESPBOY
void getKey(){
  thiskey = 0;
  if(!mcp.digitalRead(0))
     thiskey |= 4; 
  if(!mcp.digitalRead(1))
     thiskey |= 1;
  if(!mcp.digitalRead(2))
     thiskey |= 2;
  if(!mcp.digitalRead(3))
     thiskey |= 8;
  if(!mcp.digitalRead(4))
     thiskey |= 16;
  if(!mcp.digitalRead(5))
     thiskey |= 32;
  if(!mcp.digitalRead(6))
     thiskey |= 64;
  if(!mcp.digitalRead(7))
     thiskey |= 128;
}
#else
void getKey(){
  byte dio_in;
  Wire.beginTransmission(i2c_adress);
  Wire.write(B11111111); //Конфигурация всех портов PCF8574P на клавиатуре как входа
  Wire.endTransmission();
  Wire.requestFrom(i2c_adress,1);
  dio_in = Wire.read();  //читаем состояние портов PCF8574P(кнопок)
  thiskey = 0;
  if((dio_in & 128) == 0)
    thiskey |= 4;
  if((dio_in & 64) == 0)
    thiskey |= 8;
  if((dio_in & 32) == 0)
    thiskey |= 2;
  if((dio_in & 16) == 0)
    thiskey |= 1; //up
  if((dio_in & 8) == 0)
    thiskey |= 128;
  if((dio_in & 4) == 0)
    thiskey |= 32;
  if((dio_in & 2) == 0)
    thiskey |= 16; 
  if((dio_in & 1) == 0)
    thiskey |= 64; 
}
#endif
