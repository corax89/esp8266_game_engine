#include <EEPROM.h>
#include "settings.h"

#define FIFO_MAX_SIZE 32

int16_t reg[16] __attribute__ ((aligned));
int16_t shadow_reg[16] __attribute__ ((aligned));
uint16_t pc = 0;
uint16_t interrupt = 0;
uint16_t dataName = 0;
uint32_t accum, saccum;
uint8_t carry = 0;
uint8_t zero = 0;
uint8_t negative = 0;
uint8_t redraw = 0;
int8_t color = 1;
int8_t bgcolor = 0;
int8_t keyPosition;
char s_buffer[7];
String loadedFileName;
char strBuf[16];
uint8_t strBufLength = 0;
uint8_t strBufPosition = 0;

struct Fifo_t {
  uint16_t el[FIFO_MAX_SIZE];
  uint8_t position_read;
  uint8_t position_write;
  uint8_t size;
};

struct Fifo_t interruptFifo;

#pragma GCC optimize ("-O2")
#pragma GCC push_options

inline void fifoClear(){
  interruptFifo.position_read = 0;
  interruptFifo.position_write = 0;
  interruptFifo.size = 0;
  for(int16_t i = 0; i < FIFO_MAX_SIZE; i++)
    interruptFifo.el[i] = 0;
}

inline void pushInFifo(int16_t n){
  if(interruptFifo.size < FIFO_MAX_SIZE){
    interruptFifo.el[interruptFifo.position_write] = n;
    interruptFifo.position_write++;
    if(interruptFifo.position_write >= FIFO_MAX_SIZE)
      interruptFifo.position_write = 0;
    interruptFifo.size++;
  }
}

inline uint16_t popOutFifo(){
  uint16_t out = 0;
  if(interruptFifo.size > 0){
    interruptFifo.size--;
    out = interruptFifo.el[interruptFifo.position_read];
    interruptFifo.position_read++;
    if(interruptFifo.position_read >= FIFO_MAX_SIZE)
      interruptFifo.position_read = 0;
  }
  return out;
}

inline int16_t flagsTouint8_t(){
  return (carry & 0x1) + ((zero & 0x1) << 1)  + ((negative & 0x1) << 2);
}
  
inline void uint8_tToFlags(int16_t b){
  carry = b & 0x1;
  zero = (b & 0x2) >> 1;
  negative = (b & 0x4) >> 2;
}

inline void setinterrupt(uint16_t adr, int16_t param){
  if(interrupt == 0 && adr != 0){
    saccum = accum;
    shadow_reg[0] = flagsTouint8_t();
    for(int8_t j = 1; j <= 15; j++){
      shadow_reg[j] = reg[j];
    }
    reg[0] -= 2;
    writeInt(reg[0], param);
    reg[0] -= 2;
    writeInt(reg[0], pc);
    interrupt = pc;
    pc = adr;
  }
  else{
    pushInFifo(adr);
    pushInFifo(param);
  }
}

void setLoadedFileName(String s){
  loadedFileName = s;
}

void cpuInit(){
  for(uint8_t i = 1; i < 16; i++){
    reg[i] = 0;
  }
  strBufPosition = 0;
  interrupt = 0;
  fifoClear();
  display_init();
  reg[0] = RAM_SIZE - 1;//stack pointer
  clearScr(0);
  color = 1;
  bgcolor = 0;
  setCharX(0);
  setCharY(0);
  pc = 0;
  carry = 0;
  zero = 0;
  negative = 0;
  accum = 0;
  saccum = 0;
  dataName = 0;
  setRtttlPlay(0);
}

void debug(){
  for(uint8_t i = 0; i < 16; i++){
    Serial.print(" R");
    Serial.print(i);
    Serial.print(':');
    Serial.print(reg[i]);
  }
  Serial.print(F(" OP:"));
  Serial.print(readMem(pc),HEX);
  Serial.print(F(" PC:"));
  Serial.println(pc);
  Serial.print(F("carry: "));
  Serial.print(carry);
  Serial.print(F(" zero: "));
  Serial.print(zero);
  Serial.print(F(" negative: "));
  Serial.print(negative);
  Serial.print(F(" interrupt: "));
  Serial.print(interrupt);
  Serial.print('/');
  Serial.println(interruptFifo.size);
  spriteDebug();
}

inline void setRedraw(){
  redraw = 1;
}

inline void setFlags(int32_t n){
  carry = (n > 0xffff) ? 1 : 0;
  zero = (n == 0) ? 1 : 0;
  negative = (n < 0) ? 1 : 0;
}

int16_t isqrt(int16_t n) {
  int g = 0x8000;
  int c = 0x8000;
  for (;;) {
    if (g*g > n) {
      g ^= c;
    }
    c >>= 1;
    if (c == 0) {
      return g;
    }
    g |= c;
  }
}

inline int16_t distancepp(int16_t x1, int16_t y1, int16_t x2, int16_t y2){
  return isqrt((x2 - x1)*(x2 - x1) + (y2-y1)*(y2-y1));
}

inline void setDataName(uint16_t address){
  dataName = address;
}

