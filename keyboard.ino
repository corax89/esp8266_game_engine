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

uint8_t virtualKeyboard(char buf[], uint8_t len){
  String s;
  char keys[] =      "0123456789%+-*/=qwertyuiop[]{}()asdfghjkl:;\"/#$ zxcvbnm<>?.,!@";
  char keysShift[] = "0123456789%+-*/=QWERTYUIOP[]{}()ASDFGHJKL:;\"/#^ ZXCVBNM<>?&|!@";
  int16_t x = 4, y = 4, px = 0, py = 0;
  uint8_t isShift = 0;
  TFT_eSprite img = TFT_eSprite(&tft);
  img.setColorDepth(1);
  img.createSprite(64, 48);
  img.setTextSize(2);
  tft.fillRect(64, 64, 64, 64, 0x0000);
  tft.drawRect(64, 64, 65, 65, 0xFC00);  
  getKey();
  while(1){
     while(x != px * 16 || y !=  py * 16){
        if(x < px * 16)
          x += 4;
        else if(x > px * 16)
          x -= 4;
        if(y < py * 16)
          y += 4;
        else if(y > py * 16)
          y -= 4;
        img.fillRect(0, 0, 64, 48, 0x0000);
        for(uint8_t i = 0; i < 4; i++){
          for(uint8_t j = 0; j < 16; j++){
            if(i == 3 && j == 14){
              img.setTextSize(1);
              img.drawChar('S', 32 + j * 16 - x - 4, 16 + i * 16 - y);
              img.drawChar('h', 32 + j * 16 - x + 2, 16 + i * 16 - y);
              img.drawChar('i', 32 + j * 16 - x - 4, 16 + i * 16 - y + 8);
              img.drawChar('f', 32 + j * 16 - x + 0, 16 + i * 16 - y + 8);
              img.drawChar('t', 32 + j * 16 - x + 6, 16 + i * 16 - y + 8);
              img.setTextSize(2);
            }
            else if(i == 3 && j == 15){
              img.setTextSize(1);
              img.drawChar('E', 32 + j * 16 - x - 4, 16 + i * 16 - y);
              img.drawChar('n', 32 + j * 16 - x + 2, 16 + i * 16 - y);
              img.drawChar('t', 32 + j * 16 - x - 4, 16 + i * 16 - y + 8);
              img.drawChar('e', 32 + j * 16 - x + 2, 16 + i * 16 - y + 8);
              img.drawChar('r', 32 + j * 16 - x + 8, 16 + i * 16 - y + 8);
              img.setTextSize(2);
            }
            else{
              if(isShift)
                img.drawChar(keysShift[i * 16 + j], 32 + j * 16 - x, 16 + i * 16 - y);
              else
                img.drawChar(keys[i * 16 + j], 32 + j * 16 - x, 16 + i * 16 - y);
            }
          }
        }
        img.pushSprite(66, 81);
        tft.drawRect(93, 96, 18, 17, 0xFC00);
        delay(10);
    }
    delay(200);
    getKey();
    if(thiskey & 2){//down
      if(py < 3)
        py++;
      else
        py = 0;
    }
    else if(thiskey & 1){//up
      if(py > 0)
        py--;
      else
        py = 3;
    }
    else if(thiskey & 8){//left
      if(px < 15)
        px++;
      else
        px = 0;
    }
    else if(thiskey & 4){//right
      if(px > 0)
        px--;
      else
        px = 15;
    }
    else if(thiskey & 16){//ok
      if(py == 3 && px == 14){
        isShift = 1 - isShift;
        x += 4;
      }
      else if(py == 3 && px == 15){
        img.deleteSprite();
        for(int8_t j = 0; j < min(s.length(), len); j++)
          buf[j] = s[j]; 
        return min(s.length(), len);
      }
      else{
        if(isShift)
          s += keysShift[px + py * 16];
        else
          s += keys[px + py * 16];
        tft.fillRect(65, 65, 63, 15, 0x0000);
        tft.setCursor(67,68);
        if(s.length() < 10)
          tft.print(s);
        else
          tft.print(s.substring(s.length() - 10));
      }
    }
    else if(thiskey & 32){//del
      s = s.substring(0, s.length() - 1);
      tft.fillRect(65, 65, 63, 15, 0x0000);
      tft.setCursor(67,68);
      if(s.length() < 10)
        tft.print(s);
      else
        tft.print(s.substring(s.length() - 10));
      delay(200);
    }
  }
  return 0;
}
