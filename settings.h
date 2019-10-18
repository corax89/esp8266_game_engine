#define ESPBOY
#define RAM_SIZE 20 * 1024
#define FREQUENCY    160    // valid 80, 160
#define APSSID "ESPboy"
#define APPSK  "87654321"

#ifdef ESPBOY
  #define MCP23017address 0
  #define csTFTMCP23017pin 8
  #define LEDquantity     1
  #define LEDPIN         D4
  #define SOUNDPIN       D3
#else
  #define SOUNDPIN       -1
  #define DEBUG_ON_SCREEN
#endif

#define SCREEN_WIDTH 128
#define SCREEN_WIDTH_BYTES 64
#define SCREEN_HEIGHT 128
#ifdef ESPBOY
  #define SCREEN_REAL_WIDTH 128
  #define SCREEN_REAL_HEIGHT 128
#else
  #define SCREEN_REAL_WIDTH 320
  #define SCREEN_REAL_HEIGHT 240
#endif
#define SCREEN_SIZE (SCREEN_HEIGHT * SCREEN_WIDTH_BYTES)
#define SCREEN_ARRAY_DEF SCREEN_SIZE
#define SCREEN_ADDR(x, y) ((int(y) << 6) + int(x))

#define PARTICLE_COUNT 24
#define EEPROM_SIZE   512