boolean testDataName(uint16_t pos){
  uint8_t c;
  if(dataName){
    for(uint16_t i = 0; i < 12; i++){
      c = EEPROM.read(pos + i);
      if(c != readMem(dataName + i))
        return false;
      if(c == 0)
        return true;
    }
  }
  else{
    for(uint16_t i = 0; i < 12; i++){
      c = EEPROM.read(pos + i);
      if(c != loadedFileName[i])
        return false;
      if(c == 0)
        return true;
    }
  }
  return true;
}

uint16_t findData(){
  uint16_t pos;
  uint8_t c;
  pos = 0;
  while(pos < EEPROM_SIZE){
    c = EEPROM.read(pos);
    if(c == 0 || c == 0xff)
      return EEPROM_SIZE;
    if(testDataName(pos + 1))
      return pos;
    pos += c;
  }
  return EEPROM_SIZE;
}

uint16_t findEndData(){
  uint16_t pos;
  uint8_t c;
  pos = 0;
  while(pos < EEPROM_SIZE){
    c = EEPROM.read(pos);
    if(c == 0 || c == 0xff)
      return pos;
    pos += c;
  }
  return EEPROM_SIZE;
}

uint16_t saveData(uint16_t arrayAddress, uint16_t count){
  uint16_t i,pos;
  uint8_t c;
  if(count > 242)
    count = 242;
  pos = findData();
  if(pos == EEPROM_SIZE)
    pos = findEndData();
  if((EEPROM_SIZE - pos) - 12 < count)
    return (EEPROM_SIZE - pos) - 12;
  c = EEPROM.read(pos);
  if(c == 0 || c == 0xff){
    EEPROM.write(pos, 12 + count);
    for(i = 0; i < 12; i++)
      EEPROM.write(pos + i + 1, readMem(dataName + i));
  }
  else if(c < count)
    return c;
  pos += 12;
  for(i = 0; i < count; i++)
    EEPROM.write(pos + i, readMem(arrayAddress + i));
  EEPROM.commit();
  return count;
}

uint16_t loadData(uint16_t arrayAddress){
  uint16_t i,pos;
  uint8_t c;
  pos = findData();
  if(pos == EEPROM_SIZE)
    return 0;
  c = EEPROM.read(pos) - 12;
  pos += 12;
  for(i = 0; i < c; i++)
    writeMem(arrayAddress + i, EEPROM.read(pos + i));
  return c;
}

int16_t fixed_sin(int x) {
  //Bhaskara I's sine approximation sin(x°) = 4·x·(180−x)/(40500−x·(180−x))
  char pos = 1;  // positive - keeps an eye on the sign.
  if (x < 0){
    x = -x;
    pos = !pos;
  }
  if (x >= 360)
    x %= 360;
  if (x > 180){
    x -= 180;
    pos = !pos;
  }
  int16_t nv = x * (180 - x);
  int32_t s = (nv * 4  * (1 << MULTIPLY_FP_RESOLUTION_BITS))/(40500 - nv);
  if (pos) 
    return (int16_t)s;
  return (int16_t)-s;
}

inline int16_t fixed_cos(int16_t g){
  return fixed_sin(g+90);
}

inline void copyMem(uint16_t to_adr, uint16_t from_adr, uint16_t num_bytes) {
  for (uint16_t i = 0; i < num_bytes; i++) {
    writeMem(to_adr++, readMem(from_adr++));
  }
}

#ifdef ESPBOY
void setLedColor(uint16_t r5g6b5){
  uint8_t r,g,b;
  r = ((((r5g6b5 >> 11) & 0x1F) * 527) + 23) >> 6;
  g = ((((r5g6b5 >> 5) & 0x3F) * 259) + 33) >> 6;
  b = (((r5g6b5 & 0x1F) * 527) + 23) >> 6;
  myled.setRGB(r, g, b);
}
#endif

inline void cpuRun(uint16_t n){
  for(uint16_t i=0; i < n; i++){
    cpuStep();
  }
}

