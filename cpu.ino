int16_t reg[15];
int16_t pc = 0;
byte carry = 0;
byte zero = 0;
byte negative = 0;
byte redraw = 0;
int8_t color = 1;
int8_t bgcolor = 0;
String s_buffer;

void cpuInit(){
  for(byte i = 1; i < 16; i++){
    reg[i] = 0;
  }
  display_init();
  reg[0] = RAM_SIZE - 1;//stack pointer
  clearScr();
  color = 1;
  bgcolor = 0;
  setCharX(0);
  setCharY(0);
  pc = 0;
  tft.setTextColor(palette[color]);
}
/*
void debug(){
  for(byte i = 0; i < 16; i++){
    Serial.print(" R");
    Serial.print(i);
    Serial.print(':');
    Serial.print(reg[i]);
  }
  Serial.print(" OP:");
  Serial.print(readMem(pc),HEX);
  Serial.print(" PC:");
  Serial.println(pc);
  delay(10);
}*/

inline void writeInt(uint16_t adr, int16_t n){
  writeMem(adr + 1, (n & 0xff00) >> 8);
  writeMem(adr, n & 0xff);
}

inline int16_t readInt(uint16_t adr){
    return (readMem(adr + 1) << 8) + readMem(adr);
}

inline void writeMem(uint16_t adr, int16_t n){
  if(adr < RAM_SIZE)
    mem[adr] = n;
}

inline byte readMem(uint16_t adr){
  return (adr < RAM_SIZE) ? mem[adr] : 0;
}

void setRedraw(){
  redraw = 1;
}

inline int16_t setFlags(int32_t n){
  carry = (n > 0xffff) ? 1 : 0;
  zero = (n == 0) ? 1 : 0;
  negative = (n < 0) ? 1 : 0;
  return (int16_t)n;
}

inline int16_t setFlagsC(int16_t n){
  carry = (n > 0xff) ? 1 : 0;
  zero = (n == 0) ? 1 : 0;
  negative = (n < 0) ? 1 : 0;
  return (uint16_t)n;
}

void cpuRun(uint16_t n){
  for(uint16_t i=0; i < n; i++)
    cpuStep();
}

