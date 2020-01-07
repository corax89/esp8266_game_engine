/*
ESPboy keyboard module class
for www.ESPboy.com project by RomanS
thanks to Plague for help with coding
*/

#include "ESPboy_keyboard.h"

constexpr wchar_t keybCurrent[maxKeyboardLayouts][maxKeyboardRows][maxKeyboardCols + 1] PROGMEM = {
   {L"qeruo", L"wsghl",  L"|dtyi", L"ap~><", L" zcnm", L"^xvb&", L"`~fjk"},
   {L"QERUO", L"WSGHL",  L"|DTYI", L"AP~><", L" ZCNM", L"^XVB$", L"`~FJK"},
   {L"#23_+", L"14/:\"", L"|5()-", L"*@~><", L" 79,.", L"^8?!%", L"0~6;'"}
};


keyboardModule::keyboardModule(uint8_t clickState, uint8_t backlitState, uint32_t bckltOffdelay){
  currentLayout = keybNorm;
  clickFlag = clickState;
  backlitFlag = backlitState;
  backlitOffDelay = bckltOffdelay;
  autoBacklitOffFlag = 0;
  backlitOnTimer = millis();
}


uint8_t keyboardModule::begin(){
  Wire.begin();    
  Wire.beginTransmission(0x27); //check for MCP23017 Keyboard module at address 0x27
  if (!Wire.endTransmission()) 
  {
    initFlag = 1;
    mcpKeyboard.begin(7);
    for (uint8_t i = 0; i < 7; i++){
      mcpKeyboard.pinMode(i, OUTPUT);
      mcpKeyboard.digitalWrite(i, HIGH);}
    for (uint8_t i = 0; i < 5; i++){
      mcpKeyboard.pinMode(i+8, INPUT);
      mcpKeyboard.pullUp(i+8, HIGH);} 
    mcpKeyboard.pinMode(7, OUTPUT);
    mcpKeyboard.digitalWrite(7, HIGH);
    setBacklitState(backlitFlag); 
  }
  else initFlag = 0; 
  return (initFlag);
}


void keyboardModule::scanKeyboard(){
  static uint8_t keysReaded[7];
  static uint8_t row, col;
   for (row = 0; row < 7; row++){
     mcpKeyboard.digitalWrite(row, LOW);
     keysReaded [row] = ((mcpKeyboard.readGPIOAB()>>8) & 31);
     mcpKeyboard.digitalWrite(row, HIGH);
   }
   for (row = 0; row < 7; row++)
     for (col = 0; col < 5; col++)
       if (!((keysReaded[row] >> col) & 1)){
         rowKey = row;
         colKey = col;
         pressedKey = pgm_read_word_near(&keybCurrent[currentLayout][rowKey][colKey]);
       }
}


wchar_t keyboardModule::getPressedKey (){
   pressedKey = 0;
   scanKeyboard();
   if (backlitOffDelay && !autoBacklitOffFlag && millis() > backlitOnTimer + backlitOffDelay){
     autoBacklitOffFlag = 1;
     mcpKeyboard.digitalWrite(7, LOW);
   }
   if (pressedKey){
     backlitOnTimer = millis();
     autoBacklitOffFlag = 0;
     setBacklitState(backlitFlag);
    }
   switch (pressedKey){
    case '|':
      currentLayout++;
      if (currentLayout > maxKeyboardLayouts-1) currentLayout = 0;
      while (keysUnpressed());
      break;
    case '~':
      while (pressedKey == '~') scanKeyboard();
      if (currentLayout == keybNorm )
        pressedKey = pgm_read_word_near(&keybCurrent[keybShift][rowKey][colKey]);
      if (currentLayout == keybShift)
        pressedKey = pgm_read_word_near(&keybCurrent[keybNorm][rowKey][colKey]); 
      break;
    case '^':
      while (pressedKey == '^') scanKeyboard();
      pressedKey = pgm_read_word_near(&keybCurrent[keybAlt][rowKey][colKey]);
      break;
    case '`':
      setBacklitState (!backlitFlag);
      while (keysUnpressed());
      break;
    case '&':
      if (clickFlag == 1) clickFlag = 0;
      else clickFlag = 1;
      while (keysUnpressed());     
      break;
   }
   if (pressedKey && clickFlag) tone(SOUNDPIN, 200, 10);
   if (pressedKey == '|' || pressedKey == '~' || pressedKey == '^' || pressedKey == '&' || pressedKey == '`') 
     pressedKey = 0;
   return pressedKey;
}


uint8_t keyboardModule::keysUnpressed(){
  static uint8_t keysPressed;
  static uint8_t row;
    keysPressed = 0;
    for (row = 0; row < 7; row++){
       mcpKeyboard.digitalWrite(row, LOW);
       if ((mcpKeyboard.readGPIOAB() & 7936) != 7936) keysPressed++;
       mcpKeyboard.digitalWrite(row, HIGH);
    }
    yield();
    return (keysPressed);
}


void keyboardModule::setBacklitState(uint8_t backlitState){
  backlitFlag = backlitState;
  if (backlitFlag) mcpKeyboard.digitalWrite(7, HIGH);
  else mcpKeyboard.digitalWrite(7, LOW);
}


uint8_t keyboardModule::getLastPressedKeyRow(){
  return (rowKey);
}


uint8_t keyboardModule::getLastPressedKeyCol(){
  return (colKey);
}


wchar_t keyboardModule::getLastPressedKey(){
  return (pressedKey);
}


uint8_t keyboardModule::getCurrentLayout(){
  return(currentLayout);
}

uint8_t keyboardModule::getClickState(){
  return(clickFlag);
}

void keyboardModule::setClickState(uint8_t clickState){
  clickFlag = clickState;
}

uint8_t keyboardModule::getBacklitState(){
  return(backlitFlag);
}