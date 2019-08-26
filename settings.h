#define ESPBOY 1
#define RAM_SIZE 20 * 1024
#define FREQUENCY    160    // valid 80, 160

#ifdef ESPBOY
  #define MCP23017address 0
  #define csTFTMCP23017pin 8
  #define LEDquantity     1
  #define LEDPIN         D4
  #define SOUNDPIN       D3
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
// #define ONE_DIM_SCREEN_ARRAY

#ifndef ONE_DIM_SCREEN_ARRAY
#define SCREEN_ARRAY_DEF SCREEN_HEIGHT][SCREEN_WIDTH_BYTES
#define SCREEN_ADDR(x, y) y][x
#else
#define SCREEN_ARRAY_DEF SCREEN_SIZE
#define SCREEN_ADDR(x, y) ((int(y) << 6) + int(x))
#endif

#define PARTICLE_COUNT 32
#define EEPROM_SIZE   512
