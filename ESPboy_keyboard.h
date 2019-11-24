/*
ESPboy keyboard module class
for www.ESPboy.com project by RomanS
thanks to Plague for help with coding
     COL0  COL1  COL2  COL3  COL4
ROW0   Q      E     R     U     O
ROW1   W      S     G     H     L
ROW2   |sym   D     T     Y     I
ROW3   A      P     ~sh2  >ent  <bck
ROW4   spc    Z     C     N     M
ROW5   ^alt   X     V     B     $
ROW6   `mik   ~sh1  F     J     K
*/

#include <Arduino.h>
#include "Adafruit_MCP23017.h"

#ifndef ESPboy_Keyboard
#define ESPboy_Keyboard

#define maxKeyboardLayouts 3
#define maxKeyboardRows  7
#define maxKeyboardCols  5

#define SOUNDPIN       D3


class keyboardModule{
private:
  Adafruit_MCP23017 mcpKeyboard;
  enum keybLayouts {keybNorm = 0, keybShift, keybAlt};
  wchar_t pressedKey;
  uint8_t initFlag, clickFlag, backlitFlag, rowKey, colKey, currentLayout, autoBacklitOffFlag; 
  uint64_t backlitOnTimer;
  uint32_t backlitOffDelay;
  void scanKeyboard();
public:
  keyboardModule(uint8_t clickState, uint8_t backlitState, uint32_t bckltOffdelay);
  uint8_t begin();
  uint8_t keysUnpressed();
  uint8_t getLastPressedKeyRow();
  uint8_t getLastPressedKeyCol();
  uint8_t getCurrentLayout();
  uint8_t getClickState();
  void    setClickState(uint8_t clickState);
  uint8_t getBacklitState();
  void    setBacklitState(uint8_t backlitState);
  wchar_t getPressedKey ();
  wchar_t getLastPressedKey();

};

#endif