void cpuStep(){
  uint8_t op1 = readMem(pc++);
  uint8_t op2 = readMem(pc++);
  uint8_t reg1 = 0;
  uint8_t reg2 = 0;
  uint8_t reg3 = 0;
  uint16_t adr;
  uint16_t j;
  switch(op1 >> 4){
    case 0x0:
      switch(op1){ 
        case 0x01: 
          //LDI R,int   01 0R XXXX
          reg1 = (op2 & 0xf);
          accum = readInt(pc);
          reg[reg1] = (int16_t)accum;
          pc += 2;
          break;
        case 0x02: 
          //LDI R,(R)   02 RR
          reg1 = ((op2 & 0xf0) >> 4);
          reg2 = (op2 & 0xf);
          accum = readInt(reg[reg2]);
          reg[reg1] = (int16_t)accum;
          break;
        case 0x03: 
          //LDI R,(adr) 03 0R XXXX
          reg1 = (op2 & 0xf);
          accum = readInt(readInt(pc));
          reg[reg1] = (int16_t)accum;
          pc += 2;
          break;
        case 0x04: 
          //LDI R,(int+R) 04 RR XXXX
          reg1 = ((op2 & 0xf0) >> 4);
          reg2 = (op2 & 0xf);
          accum = readInt(reg[reg2] + readInt(pc));
          reg[reg1] = (int16_t)accum;
          pc += 2;
          break;
        case 0x05: 
          //STI (R),R   05 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          writeInt(reg[reg1],reg[reg2]);
          break;
        case 0x06:
          if((op2 & 0x0f) == 0){
            //STI (adr),R 06 R0 XXXX
            reg1 = (op2 & 0xf0) >> 4;
            writeInt(readInt(pc),reg[reg1]);
            pc += 2;
          }
          else{
            //STI (adr+R),R 06 RR XXXX
            reg1 = (op2 & 0xf0) >> 4;
            reg2 = op2 & 0xf;
            writeInt(readInt(pc) + reg[reg1],reg[reg2]);
            pc += 2;
          }
          break;
        case 0x07:
          //MOV R,R   07 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          reg[reg1] = reg[reg2];
          break;
        case 0x08:
          //LDIAL R,(int+R*2) 08 RR XXXX
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          accum = readInt(reg[reg2] * 2 + readInt(pc));
          reg[reg1] = (int16_t)accum;
          pc += 2;
          break;
        case 0x09:
          //STIAL (adr+R*2),R   09 RR XXXX
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          writeInt(readInt(pc) + reg[reg1] * 2,reg[reg2]);
          pc += 2;
          break;
        default:
          pc++;
      }
      break;
    case 0x1:
      // LDC R,char 1R XX
      reg1 = (op1 & 0xf);
      accum = op2;
      reg[reg1] = (int16_t)accum;
      break;
    case 0x2:
      if(op1 == 0x20){
        // LDC R,(R)  20 RR
        reg1 = ((op2 & 0xf0) >> 4);
        reg2 = (op2 & 0xf);
        accum = readMem(reg[reg2]);
        reg[reg1] = (int16_t)accum;
      }
      else{
        // LDC R,(R+R)  2R RR
        reg1 = (op1 & 0xf);
        reg2 = ((op2 & 0xf0) >> 4);
        reg3 = (op2 & 0xf);
        accum = readMem(reg[reg2] + reg[reg3]);
        reg[reg1] = (int16_t)accum;
      }
      break;
    case 0x3: 
      switch(op1){
        case 0x30:
          // LDC R,(int+R)30 RR XXXX
          reg1 = ((op2 & 0xf0) >> 4);
          reg2 = (op2 & 0xf);
          accum = readMem(reg[reg2] + readInt(pc));
          reg[reg1] = (int16_t)accum;
          pc += 2;
          break;
        case 0x31:
          // LDC R,(adr)  31 0R XXXX
          reg1 = (op2 & 0xf);
          accum = readMem(readInt(pc));
          reg[reg1] = (int16_t)accum;
          pc += 2;
          break;
        case 0x32:
          // STC (adr),R  32 0R XXXX
          reg1 = (op2 & 0xf0) >> 4;
          writeMem(readInt(pc),reg[reg1]);
          pc += 2;
          break;
        case 0x33:
          // STC (int+R),R33 RR XXXX
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          writeMem(readInt(pc) + reg[reg1],reg[reg2]);
          pc += 2;
          break;
      }
      break;
    case 0x4:
      if(op1 == 0x40){
          // STC (R),R  40 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          writeMem(reg[reg1], reg[reg2]);
        }
        else{
          // STC (R+R),R  4R RR 
          reg1 = (op1 & 0xf);
          reg2 = ((op2 & 0xf0) >> 4);
          reg3 = (op2 & 0xf);
          writeMem(reg[reg1] + reg[reg2], reg[reg3]);
        }
      break;
    case 0x5:
      switch(op1){ 
        case 0x50:
          //HLT       5000
          clearSpriteScr();
          noTone(SOUNDPIN);
          fileList("/");
          break;
        case 0x51:
          // STIMER R,R   51RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          timers[reg[reg1] & 0x7] = reg[reg2];
          break;
        case 0x52:
          // GTIMER R   520R
          reg1 = op2 & 0xf;
          accum = timers[reg[reg1] & 0x7];
          reg[reg1] = (int16_t)accum;
          break;
        case 0x53:
          // SETLED R   530R
        #ifdef ESPBOY
          reg1 = op2 & 0xf;
          setLedColor(reg[reg1]);
        #endif
          break;
        case 0x54:
            // LOADRT   540R
            reg1 = (op2 & 0xf0) >> 4;
            reg2 = op2 & 0xf;
            setRtttlAddress((uint16_t)reg[reg1]);
            setRtttlLoop(reg[reg2]);
            break;
          case 0x55:
            switch(op2){
              // PLAYRT   5500
              case 0x00:
                setRtttlPlay(1);
                break;
              // PAUSERT    5501
              case 0x01:
                setRtttlPlay(0);
                break;
              // STOPRT   5502
              case 0x02:
                setRtttlPlay(2);
                break;
            }
            break;
          case 0x56:
            // LOADRT   540R
            reg1 = (op2 & 0xf0) >> 4;
            reg2 = op2 & 0xf;
            addTone(reg[reg1], reg[reg2]);
            break;
      case 0x57:
        if (op2 < 0x10){
          // LDATA R      57 0R
          reg2 = op2 & 0xf;
          reg[reg2] = loadData(reg[reg2]);
        }
        else if (op2 < 0x20){
          // NDATA R      57 1R
          reg2 = op2 & 0xf;
          setDataName(reg[reg2]);
        }
        break;
      case 0x58:
        // SDATA R,R      58 RR
        reg1 = (op2 & 0xf0) >> 4;
        reg2 = op2 & 0xf;
        reg[reg1] = saveData(reg[reg1], reg[reg2]);
        break;
      }
      break;
    case 0x6:
      // LDI R,(R+R)  6R RR
      reg1 = (op1 & 0xf);
      reg2 = ((op2 & 0xf0) >> 4);
      reg3 = (op2 & 0xf);
      accum = readInt(reg[reg2] + reg[reg3]);
      reg[reg1] = (int16_t)accum;
      break;
    case 0x7:
      // STI (R+R),R  7R RR
      reg1 = (op1 & 0xf);
      reg2 = ((op2 & 0xf0) >> 4);
      reg3 = (op2 & 0xf);
      writeInt(reg[reg1] + reg[reg2], reg[reg3]);
      break;  
    case 0x8:
      switch(op1){
        case 0x80:
          // POP R    80 0R
          reg1 = (op2 & 0xf);
          reg[reg1] = readInt(reg[0]);
          reg[0] += 2;
          break;
        case 0x81:
          // POPN R   81 0R
          reg1 = (op2 & 0xf);
          for(j = reg1; j >= 1; j--){
            reg[j] = readInt(reg[0]);
            reg[0] += 2;
          }
          break;
        case 0x82:
          // PUSH R   82 0R
          reg1 = (op2 & 0xf);
          reg[0] -= 2;
          writeInt(reg[0], reg[reg1]);
          break;
        case 0x83:
          // PUSHN R    83 0R
          reg1 = (op2 & 0xf);
          for(j = 1; j <= reg1; j++){
            reg[0] -= 2;
            writeInt(reg[0], reg[j]);
          }
          break;
      }
      break;
    case 0x9:
      switch(op1){
        case 0x90:
          // JMP adr    90 00 XXXX
          pc = readInt(pc);
          break;
        case 0x91:
          // JNZ adr    91 00 XXXX
          setFlags(accum);
          if(zero == 0)
            pc = readInt(pc);
          else 
            pc += 2;
          break;
        case 0x92:
          // JZ adr   92 00 XXXX
          setFlags(accum);
          if(zero != 0)
            pc = readInt(pc);
          else 
            pc += 2;
          break;
        case 0x93:
          // JNP adr    93 00 XXXX
          setFlags(accum);
          if(negative == 1)
            pc = readInt(pc);
          else 
            pc += 2;
          break;
        case 0x94:
          // JP adr   94 00 XXXX
          setFlags(accum);
          if(negative != 1)
            pc = readInt(pc);
          else 
            pc += 2;
          break;
        case 0x95:
          // JNC adr    95 00 XXXX
          setFlags(accum);
          if(carry != 1)
            pc = readInt(pc);
          else 
            pc += 2;
          break;
        case 0x96:
          // JC adr   96 00 XXXX
          setFlags(accum);
          if(carry == 1)
            pc = readInt(pc);
          else 
            pc += 2;
          break;
        case 0x97:
          // JZR R,adr  97 0R XXXX
          setFlags(accum);
          reg1 = op2 & 0xf;
          if(reg[reg1] == 0)
            pc = readInt(pc);
          else 
            pc += 2;
          break;
        case 0x98:
          // JNZR R,adr 98 0R XXXX
          setFlags(accum);
          reg1 = op2 & 0xf;
          if(reg[reg1] != 0)
            pc = readInt(pc);
          else 
            pc += 2;
          break;
        case 0x99:
          // CALL adr   99 00 XXXX
          reg[0] -= 2;
          writeInt(reg[0], pc + 2);
          pc = readInt(pc);
          break;
        case 0x9A:
          // RET      9A 00
          if(!interrupt){
            pc = readInt(reg[0]);
            reg[0] += 2;
          }
          else{
            pc = readInt(reg[0]);
            if(pc == interrupt){
              reg[0] += 4;
              accum = saccum;
              for(int8_t j = 15; j >= 1; j--){
                reg[j] = shadow_reg[j];
              }
              uint8_tToFlags(shadow_reg[0]);
              interrupt = 0;
              if(interruptFifo.size > 0)
                setinterrupt(popOutFifo(), popOutFifo());
            }
            else
              reg[0] += 2;
          }
          break;
      }
      break;
    case 0xA:
      switch(op1){
        case 0xA0:
          // ADD R,R    A0 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          accum = reg[reg1] + reg[reg2];
          reg[reg1] = (int16_t)accum;
          break;
        case 0xA1:
          // ADC R,R    A1 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          accum = reg[reg1] + reg[reg2] + carry;
          reg[reg1] = (int16_t)accum;
          break;
        case 0xA2:
          // SUB R,R    A2 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          accum = reg[reg1] - reg[reg2];
          reg[reg1] = (int16_t)accum;
          break;
        case 0xA3:
          // SBC R,R    A3 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          accum = reg[reg1] - reg[reg2] - carry;
          reg[reg1] = (int16_t)accum;
          break;
        case 0xA4:
          // MUL R,R    A4 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          accum = reg[reg1] * reg[reg2];
          reg[reg1] = (int16_t)accum;
          break;
        case 0xA5:
          // DIV R,R    A5 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          if(reg[reg2] != 0){
            accum = reg[reg1] / reg[reg2];
            reg[reg2] = reg[reg1] % reg[reg2];
          }
          else{
            accum = 0;//error
            reg[reg2] = 0;
          }
          reg[reg1] = (int16_t)accum;
          break;
        case 0xA6:
          // AND R,R    A6 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          accum = reg[reg1] & reg[reg2];
          reg[reg1] = (int16_t)accum;
          break;
        case 0xA7:
          // OR R,R   A7 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          accum = reg[reg1] | reg[reg2];
          reg[reg1] = (int16_t)accum;
          break;
        case 0xA8:
          if(op2 == 0x10){
            // INC adr    A8 10 XXXX
            reg1 = op2 & 0xf;
            accum = readInt(readInt(pc)) + 1;
            writeInt(readInt(pc), (int16_t)accum);
            pc += 2;
          }
          else if(op2 > 0x10){
            // INC R,n    A8 nR
            reg1 = op2 & 0xf;
            accum = reg[reg1] + (op2 >> 4);
            reg[reg1] = (int16_t)accum;
          }
          else{
            // INC R    A8 0R       
            reg1 = op2 & 0xf;
            accum = reg[reg1] + 1;
            reg[reg1] = (int16_t)accum;
          }
          break;
        case 0xA9:
          if(op2 == 0x10){
            // DEC adr    A9 10 XXXX
            reg1 = op2 & 0xf;
            accum = readInt(readInt(pc)) - 1;
            writeInt(readInt(pc), (int16_t)accum);
            pc += 2;
          }
          else if(op2 > 0x10){
            // DEC R,n    A9 nR
            reg1 = op2 & 0xf;
            accum = reg[reg1] - (op2 >> 4);
            reg[reg1] = (int16_t)accum;
          }
          else{
            // DEC R    A9 0R
            reg1 = op2 & 0xf;
            accum = reg[reg1] - 1;
            reg[reg1] = (int16_t)accum;
          }
          break;
        case 0xAA:
          // XOR R,R    AA RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          accum = reg[reg1] ^ reg[reg2];
          reg[reg1] = (int16_t)accum;
          break;
        case 0xAB:
          // SHL R,R    AB RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          accum = reg[reg1] << reg[reg2];
          reg[reg1] = (int16_t)accum;
          break;
        case 0xAC:
          // SHR R,R    AC RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          accum = reg[reg1] >> reg[reg2];
          reg[reg1] = (int16_t)accum;
          break;
        case 0xAD:
          reg1 = op2 & 0xf;
          reg2 = op2 & 0xf0;
          switch(reg2){
            // RAND R   AD 0R
            case 0x00:
              accum = random(0, reg[reg1] + 1);
              reg[reg1] = (int16_t)accum;
            break;
            // SQRT R    AD 1R
            case 0x10:
              accum = isqrt(reg[reg1]);
              reg[reg1] = (int16_t)accum;
            break;
            // NOT R    AD 2R
            case 0x20:
              accum = (~reg[reg1]);
              reg[reg1] = (int16_t)accum;
            break;
          }
          break;
        case 0xAE:
          // ANDL R,R   AE RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          accum = (reg[reg1] != 0 && reg[reg2] != 0) ? 1 : 0;
          reg[reg1] = (int16_t)accum;
          break;
        case 0xAF:
          // ORL R,R    AF RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          accum = (reg[reg1] != 0 || reg[reg2] != 0) ? 1 : 0;
          reg[reg1] = (int16_t)accum;
          break;
      }
      break;
    case 0xB:
      //CMP R,CHR   BR XX
      reg1 = (op1 & 0x0f);
      accum = reg[reg1] - op2;
      break;
    case 0xC:
      switch(op1){
        case 0xC0:
          //CMP R,INT   C0 R0 XXXX
          reg1 = (op2 & 0xf0) >> 4;
          accum = reg[reg1] - readInt(pc);
          pc += 2;
          break;
        case 0xC1:
          //CMP R,R   C1 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          accum = reg[reg1] - reg[reg2];
          break;
        case 0xC2:
          //LDF R,F   C2 RF
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          setFlags(accum);
          switch(reg2){
            case 0:
              reg[reg1] = carry;
              break;
            case 1:
              reg[reg1] = zero;
              break;
            case 2:
              reg[reg1] = negative;
              break;
            case 3: //pozitive
              if(negative == 0 && zero == 0)
                reg[reg1] = 1;
              else
                reg[reg1] = 0;
              break;
            case 4: //not pozitive
              if(negative == 0 && zero == 0)
                reg[reg1] = 0;
              else
                reg[reg1] = 1;
              break;
            case 5:
              reg[reg1] = 1 - zero;
              break;
            case 6:
                reg[reg1] = redraw;
                redraw = 0;
              break;
            default:
              reg[reg1] = 0;
          }
          break;
        case 0xc3:
          reg1 = op2 & 0x0f;
          reg2 = op2 & 0xf0;
          switch(reg2){
            // ITOF R   C3 0R
            case 0x00:
              reg[reg1] = reg[reg1] * (1 << MULTIPLY_FP_RESOLUTION_BITS);
              break;
            // FTOI R   C3 1R
            case 0x10:
              reg[reg1] = reg[reg1] / (1 << MULTIPLY_FP_RESOLUTION_BITS);
              break;
            // SIN R   C3 2R
            case 0x20:
              reg[reg1] = fixed_sin(reg[reg1]);
              break;
            // SIN R   C3 3R
            case 0x30:
              reg[reg1] = fixed_cos(reg[reg1]);
              break;
            // MEMCPY R    C3 4R
            case 0x40:
              adr = reg[reg1];
              copyMem(readInt(adr + 4), readInt(adr + 2), readInt(adr));
              break;
          }
          break;
        case 0xC4:
          // MULF R,R   C4 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          accum = (reg[reg1] * reg[reg2]) / (1 << MULTIPLY_FP_RESOLUTION_BITS);
          reg[reg1] = (uint16_t) accum;
          break;
        case 0xC5:
          // DIVF R,R   C5 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          accum = (reg[reg1] * (1 << MULTIPLY_FP_RESOLUTION_BITS)) / reg[reg2];
          reg[reg1] = (uint16_t) accum;
          break;
      }
      break;
    case 0xD:
      switch(op1){ 
        case 0xD0:
          //CLS   D000
          if((op2 & 0xff) == 0)
            clearScr(bgcolor);
          else{
            //GSPRXY R,R
            reg1 = (op2 & 0xf0) >> 4;
            reg2 = op2 & 0xf;
            reg[reg1] = getSpriteInXY(reg[reg1], reg[reg2]);
          }
          break;
        case 0xD1:
          switch(op2 & 0xf0){
            case 0x00:
              //PUTC R  D10R
              reg1 = (op2 & 0xf);
              printc((char)reg[reg1], color, bgcolor);
              break;
            case 0x10:
              //PUTS R  D11R
              reg1 = (op2 & 0xf);
              j = 0;
              while(!(readMem(reg[reg1] + j) == 0 || j > 1000)){
                printc((char)(readMem(reg[reg1] + j)), color, bgcolor);
                j++;
              }
              break;
            case 0x20:
              //PUTN R D12R
              reg1 = (op2 & 0xf);
              if(reg[reg1] < 32768)
                itoa(reg[reg1], s_buffer, 10);
              else
                itoa(reg[reg1] - 0x10000, s_buffer, 10);
              j = 0;
              while(s_buffer[j]){
                printc(s_buffer[j], color, bgcolor);
                j++;
              }
              break;
            case 0x30:
              //SETX R      D13R
              reg1 = (op2 & 0xf);
              setCharX(reg[reg1] & 0xff);
              break;
            case 0x40:
              //SETY R      D14R
              reg1 = (op2 & 0xf);
              setCharY(reg[reg1] & 0xff);
              break;
            case 0x50:
              //DRECT R     D15R
              reg1 = (op2 & 0xf);
              adr = reg[reg1];
              drwRect(readInt(adr + 6), readInt(adr + 4), readInt(adr + 2), readInt(adr));
              break;
            case 0x60:
              //FRECT R     D16R
              reg1 = (op2 & 0xf);
              adr = reg[reg1];
              fllRect(readInt(adr + 6), readInt(adr + 4), readInt(adr + 2), readInt(adr));
              break;
            case 0x70:
              //DCIRC R     D17R
              reg1 = (op2 & 0xf);
              adr = reg[reg1];
              drwCirc(readInt(adr + 4), readInt(adr + 2), readInt(adr));
              break;
            case 0x80:
              //FCIRC R     D18R
              reg1 = (op2 & 0xf);
              adr = reg[reg1];
              fllCirc(readInt(adr + 4), readInt(adr + 2), readInt(adr));
              break;
            case 0x90:
              //DTRIANG R   D19R
              reg1 = (op2 & 0xf);
              adr = reg[reg1];
              drwTriangle(readInt(adr + 10), readInt(adr + 8), readInt(adr + 6), readInt(adr + 4), readInt(adr + 2), readInt(adr));
              break;
            case 0xA0:
              //FTRIANG R   D1AR
              reg1 = (op2 & 0xf);
              adr = reg[reg1];
              fllTriangle(readInt(adr + 10), readInt(adr + 8), readInt(adr + 6), readInt(adr + 4), readInt(adr + 2), readInt(adr));
              break;
            case 0xB0:
              //PUTF R      D1BR
              reg1 = (op2 & 0xf);
              printfix(reg[reg1], color, bgcolor);
              break;
            case 0xC0:
              // DRWCHAR R  D1 CR
              reg1 = (op2 & 0xf);
              adr = reg[reg1]; //регистр указывает на участок памяти, в котором расположены последовательно y, x, char
              drawChar(readInt(adr + 4), readInt(adr + 2), readInt(adr));
              break;
            case 0xD0:
              // DRWSTR R D1 DR
              reg1 = (op2 & 0xf);
              adr = reg[reg1]; //регистр указывает на участок памяти, в котором расположены последовательно y, x, string
              drawString(readInt(adr + 4), readInt(adr + 2), readInt(adr));
              break;
            case 0xE0:
              // FONTLOAD R D1 ER
              reg1 = (op2 & 0xf);
              adr = reg[reg1]; //регистр указывает на участок памяти, в котором расположены последовательно end, start, adr
              fontload(readInt(adr + 4), readInt(adr + 2), readInt(adr));
              break;
            case 0xF0:
              // FONTSIZE R D1 FR
              reg1 = (op2 & 0xf);
              adr = reg[reg1]; //регистр указывает на участок памяти, в котором расположены последовательно fontheight, fontwidth, imgheight, imgwidth
              fontsize(readInt(adr + 6), readInt(adr + 4), readInt(adr + 2), readInt(adr));
              break;
          }
          break;
        case 0xD2: 
          switch(op2 & 0xf0){
            case 0x00:
              // GETK R     D20R
              reg1 = (op2 & 0xf);
              if(strBufLength == 0){
                if(getCharY() > 8)
                  strBufLength = virtualKeyboard(2, 2, strBuf, 16);
                else
                  strBufLength = virtualKeyboard(2, 78, strBuf, 16);
                if(strBufLength == 0){
                  strBuf[0] = '\n';
                  strBufLength = 1;
                }
                setRedrawRect(0, 128);
                strBufPosition = 0;
              }
              if(strBufLength > 0){
                if(strBufPosition < strBufLength){
                  reg[reg1] = strBuf[strBufPosition];
                }
                else{
                  strBufPosition = 0;
                  strBufLength = 0;
                  pc -= 2;
                }
                strBufPosition++;
              }
              else
                pc -= 2;
              thiskey = 0;
              break;
            case 0x10:
              // GETJ R     D21R
              reg1 = (op2 & 0xf);
              reg[reg1] = thiskey;
              break;
          }
          break;
        case 0xD3:
          // PPIX R,R   D3RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          setPix(reg[reg1], reg[reg2], color);
          break;
        case 0xD4:
          switch(op2 & 0xf0){
              case 0x00:
                // DRWIM R      D40R
                reg1 = op2 & 0xf;
                adr = reg[reg1];//регистр указывает на участок памяти, в котором расположены последовательно h, w, y, x, адрес
                drawImg(readInt(adr + 8), readInt(adr + 6), readInt(adr + 4), readInt(adr + 2), readInt(adr));
                break;
              case 0x10:
                // SFCLR R      D41R
                reg1 = op2 & 0xf;
                color = reg[reg1] & 0xf;
                break;
              case 0x20:
                // SBCLR R      D42R
                reg1 = op2 & 0xf;
                bgcolor = reg[reg1] & 0xf;
                break;
              case 0x30:
                // GFCLR R      D43R
                reg1 = op2 & 0xf;
                reg[reg1] = color;
                break;
              case 0x40:
                // GBCLR R      D44R
                reg1 = op2 & 0xf;
                reg[reg1] = bgcolor;
                break;
              case 0x50:
               // ISIZE      D45R
                reg1 = op2 & 0xf;
                setImageSize(reg[reg1]);
                break;
              case 0x60:
                // DLINE      D46R
                reg1 = op2 & 0xf;
                adr = reg[reg1];//регистр указывает на участок памяти, в котором расположены последовательно y1, x1, y, x
                drwLine(readInt(adr + 6), readInt(adr + 4), readInt(adr + 2), readInt(adr));
                break;
              case 0x070:
                // DRWRLE R   D47R
                reg1 = op2 & 0xf;
                adr = reg[reg1];//регистр указывает на участок памяти, в котором расположены последовательно h, w, y, x, адрес
                drawImgRLE(readInt(adr + 8), readInt(adr + 6), readInt(adr + 4), readInt(adr + 2), readInt(adr));
                break;
              case 0x80:
                // LDTILE R   D4 8R
                reg1 = op2 & 0xf;
                adr = reg[reg1];//регистр указывает на участок памяти, в котором расположены последовательно height, width, iheight, iwidth, adr
                loadTile(readInt(adr + 8), readInt(adr + 6), readInt(adr + 4), readInt(adr + 2), readInt(adr));
                break;
              case 0x90:
                // SPRSDS R D4 9R
                reg1 = op2 & 0xf;
                adr = reg[reg1];//регистр указывает на участок памяти, в котором расположены последовательно direction, speed, n
                spriteSetDirectionAndSpeed(readInt(adr + 4), readInt(adr + 2), readInt(adr));
                break;
              case 0xA0:
                // DRW1BIT R D4AR
                reg1 = op2 & 0xf;
                adr = reg[reg1];//регистр указывает на участок памяти, в котором расположены последовательно h, w, y, x, адрес
                drawImageBit(readInt(adr + 8), readInt(adr + 6), readInt(adr + 4), readInt(adr + 2), readInt(adr));
                break;
              case 0xB0:
                // SETCLIP R D4BR
                reg1 = op2 & 0xf;
                adr = reg[reg1];//регистр указывает на участок памяти, в котором расположены последовательно y1, x1, y0, x0
                setClip(readInt(adr + 6), readInt(adr + 4), readInt(adr + 2), readInt(adr));
                break;
              case 0xC0:
                // SETFPS R   D4 CR
                reg1 = op2 & 0xf;
                if(reg[reg1] >= 1 && reg[reg1] <= 40)
                  timeForRedraw = 1000 / reg[reg1];
                break;
              case 0xD0:
                // SETCTILE R D4 DR
                reg1 = op2 & 0xf;
                setTileCollisionMap(reg[reg1]);
                break;
            }
            break;
        case 0xD5:
          // LDSPRT R,R   D5RR
          reg1 = (op2 & 0xf0) >> 4;//номер спрайта
          reg2 = op2 & 0x0f;//адрес спрайта
          setSpr((reg[reg1] < SPRITE_COUNT) ? reg[reg1] : 0, reg[reg2]);
          break;
        case 0xD6:
          // SPALET R,R   D6 RR
          reg1 = (op2 & 0xf0) >> 4;//номер палитры
          reg2 = op2 & 0xf;//цвет
          changePalette(reg[reg1] & 15, reg[reg2]);
          break;
        case 0xD7:
            reg1 = op2 & 0xf;
            adr = reg[reg1];
            switch(op2 & 0xf0){
              case 0x0:
                // SPART R     D7 0R
                //регистр указывает на участок памяти, в котором расположены последовательно count, time, gravity
                setParticle(readInt(adr + 4), readInt(adr + 2), readInt(adr));
                break;
              case 0x10:
                //регистр указывает на участок памяти, в котором расположены последовательно speed, direction2, direction1, time
                setEmitter(readInt(adr + 6), readInt(adr + 4), readInt(adr + 2), readInt(adr));
                break;
              case 0x20:
                //регистр указывает на участок памяти, в котором расположены последовательно color, y, x
                drawParticle(readInt(adr + 4), readInt(adr + 2), readInt(adr) & 0xf);
                break;
              case 0x50:
                //регистр указывает на участок памяти, в котором расположены последовательно y2,x2,y1,x1
                reg[1] = distancepp(readInt(adr + 6), readInt(adr + 4), readInt(adr + 2), readInt(adr));
                break;
              case 0x60:
                //регистр указывает на участок памяти, в котором расположены последовательно size,height,width
                setEmitterSize(readInt(adr + 4), readInt(adr + 2), readInt(adr));
                break;
            }
            break;
        case 0xD8:
          // SCROLL R,R   D8RR
          reg1 = (op2 & 0xf0) >> 4;//шаг
          reg2 = op2 & 0xf;//направление
          scrollScreen(1, reg[reg2]);
          break;
        case 0xD9:
          // GETPIX R,R   D9RR
          reg1 = (op2 & 0xf0) >> 4;//x
          reg2 = op2 & 0xf;//y
          reg[reg1] = getPix(reg[reg1], reg[reg2]);
          break;
        case 0xDA:
          // DRTILE R   DA RR
          reg1 = (op2 & 0xf0) >> 4;//x
          reg2 = op2 & 0xf;//y
          drawTile(reg[reg1], reg[reg2]);
          break;
        case 0xDC:
          // SPRGTX R,X   DC RX
          reg1 = (op2 & 0xf0) >> 4;//num
          reg2 = op2 & 0xf;//value
          reg[reg1] = getSpriteValue((reg[reg1] < SPRITE_COUNT) ? reg[reg1] : 0, reg[reg2]);
          break;
        case 0xDE:
          // AGBSPR R,R     DE RR
          reg1 = (op2 & 0xf0) >> 4;//n1
          reg2 = op2 & 0xf;//n2
          reg[reg1] = angleBetweenSprites(reg[reg1], reg[reg2]);
          break;
        case 0xDF:
          // GTILEXY R,R      DF RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          reg[reg1] = getTileInXY(reg[reg1], reg[reg2], 0);
          break;
      }
      break;
    case 0xE:
      // DRSPRT R,R,R ERRR
      reg1 = (op1 & 0xf);//номер спрайта
      reg2 = (op2 & 0xf0) >> 4;//x
      reg3 = op2 & 0xf;//y
      setSprPosition((reg[reg1] < SPRITE_COUNT) ? reg[reg1] : 0, reg[reg2], reg[reg3]);
      if(getSpriteValue((reg[reg1] < SPRITE_COUNT) ? reg[reg1] : 0, 7) < 1)
        setSpriteValue((reg[reg1] < SPRITE_COUNT) ? reg[reg1] : 0, 7, 1);
      break;
    case 0xF:
      // SSPRTV R,R,R FR RR
      reg1 = (op1 & 0xf);//номер спрайта
      reg2 = (op2 & 0xf0) >> 4;//type
      reg3 = op2 & 0xf;//value
      setSpriteValue((reg[reg1] < SPRITE_COUNT) ? reg[reg1] : 0, reg[reg2], reg[reg3]); 
      break;
  }
}