void cpuStep(){
  byte op1 = readMem(pc++);
  byte op2 = readMem(pc++);
  byte reg1 = 0;
  byte reg2 = 0;
  byte reg3 = 0;
  uint16_t adr;
  uint16_t j;
  uint32_t n = 0;
  //if(isDebug)
  //  debug();
  switch(op1 >> 4){
    case 0x0:
      switch(op1){ 
        case 0x01: 
          //LDI R,int   01 0R XXXX
          reg1 = (op2 & 0xf);
          reg[reg1] = readInt(pc);
          setFlags(reg[reg1]);
          pc += 2;
          break;
        case 0x02: 
          //LDI R,(R)   02 RR
          reg1 = ((op2 & 0xf0) >> 4);
          reg2 = (op2 & 0xf);
          reg[reg1] = readInt(reg[reg2]);
          setFlags(reg[reg1]);
          break;
        case 0x03: 
          //LDI R,(adr) 03 0R XXXX
          reg1 = (op2 & 0xf);
          reg[reg1] = readInt(readInt(pc));
          setFlags(reg[reg1]);
          pc += 2;
          break;
        case 0x04: 
          //LDI R,(int+R) 04 RR XXXX
          reg1 = ((op2 & 0xf0) >> 4);
          reg2 = (op2 & 0xf);
          reg[reg1] = readInt(reg[reg2] + readInt(pc));
          setFlags(reg[reg1]);
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
        default:
          pc++;
      }
      break;
    case 0x1:
      // LDC R,char 1R XX
      reg1 = (op1 & 0xf);
      reg[reg1] = op2;
      setFlagsC(reg[reg1]);
      break;
    case 0x2:
      if(op1 == 0x20){
        // LDC R,(R)  20 RR
        reg1 = ((op2 & 0xf0) >> 4);
        reg2 = (op2 & 0xf);
        reg[reg1] = readMem(reg[reg2]);
        setFlagsC(reg[reg1]);
      }
      else{
        // LDC R,(R+R)  2R RR
        reg1 = (op1 & 0xf);
        reg2 = ((op2 & 0xf0) >> 4);
        reg3 = (op2 & 0xf);
        reg[reg1] = readMem(reg[reg2] + reg[reg3]);
        setFlagsC(reg[reg1]);
      }
      break;
    case 0x3: 
      switch(op1){
        case 0x30:
          // LDC R,(int+R)30 RR XXXX
          reg1 = ((op2 & 0xf0) >> 4);
          reg2 = (op2 & 0xf);
          reg[reg1] = readMem(reg[reg2] + readInt(pc));
          setFlagsC(reg[reg1]);
          pc += 2;
          break;
        case 0x31:
          // LDC R,(adr)  31 0R XXXX
          reg1 = (op2 & 0xf);
          reg[reg1] = readMem(readInt(pc));
          setFlagsC(reg[reg1]);
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
          pc -= 2;
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
          reg[reg1] = timers[reg[reg1] & 0x7];
          setFlags(reg[reg1]);
          break;
      }
      break;
    case 0x6:
      // LDI R,(R+R)  6R RR
      reg1 = (op1 & 0xf);
      reg2 = ((op2 & 0xf0) >> 4);
      reg3 = (op2 & 0xf);
      reg[reg1] = readInt(reg[reg2] + reg[reg3]);
      setFlags(reg[reg1]);
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
          if(zero == 0)
            pc = readInt(pc);
          else 
            pc += 2;
          break;
        case 0x92:
          // JZ adr   92 00 XXXX
          if(zero != 0)
            pc = readInt(pc);
          else 
            pc += 2;
          break;
        case 0x93:
          // JNP adr    93 00 XXXX
          if(negative == 1)
            pc = readInt(pc);
          else 
            pc += 2;
          break;
        case 0x94:
          // JP adr   94 00 XXXX
          if(negative != 1)
            pc = readInt(pc);
          else 
            pc += 2;
          break;
        case 0x95:
          // JNC adr    95 00 XXXX
          if(carry != 1)
            pc = readInt(pc);
          else 
            pc += 2;
          break;
        case 0x96:
          // JC adr   96 00 XXXX
          if(carry == 1)
            pc = readInt(pc);
          else 
            pc += 2;
          break;
        case 0x97:
          // JZR R,adr  97 0R XXXX
          reg1 = op2 & 0xf;
          if(reg[reg1] == 0)
            pc = readInt(pc);
          else 
            pc += 2;
          break;
        case 0x98:
          // JNZR R,adr 98 0R XXXX
          reg1 = op2 & 0xf;
          if(reg[reg1] != 0)
            pc = readInt(pc);
          else 
            pc += 2;
          break;
        case 0x99:
          // CALL adr   99 00 XXXX
          reg[0] -= 2;
          if(reg[0] < 0)
            reg[0] += 0xffff;
          writeInt(reg[0], pc + 2);
          pc = readInt(pc);
          break;
        case 0x9A:
          // RET      9A 00
          pc = readInt(reg[0]);
          reg[0] += 2;
          break;
      }
      break;
    case 0xA:
      switch(op1){
        case 0xA0:
          // ADD R,R    A0 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          n = reg[reg1] + reg[reg2];
          n = setFlags(n);
          reg[reg1] = n;
          break;
        case 0xA1:
          // ADC R,R    A1 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          n = reg[reg1] + reg[reg2] + carry;
          n = setFlags(n);
          reg[reg1] = n;
          break;
        case 0xA2:
          // SUB R,R    A2 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          n = reg[reg1] - reg[reg2];
          n = setFlags(n);
          reg[reg1] = n;
          break;
        case 0xA3:
          // SBC R,R    A3 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          n = reg[reg1] - reg[reg2] - carry;
          n = setFlags(n);
          reg[reg1] = n;
          break;
        case 0xA4:
          // MUL R,R    A4 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          n = reg[reg1] * reg[reg2];
          n = setFlags(n);
          reg[reg1] = n;
          break;
        case 0xA5:
          // DIV R,R    A5 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          if(reg[reg2] != 0)
            n = reg[reg1] / reg[reg2];
          else
            n = 0;//error
          n = setFlags(n);
          reg[reg2] = reg[reg1] % reg[reg2];
          reg[reg1] = n;
          break;
        case 0xA6:
          // AND R,R    A6 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          n = reg[reg1] & reg[reg2];
          n = setFlags(n);
          reg[reg1] = n;
          break;
        case 0xA7:
          // OR R,R   A7 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          n = reg[reg1] | reg[reg2];
          n = setFlags(n);
          reg[reg1] = n;
          break;
        case 0xA8:
          if(op2 == 0x10){
            // INC adr    A8 10 XXXX
            reg1 = op2 & 0xf;
            n = readInt(readInt(pc)) + 1;
            n = setFlags(n);
            writeInt(readInt(pc), n);
            pc += 2;
          }
          else if(op2 > 0x10){
            // INC R,n    A8 nR
            reg1 = op2 & 0xf;
            n = reg[reg1] + (op2 >> 4);
            n = setFlags(n);
            reg[reg1] = n;
          }
          else{
            // INC R    A8 0R       
            reg1 = op2 & 0xf;
            n = reg[reg1] + 1;
            n = setFlags(n);
            reg[reg1] = n;
          }
          break;
        case 0xA9:
          if(op2 == 0x10){
            // DEC adr    A9 10 XXXX
            reg1 = op2 & 0xf;
            n = readInt(readInt(pc)) - 1;
            n = setFlags(n);
            writeInt(readInt(pc), n);
            pc += 2;
          }
          else if(op2 > 0x10){
            // DEC R,n    A9 nR
            reg1 = op2 & 0xf;
            n = reg[reg1] - (op2 >> 4);
            n = setFlags(n);
            reg[reg1] = n;
          }
          else{
            // DEC R    A9 0R
            reg1 = op2 & 0xf;
            n = reg[reg1] - 1;
            n = setFlags(n);
            reg[reg1] = n;
          }
          break;
        case 0xAA:
          // XOR R,R    AA RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          n = reg[reg1] ^ reg[reg2];
          n = setFlags(n);
          reg[reg1] = n;
          break;
        case 0xAB:
          // SHL R,R    AB RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          n = reg[reg1] << reg[reg2];
          n = setFlags(n);
          reg[reg1] = n;
          break;
        case 0xAC:
          // SHR R,R    AC RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          n = reg[reg1] >> reg[reg2];
          n = setFlags(n);
          reg[reg1] = n;
          break;
        case 0xAD:
          // RAND R,R   AD 0R
          reg1 = op2 & 0xf;
          n = random(0, reg[reg1] + 1);
          n = setFlags(n);
          reg[reg1] = n;
          break;
      }
      break;
    case 0xB:
      //CMP R,CHR   BR XX
      reg1 = (op1 & 0x0f);
      n = reg[reg1] - op2;
      n = setFlags(n);
      break;
    case 0xC:
      switch(op1){
        case 0xC0:
          //CMP R,INT   C0 R0 XXXX
          reg1 = (op2 & 0xf0) >> 4;
          n = reg[reg1] - readInt(pc);
          n = setFlags(n);
          pc += 2;
          break;
        case 0xC1:
          //CMP R,R   C1 RR
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          n = reg[reg1] - reg[reg2];
          n = setFlags(n);
          break;
        case 0xC2:
          //LDF R,F   C2 RF
          reg1 = (op2 & 0xf0) >> 4;
          reg2 = op2 & 0xf;
          if(reg2 == 0)
            reg[reg1] = carry;
          else if(reg2 == 1)
            reg[reg1] = zero;
          else if(reg2 == 2)
            reg[reg1] = negative;
          else if(reg2 == 3){ //pozitive
            if(negative == 0 && zero == 0)
              reg[reg1] = 1;
            else
              reg[reg1] = 0;
          }
          else if(reg2 == 4){ //not pozitive
            if(negative == 0 && zero == 0)
              reg[reg1] = 0;
            else
              reg[reg1] = 1;
          }
          else if(reg2 == 5)
            reg[reg1] = 1 - zero;
          else if(reg2 == 6){
              reg[reg1] = redraw;
              redraw = 0;
            }
          else
            reg[reg1] = 0;
          break;
      }
      break;
    case 0xD:
      switch(op1){ 
        case 0xD0:
          //CLS   D000
          clearScr();
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
                s_buffer = String(reg[reg1]);
              else
                s_buffer = String(reg[reg1] - 0x10000);
              for(j = 0; j < s_buffer.length(); j++){
                printc(s_buffer[j], color, bgcolor);
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
          }
          break;
        case 0xD2: 
          switch(op2 & 0xf0){
            case 0x00:
              // GETK R     D20R
              reg1 = (op2 & 0xf);
              if(Serial.available())
                reg[reg1] = Serial.read();
              else
                pc -= 2;
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
                setImageSize(reg[reg1] & 31);
                break;
              case 0x60:
                // DLINE      D46R
                reg1 = op2 & 0xf;
                adr = reg[reg1];//регистр указывает на участок памяти, в котором расположены последовательно y1, x1, y, x
                drwLine(readInt(adr + 6), readInt(adr + 4), readInt(adr + 2), readInt(adr));
                break;
              case 0x070:
                // // DRWRLE R   D47R
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
            }
            break;
        case 0xD5:
          // LDSPRT R,R   D5RR
          reg1 = (op2 & 0xf0) >> 4;//номер спрайта
          reg2 = op2 & 0xf;//адрес спрайта
          setSpr(reg[reg1] & 0x1f, reg[reg2]);
          break;
        case 0xD6:
          // SPALET R,R   D6 RR
          reg1 = (op2 & 0xf0) >> 4;//номер спрайта
          reg2 = op2 & 0xf;//width
          changePalette(reg[reg1] & 15, reg[reg2]);
          break;
        case 0xD7:
            reg1 = op2 & 0xf;
            adr = reg[reg1];
            // SPART R     D7 0R
            if((op2 & 0xf0) == 0x0)
              //регистр указывает на участок памяти, в котором расположены последовательно count, time, gravity
              setParticle(readInt(adr + 4), readInt(adr + 2), readInt(adr));
            else if((op2 & 0xf0) == 0x10)
              //регистр указывает на участок памяти, в котором расположены последовательно speed, direction2, direction1, time
              setEmitter(readInt(adr + 6), readInt(adr + 4), readInt(adr + 2), readInt(adr));
            else if((op2 & 0xf0) == 0x20)
              //регистр указывает на участок памяти, в котором расположены последовательно color, y, x
              drawParticle(readInt(adr + 4), readInt(adr + 2), readInt(adr) & 0xf);
            break;
        case 0xD8:
          // SCROLL R,R   D8RR
          reg1 = (op2 & 0xf0) >> 4;//шаг
          reg2 = op2 & 0xf;//направление
          scrollScreen(reg[reg1], reg[reg2]);
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
        case 0xDB:
          // SPRSPX R,R   DB RR
          reg1 = (op2 & 0xf0) >> 4;//num
          reg2 = op2 & 0xf;//speed y

          break;
        case 0xDC:
          // SPRGTX R,X   DC RX
          reg1 = (op2 & 0xf0) >> 4;//num
          reg2 = op2 & 0xf;//value
          reg[reg1] = getSpriteValue(reg[reg1] & 31, reg[reg2]);
          break;
      }
      break;
    case 0xE:
      // DRSPRT R,R,R ERRR
      reg1 = (op1 & 0xf);//номер спрайта
      reg2 = (op2 & 0xf0) >> 4;//x
      reg3 = op2 & 0xf;//y
      setSprPosition(reg[reg1] & 0x1f, reg[reg2], reg[reg3]);
      if(getSpriteValue(reg[reg1] & 0x1f, 7) < 1)
        setSpriteValue(reg[reg1] & 0x1f, 7, 1);
      break;
    case 0xF:
      // SSPRTV R,R,R FR RR
      reg1 = (op1 & 0xf);//номер спрайта
      reg2 = (op2 & 0xf0) >> 4;//type
      reg3 = op2 & 0xf;//value
      setSpriteValue(reg[reg1] & 0x1f, reg[reg2], reg[reg3]); 
      break;
  }
}
