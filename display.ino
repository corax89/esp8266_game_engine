#include "font_a.c"
#include "settings.h"

#define SPRITE_IS_SOLID(a)              (sprite_table[a].flags & 1)
#define SPRITE_IS_SCROLLED(a)           (sprite_table[a].flags & 2)
#define SPRITE_IS_ONEBIT(a)             (sprite_table[a].flags & 4)
#define SPRITE_IS_FLIP_HORIZONTAL(a)    (sprite_table[a].flags & 8)

struct sprite {
  uint16_t address;
  int16_t x;
  int16_t y;
  uint8_t width;
  uint8_t height;
  int8_t speedx;
  int8_t speedy;
  int16_t angle;
  int8_t lives;
  int8_t collision;
  uint8_t flags; //0 0 0 0 fliphorizontal isonebit scrolled solid
  int8_t gravity;
  uint16_t oncollision;
  uint16_t onexitscreen;
};

struct Particle {
  int16_t time;
  int16_t x;
  int16_t y;
  int8_t gravity;
  int8_t speedx;
  int8_t speedy;
  int8_t color;
};

struct Emitter { 
  int16_t time;
  int16_t timer;
  int16_t timeparticle;
  uint8_t count;
  int8_t gravity;
  int16_t x;
  int16_t y;
  int8_t speedx;
  int8_t speedy;
  int8_t speedx1;
  int8_t speedy1;
  int8_t color;
};

struct Tile { 
  int16_t adr;
  uint8_t imgwidth;
  uint8_t imgheight;
  uint8_t width;
  uint8_t height;
  int16_t x;
  int16_t y;
  uint16_t pixwidth;
  uint16_t pixheight;
};

static const int8_t cosT[] PROGMEM = {
  0x40, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3e, 0x3e, 0x3e, 0x3e, 0x3d, 0x3d, 0x3d, 0x3c, 0x3c, 
  0x3c, 0x3b, 0x3b, 0x3a, 0x3a, 0x3a, 0x39, 0x39, 0x38, 0x37, 0x37, 0x36, 0x36, 0x35, 0x35, 0x34, 0x33, 0x33, 0x32, 0x31, 
  0x31, 0x30, 0x2f, 0x2e, 0x2e, 0x2d, 0x2c, 0x2b, 0x2a, 0x29, 0x29, 0x28, 0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20, 
  0x20, 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 0xf, 0xe, 0xd, 0xc, 0xb, 
  0xa, 0x8, 0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8, 0xf7, 0xf5, 0xf4, 0xf3, 0xf2, 
  0xf1, 0xf0, 0xef, 0xee, 0xed, 0xec, 0xeb, 0xea, 0xe9, 0xe8, 0xe6, 0xe5, 0xe4, 0xe3, 0xe2, 0xe1, 0xe0, 0xe0, 0xdf, 0xde, 
  0xdd, 0xdc, 0xdb, 0xda, 0xd9, 0xd8, 0xd7, 0xd6, 0xd6, 0xd5, 0xd4, 0xd3, 0xd2, 0xd1, 0xd1, 0xd0, 0xcf, 0xce, 0xce, 0xcd, 
  0xcc, 0xcc, 0xcb, 0xca, 0xca, 0xc9, 0xc9, 0xc8, 0xc8, 0xc7, 0xc6, 0xc6, 0xc5, 0xc5, 0xc5, 0xc4, 0xc4, 0xc3, 0xc3, 0xc3, 
  0xc2, 0xc2, 0xc2, 0xc1, 0xc1, 0xc1, 0xc1, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 
  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc1, 0xc1, 0xc1, 0xc1, 0xc2, 0xc2, 0xc2, 0xc3, 0xc3, 0xc3, 0xc4, 0xc4, 
  0xc5, 0xc5, 0xc5, 0xc6, 0xc6, 0xc7, 0xc8, 0xc8, 0xc9, 0xc9, 0xca, 0xca, 0xcb, 0xcc, 0xcc, 0xcd, 0xce, 0xce, 0xcf, 0xd0, 
  0xd1, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, 0xdf, 0xe0, 0xe1, 
  0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf7, 
  0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x1f, 0x20, 0x21, 0x22, 0x23, 
  0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2e, 0x2f, 0x30, 0x31, 0x31, 0x32, 0x33, 0x33, 
  0x34, 0x35, 0x35, 0x36, 0x36, 0x37, 0x37, 0x38, 0x39, 0x39, 0x3a, 0x3a, 0x3a, 0x3b, 0x3b, 0x3c, 0x3c, 0x3c, 0x3d, 0x3d, 
  0x3d, 0x3e, 0x3e, 0x3e, 0x3e, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f
};
static const int8_t sinT[] PROGMEM = {
  0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
  0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x29, 0x2a, 
  0x2b, 0x2c, 0x2d, 0x2e, 0x2e, 0x2f, 0x30, 0x31, 0x31, 0x32, 0x33, 0x33, 0x34, 0x35, 0x35, 0x36, 0x36, 0x37, 0x37, 0x38, 
  0x39, 0x39, 0x3a, 0x3a, 0x3a, 0x3b, 0x3b, 0x3c, 0x3c, 0x3c, 0x3d, 0x3d, 0x3d, 0x3e, 0x3e, 0x3e, 0x3e, 0x3f, 0x3f, 0x3f, 
  0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3e, 0x3e, 
  0x3e, 0x3e, 0x3d, 0x3d, 0x3d, 0x3c, 0x3c, 0x3c, 0x3b, 0x3b, 0x3a, 0x3a, 0x3a, 0x39, 0x39, 0x38, 0x37, 0x37, 0x36, 0x36, 
  0x35, 0x35, 0x34, 0x33, 0x33, 0x32, 0x31, 0x31, 0x30, 0x2f, 0x2e, 0x2e, 0x2d, 0x2c, 0x2b, 0x2a, 0x29, 0x29, 0x28, 0x27, 
  0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20, 0x20, 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x17, 0x16, 0x15, 0x14, 0x13, 
  0x12, 0x11, 0x10, 0xf, 0xe, 0xd, 0xc, 0xb, 0xa, 0x8, 0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 
  0xf9, 0xf8, 0xf7, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0, 0xef, 0xee, 0xed, 0xec, 0xeb, 0xea, 0xe9, 0xe8, 0xe6, 0xe5, 0xe4, 
  0xe3, 0xe2, 0xe1, 0xe0, 0xe0, 0xdf, 0xde, 0xdd, 0xdc, 0xdb, 0xda, 0xd9, 0xd8, 0xd7, 0xd6, 0xd6, 0xd5, 0xd4, 0xd3, 0xd2, 
  0xd1, 0xd1, 0xd0, 0xcf, 0xce, 0xce, 0xcd, 0xcc, 0xcc, 0xcb, 0xca, 0xca, 0xc9, 0xc9, 0xc8, 0xc8, 0xc7, 0xc6, 0xc6, 0xc5, 
  0xc5, 0xc5, 0xc4, 0xc4, 0xc3, 0xc3, 0xc3, 0xc2, 0xc2, 0xc2, 0xc1, 0xc1, 0xc1, 0xc1, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 
  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc1, 0xc1, 0xc1, 0xc1, 0xc2, 
  0xc2, 0xc2, 0xc3, 0xc3, 0xc3, 0xc4, 0xc4, 0xc5, 0xc5, 0xc5, 0xc6, 0xc6, 0xc7, 0xc8, 0xc8, 0xc9, 0xc9, 0xca, 0xca, 0xcb, 
  0xcc, 0xcc, 0xcd, 0xce, 0xce, 0xcf, 0xd0, 0xd1, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 
  0xdc, 0xdd, 0xde, 0xdf, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 
  0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe
};
static const uint8_t keyboardImage[] PROGMEM = {
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x31,0x17,0x9c,0x7d,0x12,0x21,0xe,0x71,0xc7,0xc,
  0x60,0x84,0x0,0x79,0xe7,0x84,0x49,0x14,0x12,0x10,0xa2,0x20,0x11,0x49,0x1,0x8,0x21,0x2,0x1f,0x9,0x24,0x88,0x49,0x17,
  0x1c,0x10,0x42,0x21,0x11,0x71,0x1,0x10,0x11,0x2,0x0,0x11,0xe7,0x9f,0x49,0x54,0x18,0x10,0x42,0x21,0x11,0x41,0x1,0x8,
  0x21,0x2,0x1f,0x21,0x20,0x88,0x51,0x54,0x14,0x10,0x42,0x21,0x11,0x41,0x1,0x8,0x21,0x2,0x0,0x21,0x20,0x84,0x28,0xa7,
  0x92,0x10,0x41,0xc1,0xe,0x41,0xc7,0xc,0x60,0x84,0x0,0x21,0xe7,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x30,0xe7,0x1e,0x39,0x20,0x84,0x90,
  0x41,0x5,0x1,0x28,0x43,0x9e,0x49,0xe7,0x85,0x49,0x4,0x90,0x45,0x20,0x85,0x10,0x0,0x5,0x2,0x7c,0xf4,0x52,0x49,0x4,0x9,
  0x78,0x84,0x9c,0x41,0xe0,0x86,0x10,0x0,0x0,0x4,0x29,0x45,0xd2,0x79,0xe7,0x9f,0x48,0x44,0x90,0x4d,0x20,0x85,0x10,0x41,
  0x0,0x8,0x28,0xe5,0x92,0x8,0x24,0x88,0x48,0x24,0x90,0x45,0x22,0x84,0x90,0x1,0x0,0x10,0x7c,0x54,0x12,0x8,0x24,0x84,0x49,
  0xc7,0x10,0x39,0x21,0x4,0x9e,0x0,0x0,0x0,0x29,0xe3,0xde,0x9,0xe7,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x79,0x13,0x11,0x79,0x22,0x21,0x10,0x70,
  0x0,0x10,0x60,0x44,0x40,0x11,0xe7,0x80,0x8,0xa4,0x91,0x45,0xa3,0x62,0x8,0x10,0x0,0x10,0x64,0x42,0x80,0x30,0x20,0x80,0x10,
  0x44,0xa,0x79,0x62,0xa4,0x4,0x70,0x0,0x10,0x9,0xf1,0x1f,0x11,0xe7,0x9b,0x20,0x44,0xa,0x45,0x22,0x22,0x8,0x40,0x0,0x10,
  0x10,0x42,0x80,0x11,0x0,0x91,0x40,0xa4,0x84,0x45,0x22,0x21,0x10,0x0,0x4,0x0,0x2c,0x44,0x40,0x11,0x0,0x9b,0x79,0x13,0x4,
  0x79,0x22,0x20,0x0,0x41,0x4,0x10,0x4c,0x0,0x0,0x39,0xe7,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0
};

static const uint8_t pauseImage[] PROGMEM = {
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xf1,0xe4,0x27,0xbe,0x8a,0x14,0x28,0x20,0x8a,0x14,0x28,0x20,0x8a,0x14,0x28,0x20,0xf3,0xf4,
  0x27,0x38,0x82,0x14,0x20,0xa0,0x82,0x14,0x20,0xa0,0x82,0x14,0x20,0xa0,0x82,0x14,0x20,0xa0,0x82,0x13,0xcf,0x3e,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x80,0x0,0x0,0x2,0x40,0xea,0xcc,0xc2,0x40,0xac,0x91,0x3,0xc0,0xe8,0xc8,0x82,0x40,0x88,0x84,
  0x42,0x40,0x88,0xd9,0x82,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xee,0xa1,0x59,0xb7,0x8a,0xc1,0x92,0x22,0xca,
  0x81,0x19,0x32,0x8a,0x81,0x10,0xa2,0x8e,0x81,0x1b,0x32,0x0,0x0,0x0,0x0
};
uint8_t *screen __attribute__ ((aligned));
uint8_t *sprite_screen __attribute__ ((aligned));
uint8_t line_is_draw[128] __attribute__ ((aligned));
char charArray[340] __attribute__ ((aligned));
uint16_t pix_buffer[SCREEN_REAL_WIDTH] __attribute__ ((aligned));
uint16_t rscreenWidth;
uint16_t rscreenHeight;
uint16_t displayXOffset = 32;
struct sprite sprite_table[32] __attribute__ ((aligned));
struct Particle particles[PARTICLE_COUNT] __attribute__ ((aligned));
struct Emitter emitter;
struct Tile tile;
int8_t imageSize = 1;
int8_t regx = 0;
int8_t regy = 0;
int8_t isDrawKeyboard = 0;
int8_t keyboardPos = 0;

#pragma GCC optimize ("-O2")
#pragma GCC push_options

void memoryAlloc(){
  screen = (uint8_t*)malloc(SCREEN_ARRAY_DEF * sizeof(uint8_t));
  if(screen == NULL)
    Serial.println(F("Out of memory"));
  sprite_screen = (uint8_t*)malloc(SCREEN_ARRAY_DEF * sizeof(uint8_t));
  if(sprite_screen == NULL)
    Serial.println(F("Out of memory"));
}

inline int16_t getCos(int16_t g){
  return (int16_t)(int8_t)pgm_read_byte_near(cosT + g);
}

inline int16_t getSin(int16_t g){
  return (int16_t)(int8_t)pgm_read_byte_near(sinT + g);
}

int16_t atan2_fp(int16_t y_fp, int16_t x_fp){
  int32_t coeff_1 = 45;
  int32_t coeff_1b = -56; // 56.24;
  int32_t coeff_1c = 11;  // 11.25
  int16_t coeff_2 = 135;
  int16_t angle = 0;
  int32_t r;
  int32_t r3;
  int16_t y_abs_fp = y_fp;
  if (y_abs_fp < 0)
    y_abs_fp = -y_abs_fp;
  if (y_fp == 0){
    if (x_fp >= 0){
      angle = 0;
    }
    else{
      angle = 180;
    }
  }
  else if (x_fp >= 0){
    r = (((int32_t)(x_fp - y_abs_fp)) << MULTIPLY_FP_RESOLUTION_BITS) / ((int32_t)(x_fp + y_abs_fp));
    r3 = r * r;
    r3 =  r3 >> MULTIPLY_FP_RESOLUTION_BITS;
    r3 *= r;
    r3 =  r3 >> MULTIPLY_FP_RESOLUTION_BITS;
    r3 *= coeff_1c;
    angle = (int16_t) (coeff_1 + ((coeff_1b * r + r3) >> MULTIPLY_FP_RESOLUTION_BITS));
  }
  else{
    r = (((int32_t)(x_fp + y_abs_fp)) << MULTIPLY_FP_RESOLUTION_BITS) / ((int32_t)(y_abs_fp - x_fp));
    r3 = r * r;
    r3 =  r3 >> MULTIPLY_FP_RESOLUTION_BITS;
    r3 *= r;
    r3 =  r3 >> MULTIPLY_FP_RESOLUTION_BITS;
    r3 *= coeff_1c;
    angle = coeff_2 + ((int16_t)  (((coeff_1b * r + r3) >> MULTIPLY_FP_RESOLUTION_BITS)));
  }
  if (y_fp < 0)
    return (-angle);     // negate if in quad III or IV
  else
    return (angle);
}

void display_init(){
  for(int8_t i = 0; i < 32; i++){
    sprite_table[i].address = 0;
    sprite_table[i].x = -255;
    sprite_table[i].y = -255;
    sprite_table[i].width = 8;
    sprite_table[i].height = 8;
    sprite_table[i].speedx = 0;
    sprite_table[i].speedy = 0;
    sprite_table[i].angle = 0;
    sprite_table[i].lives = 0;
    sprite_table[i].collision = -1;
    sprite_table[i].flags = 2; //isonebit = 0 scrolled = 1 solid = 0
    sprite_table[i].gravity = 0;
    sprite_table[i].oncollision = 0;
  }
  for(int8_t i = 0; i < 16; i++){
    palette[i] = bpalette[i];
    sprtpalette[i] = bpalette[i];
  }
  emitter.time = 0;
  emitter.timer = 0;
  tile.adr = 0;
  for(int8_t i = 0; i < PARTICLE_COUNT; i++)
    particles[i].time = 0;
  for(uint16_t i = 0; i < 340; i++)
    charArray[i] = 0;
  imageSize = 1;
  regx = 0;
  regy = 0;
}

void pause(){
  uint8_t prevKey = 128;
  drawPause();
  redrawScreen();
  while(1){
    delay(100);
    getKey();
    if((thiskey & 128) && prevKey != 128){
      thiskey = 0;
      delay(800);
      return;
    }
    if(thiskey & 16){
      clearSpriteScr();
      while(thiskey & 16){
        delay(100);
        getKey();
      }
      thiskey = 0;
      fileList("/");
      return;
    }
    prevKey = thiskey;
  }
}

void drawPause(){
  int16_t i = 0;
  uint8_t bit;
  uint16_t adr = 0;
  for(int8_t y = 0; y < 30; y++)
    for(uint8_t x = 0; x < 32; x++){
      if(i % 8 == 0){
        bit = pgm_read_byte_near(pauseImage + adr);
        adr++;
      }
      if(bit & 0x80)
        drawSprPixel(1, 48, 48, x, y);
      else
        drawSprPixel(11, 48, 48, x, y);
      bit = bit << 1;
      i++;
    }
}

inline int8_t randomD(int8_t a, int8_t b) {
  int8_t minv = a < b ? a : b;
  int8_t maxv = a > b ? a : b;
  return random(minv, maxv + 1);
}

void setParticle(int8_t gravity, uint8_t count, uint16_t time){
  emitter.gravity = gravity;
  emitter.count = count;
  emitter.timeparticle = time;
}

void setEmitter(uint16_t time, int16_t dir, int16_t dir1, int16_t speed){
  dir = dir % 360;
  if(dir < 0)
    dir += 360;
  emitter.time = time;
  emitter.speedx = (int8_t)((speed * getCos(dir)) >> 6);
  emitter.speedy = (int8_t)((speed * getSin(dir)) >> 6);
  emitter.speedx1 = (int8_t)((speed * getCos(dir1)) >> 6);
  emitter.speedy1 = (int8_t)((speed * getSin(dir1)) >> 6);
}

void drawParticle(int16_t x, int16_t y, uint8_t color){
  emitter.x = x << 1;
  emitter.y = y << 1;
  emitter.color = color;
  emitter.timer = emitter.time;
}

void setScreenResolution(uint16_t nw, uint16_t nh){
  if(nw < SCREEN_REAL_WIDTH)
    rscreenWidth = nw;
  else
    rscreenWidth = SCREEN_REAL_WIDTH - 1;
  if(nh < SCREEN_REAL_HEIGHT)
    rscreenHeight = nh;
  else
    rscreenHeight = SCREEN_REAL_HEIGHT - 1;
  if(rscreenWidth < 95)
    rscreenWidth = 95;
  if(rscreenHeight < 95)
    rscreenHeight = 95;
  displayXOffset = (SCREEN_REAL_WIDTH - rscreenWidth) / 2;
  for(uint8_t i = 0; i < 128; i++)
      line_is_draw[i] = 3;
  tft.fillScreen(0x0000);
}

uint16_t getDisplayXOffset(){
  return displayXOffset;
}

void redrawScreen(){
    int x_ratio = ( int ) ((128 << 16) / rscreenWidth);
    int y_ratio = ( int ) ((128 << 16) / rscreenHeight);
    uint16_t x2, hx2, y2, startx, endx, startj;
    int16_t prevy2 = -1;
    for(int16_t i = 0; i < rscreenHeight; i++) {
      y2 = ((i * y_ratio) >> 16);
      if(line_is_draw[y2]){
        if(line_is_draw[y2] == 2){
          startx = displayXOffset + rscreenWidth / 2 - 2;
          startj = rscreenWidth / 2 - 2;
        }
        else{
          startx = displayXOffset;
          startj = 0;
        }
        if(line_is_draw[y2] == 1)
          endx = displayXOffset + rscreenWidth / 2 + 2;
        else
          endx = displayXOffset + rscreenWidth;
        tft.setAddrWindow(startx, i, endx, i  + 1);
        if(prevy2 != y2)
          for ( int j = startj; j < rscreenWidth; j++) {
            x2 = ((j * x_ratio) >> 16);
            hx2 = x2 / 2;
            if(x2 & 1){
              if((sprite_screen[SCREEN_ADDR(hx2,y2)] & 0xf) > 0)
                pix_buffer[j - startj] = sprtpalette[(sprite_screen[SCREEN_ADDR(hx2,y2)] & 0xf)];
              else
                pix_buffer[j - startj] = palette[(screen[SCREEN_ADDR(hx2,y2)] & 0xf)];
            }
            else{
              if((sprite_screen[SCREEN_ADDR(hx2,y2)] & 0xf0) > 0)
                pix_buffer[j - startj] = sprtpalette[(sprite_screen[SCREEN_ADDR(hx2,y2)] & 0xf0) >> 4];
              else
                pix_buffer[j - startj] = palette[(screen[SCREEN_ADDR(hx2,y2)] & 0xf0) >> 4];
            }
        }
        prevy2 = y2;
        tft.pushColors(pix_buffer, endx - startx);
      }              
    }
    for(uint8_t i = 0; i < 128; i++)
      line_is_draw[i] = 0;
}

void setRedrawRect(uint8_t s, uint8_t e){
  for(uint8_t i = s; i < e; i++)
     line_is_draw[i] = 3;
}

void redrawParticles(){
  int16_t i, n;
  uint8_t x, y;
  if(emitter.timer > 0){
    emitter.timer -= 50;
    i = emitter.count;
    for(n = 0; n < PARTICLE_COUNT; n++){
      if(i == 0)
        break;
      if(particles[n].time <= 0){
        i--;
        particles[n].time = emitter.timeparticle;
        particles[n].x = emitter.x;
        particles[n].y = emitter.y;
        particles[n].color = emitter.color;
        particles[n].speedx = randomD(emitter.speedx, emitter.speedx1);
        particles[n].speedy = randomD(emitter.speedy, emitter.speedy1);
        particles[n].gravity = emitter.gravity;
      }
    }
  }
  for(n = 0; n < PARTICLE_COUNT; n++)
    if(particles[n].time > 0){
      x = ((particles[n].x >> 1) & 127) / 2;
      y = (particles[n].y >> 1) & 127;
      if(particles[n].x & 1)
        sprite_screen[SCREEN_ADDR(x,y)] = (sprite_screen[SCREEN_ADDR(x,y)] & 0xf0) + (particles[n].color & 0x0f);
      else
        sprite_screen[SCREEN_ADDR(x,y)] = (sprite_screen[SCREEN_ADDR(x,y)] & 0x0f) + ((particles[n].color & 0x0f) << 4);
      line_is_draw[y] |= 1 + x / 32;
      particles[n].time -= 50;
      if(random(0,2)){
        particles[n].x += particles[n].speedx;
        particles[n].speedy += particles[n].gravity;
        particles[n].y += particles[n].speedy;
      }
      else{
        particles[n].x += particles[n].speedx / 2;
        particles[n].y += particles[n].speedy / 2;
      }
      if(particles[n].x < 0 || particles[n].x > 256 || particles[n].y < 0 || particles[n].y > 256)
          particles[n].time = 0;
    }
}

int8_t getSpriteInXY(int16_t x, int16_t y){
  for(int8_t n = 0; n < 32; n++){
    if(sprite_table[n].lives > 0)
      if((sprite_table[n].x >> 2) < x && (sprite_table[n].x >> 2) + sprite_table[n].width > x &&
        (sprite_table[n].y >> 2) < y  && (sprite_table[n].y >> 2) + sprite_table[n].height > y)
          return n;
  }
  return - 1;
}

inline void moveSprites(){
  for(uint8_t i = 0; i < 32; i++){
    if(sprite_table[i].lives > 0){   
      sprite_table[i].speedy += sprite_table[i].gravity;
      sprite_table[i].x += sprite_table[i].speedx;
      sprite_table[i].y += sprite_table[i].speedy;
    }
  }
}

inline void redrawSprites(){
  for(uint8_t i = 0; i < 32; i++){
    if(sprite_table[i].lives > 0){
      if((sprite_table[i].x >> 2) + sprite_table[i].width < 0 || (sprite_table[i].x >> 2) > 127 
        || (sprite_table[i].y >> 2) + sprite_table[i].height < 0 || (sprite_table[i].y >> 2) > 127){
          if(sprite_table[i].onexitscreen > 0)
               setinterrupt(sprite_table[i].onexitscreen, i);
      }
      else
        drawSpr(i, sprite_table[i].x >> 2, sprite_table[i].y >> 2);
    }
  }
}

uint16_t getTileInXY(int16_t x, int16_t y){
  uint32_t p;
  if(x < tile.x || y < tile.y || x > tile.x + tile.pixwidth || y > tile.y + tile.pixheight)
    return 0;
  p = ((x - tile.x) / (int16_t)tile.imgwidth) + ((y - tile.y) / (int16_t)tile.imgheight * (int16_t)tile.width);
  return readInt(tile.adr + p * 2);
}

uint16_t getTail(int16_t x, int16_t y){
  if(x < 0 || x >= tile.width || y < 0 || y >= tile.height)
    return 0;
  return readInt(tile.adr + (x + y * tile.width) * 2);
}

void resolveCollision(uint8_t n, uint8_t i){
  int16_t startx, starty, startix, startiy;
  startx = sprite_table[n].x;
  starty = sprite_table[n].y;
  startix = sprite_table[i].x;
  startiy = sprite_table[i].y;
  sprite_table[n].x = sprite_table[n].x - sprite_table[n].speedx;
  sprite_table[n].y = sprite_table[n].y - sprite_table[n].speedy;
  sprite_table[i].x = sprite_table[i].x - sprite_table[i].speedx;
  sprite_table[i].y = sprite_table[i].y - sprite_table[i].speedy;
  if((sprite_table[n].speedy >= 0 && sprite_table[i].speedy <= 0) || (sprite_table[n].speedy <= 0 && sprite_table[i].speedy >= 0)){
    if(sprite_table[n].y > sprite_table[i].y){
      if(sprite_table[i].gravity){
        sprite_table[i].y = sprite_table[n].y - (sprite_table[i].height << 2);
      }
    }
    else{
      if(sprite_table[n].gravity){
        sprite_table[n].y = sprite_table[i].y - (sprite_table[n].height << 2);
      }
    }
  }
  if(sprite_table[n].x < sprite_table[i].x + (sprite_table[i].width << 2) && 
    sprite_table[n].x + (sprite_table[n].width << 2) > sprite_table[i].x &&
    sprite_table[n].y < sprite_table[i].y + (sprite_table[i].height << 2) && 
    sprite_table[n].y + (sprite_table[n].height << 2) > sprite_table[i].y){
      if(sprite_table[n].x > sprite_table[i].x){
        sprite_table[n].x++;
        sprite_table[i].x--;
      }
      else{
        sprite_table[n].x--;
        sprite_table[i].x++;
      }
      if(sprite_table[n].y > sprite_table[i].y){
        sprite_table[n].y++;
        sprite_table[i].y--;
      }
      else{
        sprite_table[n].y--;
        sprite_table[i].y++;
      }
    }
  if(sprite_table[n].gravity){
    sprite_table[n].speedx = (sprite_table[n].x - startx)/4;
    sprite_table[n].speedy = (sprite_table[n].y - starty)/4;
  }
  else{
    sprite_table[n].speedx = sprite_table[n].x - startx;
    sprite_table[n].speedy = sprite_table[n].y - starty;
  }
  if(sprite_table[i].gravity){  
    sprite_table[i].speedx = (sprite_table[i].x - startix)/4;
    sprite_table[i].speedy = (sprite_table[i].y - startiy)/4;
  }
  else{
    sprite_table[i].speedx = sprite_table[i].x - startix;
    sprite_table[i].speedy = sprite_table[i].y - startiy;
  }
}

void testSpriteCollision(){
  uint8_t n, i;
  int16_t x0, y0, x1, y1, newspeed;
  for(n = 0; n < 32; n++)
    sprite_table[n].collision = -1;
  for(n = 0; n < 32; n++){
    if(sprite_table[n].lives > 0){
      x0 = sprite_table[n].x >> 2;
      y0 = sprite_table[n].y >> 2;
      for(i = 0; i < n; i++){
        if(sprite_table[i].lives > 0){
          x1 = sprite_table[i].x >> 2;
          y1 = sprite_table[i].y >> 2;
          if(x0 < x1 + sprite_table[i].width && x0 + sprite_table[n].width > x1 
          && y0 < y1 + sprite_table[i].height && y0 + sprite_table[n].height > y1){
            sprite_table[n].collision = i;
            sprite_table[i].collision = n;
            if(sprite_table[n].oncollision > 0)
              setinterrupt(sprite_table[n].oncollision, n);
            if(sprite_table[i].oncollision > 0)
              setinterrupt(sprite_table[i].oncollision, i);
            if(SPRITE_IS_SOLID(n) && SPRITE_IS_SOLID(i)){
              resolveCollision(n,i);
            }
          }
        }
      }
      if((SPRITE_IS_SOLID(n)) && tile.adr > 0){
              if(getTileInXY(x0, y0) || getTileInXY(x0 + sprite_table[n].width, y0)
              || getTileInXY(x0 , y0 + sprite_table[n].height) || getTileInXY(x0 + sprite_table[n].width, y0 + sprite_table[n].height)){
                sprite_table[n].y = sprite_table[n].y - sprite_table[n].speedy;
                y0 = sprite_table[n].y >> 2;
                if(getTileInXY(x0, y0) 
                  || getTileInXY(x0 + sprite_table[n].width, y0)|| getTileInXY(x0, y0 + sprite_table[n].height)
                  || getTileInXY(x0 + sprite_table[n].width,y0 + sprite_table[n].height)){
                    sprite_table[n].x = sprite_table[n].x - sprite_table[n].speedx;
                    sprite_table[n].speedx = (sprite_table[n].x - (sprite_table[n].x - sprite_table[n].speedx)) / 2;
                  }
                sprite_table[n].speedy = sprite_table[n].speedy / 2 - sprite_table[n].gravity;
                x0 = sprite_table[n].x >> 2;
                y0 = sprite_table[n].y >> 2;
                if(getTileInXY(x0, y0 + sprite_table[n].height) || getTileInXY(x0 + sprite_table[n].width, y0 + sprite_table[n].height)){
                    sprite_table[n].y--;
                  }
              }
         
        }
        
    }
  }
}

inline void clearSpriteScr(){
  for(uint8_t y = 0; y < 128; y ++)
    for(uint8_t x = 0; x < 64; x += 4){
      if(*((uint32_t*)&sprite_screen[SCREEN_ADDR(x,y)]) > 0)
        line_is_draw[y] |= 1 + x / 32;
    }
  memset(sprite_screen, 0, SCREEN_SIZE);
}

inline void clearScr(uint8_t color){
  for(uint8_t y = 0; y < 128; y ++){
    for(uint8_t x = 0; x < 128; x++)
      setPix(x, y, color);
  }
}

void setImageSize(uint8_t size){
  imageSize = size;
}

void setSpr(uint16_t n, uint16_t adr){
  sprite_table[n].address = adr;
}

void setSprPosition(int8_t n, uint16_t x, uint16_t y){
  sprite_table[n].x = x << 2;
  sprite_table[n].y = y << 2;
}

void spriteSetDirectionAndSpeed(int8_t n, uint16_t speed, int16_t dir){
  dir = dir % 360;
  if(dir < 0)
    dir += 360;
  sprite_table[n].speedx = ((speed * getCos(dir)) >> 6);
  sprite_table[n].speedy = ((speed * getSin(dir)) >> 6);
}

void setSprWidth(int8_t n, uint8_t w){
  sprite_table[n].width = w;
}

void setSprHeight(int8_t n, uint8_t w){
  sprite_table[n].height = w;
}

void setSprSpeedx(int8_t n, int8_t s){
  sprite_table[n].speedx = s;
}

void setSprSpeedy(int8_t n, int8_t s){
  sprite_table[n].speedy = s;
}

int16_t angleBetweenSprites(int8_t n1, int8_t n2){
  int16_t A = atan2_fp(sprite_table[n1].y - sprite_table[n2].y, sprite_table[n1].x - sprite_table[n2].x);
  A = (A < 0) ? A + 360 : A;
  return A;
}

int16_t getSpriteValue(int8_t n, uint8_t t){
  switch(t){
    case 0:
      return sprite_table[n].x >> 2;
    case 1:
      return sprite_table[n].y >> 2;
    case 2:
      return sprite_table[n].speedx;
    case 3:
      return sprite_table[n].speedy;
    case 4:
      return sprite_table[n].width;
    case 5:
      return sprite_table[n].height;
    case 6:
      return sprite_table[n].angle;
    case 7:
      return sprite_table[n].lives;
    case 8:
      return sprite_table[n].collision;
    case 9:
      return SPRITE_IS_SCROLLED(2);
    case 10:
      return sprite_table[n].gravity;
  }
  return 0;
}

void setSpriteValue(int8_t n, uint8_t t, int16_t v){
 switch(t){
    case 0:
      sprite_table[n].x = v << 2;
      return;
    case 1:
      sprite_table[n].y = v << 2;
      return;
    case 2:
      sprite_table[n].speedx = (int8_t) v;
      return;
    case 3:
      sprite_table[n].speedy = (int8_t) v;
      return;
    case 4:
      sprite_table[n].width = v;
      return;
    case 5:
      sprite_table[n].height = v;
      return;
    case 6:
      v = v % 360;
      if(v < 0)
        v += 360;
      sprite_table[n].angle = v;
      return;
    case 7:
      sprite_table[n].lives = v;
      return;
    case 8:
      return;
    case 9:
      if(v != 0)
        sprite_table[n].flags |= 0x01;
      else
        sprite_table[n].flags &= ~0x01;
      return;
    case 10:
      sprite_table[n].gravity = v;
      return;
    case 11:
      sprite_table[n].oncollision = (uint16_t)v;
      return;
    case 12:
      sprite_table[n].onexitscreen = (uint16_t)v;
      return;
    case 13:
      if(v != 0)
        sprite_table[n].flags |= 0x02;
      else
        sprite_table[n].flags &= ~0x02;
      return;
    case 14:
      if(v != 0)
        sprite_table[n].flags |= 0x04;
      else
        sprite_table[n].flags &= ~0x04;
      return;
    case 15:
      if(v != 0)
        sprite_table[n].flags |= 0x08;
      else
        sprite_table[n].flags &= ~0x08;
      return;
 }
}

inline void drawRotateSprPixel(int8_t pixel, int8_t x0, int8_t y0, int16_t x, int16_t y, int16_t hw, int16_t hh, int16_t c, int16_t s){
  int16_t nx = hw + (((x - hw) * c - (y - hh) * s) >> 6);
  int16_t ny = hh + (((y - hh) * c + (x - hw) * s) >> 6);
  int16_t nnx = nx / 2;
  int8_t nnx0 = x0 / 2;
  if(nnx0 + nnx >= 0 && nnx0 + nnx < 64 && y0 + ny >= 0 && y0 + ny < 128){
    if(nx & 1)
      sprite_screen[SCREEN_ADDR(nnx0 + nnx, y0 + ny)] = (sprite_screen[SCREEN_ADDR(nnx0 + nnx, y0 + ny)] & 0xf0) + pixel;
    else
      sprite_screen[SCREEN_ADDR(nnx0 + nnx, y0 + ny)] = (sprite_screen[SCREEN_ADDR(nnx0 + nnx, y0 + ny)] & 0x0f) + (pixel << 4);
    line_is_draw[y0 + ny] |= 1 + (nnx0 + nnx) / 32;
  }
}

inline void drawSprPixel(int8_t pixel, int8_t x0, int8_t y0, int16_t x, int16_t y){
  if(x0 + x >= 0 && x0 + x < 128 && y0 + y >= 0 && y0 + y < 128){
    if((x0 + x) & 1)
      sprite_screen[SCREEN_ADDR((x0 + x) / 2, y0 + y)] = (sprite_screen[SCREEN_ADDR((x0 + x) / 2, y0 + y)] & 0xf0) + pixel;
    else
      sprite_screen[SCREEN_ADDR((x0 + x) / 2, y0 + y)] = (sprite_screen[SCREEN_ADDR((x0 + x) / 2, y0 + y)] & 0x0f) + (pixel << 4);
    line_is_draw[y0 + y] |= 1 + (x0 + x) / 64;
  }
}

void drawSpr(int8_t n, int16_t x, int16_t y){
  uint16_t adr = sprite_table[n].address;
  uint8_t w = sprite_table[n].width;
  uint8_t h = sprite_table[n].height;
  uint8_t ww = w;
  int16_t c, s;
  uint8_t pixel, ibit, i;
  w = w / 2;
  if(!SPRITE_IS_ONEBIT(n)){
    if(!sprite_table[n].angle){
      if(SPRITE_IS_FLIP_HORIZONTAL(n)){
        for(int8_t y1 = 0; y1 < h; y1 ++)
          if(y1 + y >= -h && y1 + y < 128 + h){
            for(int8_t x1 = 0; x1 < w; x1++){
              pixel = readMem(adr + x1 + y1 * w);
              if((pixel & 0xf0) > 0)
                drawSprPixel(pixel >> 4, x, y, ww - x1 * 2, y1);
              if((pixel & 0x0f) > 0)
                drawSprPixel(pixel & 0xf, x, y, ww - (x1 * 2 + 1), y1);
            }
          }
      }
      else{
        for(int8_t y1 = 0; y1 < h; y1 ++)
          if(y1 + y >= -h && y1 + y < 128 + h){
            for(int8_t x1 = 0; x1 < w; x1++){
              pixel = readMem(adr + x1 + y1 * w);
              if((pixel & 0xf0) > 0)
                drawSprPixel(pixel >> 4, x, y, x1 * 2, y1);
              if((pixel & 0x0f) > 0)
                drawSprPixel(pixel & 0xf, x, y, x1 * 2 + 1, y1);
            }
          }
      }
    }
    else{
      c = getCos(sprite_table[n].angle);
      s = getSin(sprite_table[n].angle);
      if(SPRITE_IS_FLIP_HORIZONTAL(n)){
        for(int8_t y1 = 0; y1 < h; y1 ++)
          if(y1 + y >= -h && y1 + y < 128 + h){
            for(int8_t x1 = 0; x1 < w; x1++)
              if(x1 + x >= -w && x1 + x < 128 + w){
                pixel = readMem(adr + x1 + y1 * w);
                if((pixel & 0xf0) > 0)
                  drawRotateSprPixel(pixel >> 4, x, y, ww - x1 * 2, y1, w, h / 2, c, s);
                if((pixel & 0x0f) > 0)
                  drawRotateSprPixel(pixel & 0xf, x, y, ww - (x1 * 2 + 1), y1, w, h / 2, c, s);
              }   
          }
      }
      else{
        for(int8_t y1 = 0; y1 < h; y1 ++)
          if(y1 + y >= -h && y1 + y < 128 + h){
            for(int8_t x1 = 0; x1 < w; x1++)
              if(x1 + x >= -w && x1 + x < 128 + w){
                pixel = readMem(adr + x1 + y1 * w);
                if((pixel & 0xf0) > 0)
                  drawRotateSprPixel(pixel >> 4, x, y, x1 * 2, y1, w, h / 2, c, s);
                if((pixel & 0x0f) > 0)
                  drawRotateSprPixel(pixel & 0xf, x, y, x1 * 2 + 1, y1, w, h / 2, c, s);
              }   
          }
      }
    }
  }
  else{
    i = 0;
    if(!sprite_table[n].angle){
      if(SPRITE_IS_FLIP_HORIZONTAL(n)){
        for(int8_t y1 = 0; y1 < h; y1++)
          for(int8_t x1 = 0; x1 < ww; x1++){
            if(i % 8 == 0){
              ibit = readMem(adr);
              adr++;
            }
            if(ibit & 0x80)
              drawSprPixel(color, x, y, ww - x1, y1);
            ibit = ibit << 1;
            i++;
          }
      }
      else{
        for(int8_t y1 = 0; y1 < h; y1++)
          for(int8_t x1 = 0; x1 < ww; x1++){
            if(i % 8 == 0){
              ibit = readMem(adr);
              adr++;
            }
            if(ibit & 0x80)
              drawSprPixel(color, x, y, x1, y1);
            ibit = ibit << 1;
            i++;
          }
      }
    }
    else{
      c = getCos(sprite_table[n].angle);
      s = getSin(sprite_table[n].angle);
      if(SPRITE_IS_FLIP_HORIZONTAL(n)){
        for(int8_t y1 = 0; y1 < h; y1++)
          for(int8_t x1 = 0; x1 < ww; x1++){
            if(i % 8 == 0){
              ibit = readMem(adr);
              adr++;
            }
            if(ibit & 0x80)
              drawRotateSprPixel(color, x, y, ww - x1, y1, w, h/2, c, s);
            ibit = ibit << 1;
            i++;
          }
      }
      else{
        for(int8_t y1 = 0; y1 < h; y1++)
          for(int8_t x1 = 0; x1 < ww; x1++){
            if(i % 8 == 0){
              ibit = readMem(adr);
              adr++;
            }
            if(ibit & 0x80)
              drawRotateSprPixel(color, x, y, x1, y1, w, h/2, c, s);
            ibit = ibit << 1;
            i++;
          }
      }
    }
  }
}

void drawImg(int16_t a, int16_t x, int16_t y, int16_t w, int16_t h){
  if(imageSize > 1){
    drawImgS(a, x, y, w, h);
    return;
  }
  uint8_t p, color;
  for(int16_t yi = 0; yi < h; yi++)
    for(int16_t xi = 0; xi < w; xi++){
      p = readMem(a);
      color = (p & 0xf0) >> 4;
      if(color > 0){
        setPix(xi + x, yi + y, color);
      }
      xi++;
      color = p & 0x0f;
      if(color > 0){
        setPix(xi + x, yi + y, color);
      }
      a++;
    }
}

void drawImgRLE(int16_t adr, int16_t x1, int16_t y1, int16_t w, int16_t h){
    if(imageSize > 1){
      drawImgRLES(adr, x1, y1, w, h);
      return;
    }
    int16_t i = 0;
    uint8_t repeat = readMem(adr);
    adr++;
    int8_t color1 = (readMem(adr) & 0xf0) >> 4;
    int8_t color2 = readMem(adr) & 0xf;
    while(i < w * h){
      if(repeat > 0x81){
        if(color1 > 0){
          setPix(x1 + i % w, y1 + i / w, color1);
        }
        if(color2 > 0){
          setPix(x1 + i % w + 1, y1 + i / w, color2);
        }
        i += 2;
        adr++;
        repeat--;
        color1 = (readMem(adr) & 0xf0) >> 4;
        color2 = readMem(adr) & 0xf;
      }
      else if(repeat == 0x81){
        repeat = readMem(adr);
        adr++;
        color1 = (readMem(adr) & 0xf0) >> 4;
        color2 = readMem(adr) & 0xf;
      }
      else if(repeat > 0){
        if(color1 > 0){
          setPix(x1 + i % w, y1 + i / w, color1);
        }
        if(color2 > 0){
          setPix(x1 + i % w + 1, y1 + i / w, color2);
        }
        i += 2;
        repeat--;
      }
      else if(repeat == 0){
        adr++;
        repeat = readMem(adr);
        adr++;
        color1 = (readMem(adr) & 0xf0) >> 4;
        color2 = readMem(adr) & 0xf;
      }
    }
  }

void drawImageBit(int16_t adr, int16_t x1, int16_t y1, int16_t w, int16_t h){
  if(imageSize > 1){
    drawImageBitS(adr, x1, y1, w, h);
    return;
  }
  int16_t i = 0;
  uint8_t ibit;
  for(int16_t y = 0; y < h; y++)
    for(int16_t x = 0; x < w; x++){
      if(i % 8 == 0){
        ibit = readMem(adr);
        adr++;
      }
      if(ibit & 0x80)
        setPix(x1 + x, y1 + y, color);
      else
        setPix(x1 + x, y1 + y, bgcolor);
      ibit = ibit << 1;
      i++;
    }
}

void drawImgS(int16_t a, int16_t x, int16_t y, int16_t w, int16_t h){
  uint8_t p, jx, jy, color, s;
  s = imageSize;
  for(int16_t yi = 0; yi < h; yi++)
    for(int16_t xi = 0; xi < w; xi++){
      p = readMem(a);
      color = (p & 0xf0) >> 4;
      if(color > 0){
        for(jx = 0; jx < s; jx++)
              for(jy = 0; jy < s; jy++)
                setPix(xi * s + x + jx, yi * s + y + jy, color);
      }
      xi++;
      color = p & 0x0f;
      if(color > 0){
        for(jx = 0; jx < s; jx++)
              for(jy = 0; jy < s; jy++)
                setPix(xi * s + x + jx, yi * s + y + jy, color);
      }
      a++;
    }
}

void drawImgRLES(int16_t adr, int16_t x1, int16_t y1, int16_t w, int16_t h){
    int16_t i = 0;
    uint8_t jx, jy;
    uint8_t repeat = readMem(adr);
    adr++;
    int8_t color1 = (readMem(adr) & 0xf0) >> 4;
    int8_t color2 = readMem(adr) & 0xf;
    while(i < w * h){
      if(repeat > 0x81){
        if(color1 > 0){
          for(jx = 0; jx < imageSize; jx++)
            for(jy = 0; jy < imageSize; jy++)
              setPix(x1 + (i % w) * imageSize + jx, y1 + i / w * imageSize + jy, color1);
        }
        if(color2 > 0){
          for(jx = 0; jx < imageSize; jx++)
            for(jy = 0; jy < imageSize; jy++)
              setPix(x1 + (i % w) * imageSize + imageSize + jx, y1 + i / w * imageSize + jy, color2);
        }
        i += 2;
        adr++;
        repeat--;
        color1 = (readMem(adr) & 0xf0) >> 4;
        color2 = readMem(adr) & 0xf;
      }
      else if(repeat == 0x81){
        repeat = readMem(adr);
        adr++;
        color1 = (readMem(adr) & 0xf0) >> 4;
        color2 = readMem(adr) & 0xf;
      }
      else if(repeat > 0){
        if(color1 > 0){
          for(jx = 0; jx < imageSize; jx++)
                for(jy = 0; jy < imageSize; jy++)
                  setPix(x1 + (i % w) * imageSize + jx, y1 + i / w * imageSize + jy, color1);
        }
        if(color2 > 0){
          for(jx = 0; jx < imageSize; jx++)
                for(jy = 0; jy < imageSize; jy++)
                  setPix(x1 + (i % w) * imageSize + imageSize + jx, y1 + i / w * imageSize + jy, color2);
        }
        i += 2;
        repeat--;
      }
      else if(repeat == 0){
        adr++;
        repeat = readMem(adr);
        adr++;
        color1 = (readMem(adr) & 0xf0) >> 4;
        color2 = readMem(adr) & 0xf;
      }
    }
  }

void drawImageBitS(int16_t adr, int16_t x1, int16_t y1, int16_t w, int16_t h){
  int16_t i = 0;
  uint8_t ibit, jx, jy;
  for(int16_t y = 0; y < h; y++)
    for(int16_t x = 0; x < w; x++){
      if(i % 8 == 0){
        ibit = readMem(adr);
        adr++;
      }
      if(ibit & 0x80){
        for(jx = 0; jx < imageSize; jx++)
          for(jy = 0; jy < imageSize; jy++)
          setPix(x1 + x * imageSize + jx, y1 + y * imageSize + jy, color);
      } 
      else{
        for(jx = 0; jx < imageSize; jx++)
          for(jy = 0; jy < imageSize; jy++)
            setPix(x1 + x * imageSize + jx, y1 + y * imageSize + jy, bgcolor);
      } 
      ibit = ibit << 1;
      i++;
    }
}

void loadTile(int16_t adr, uint8_t iwidth, uint8_t iheight, uint8_t width, uint8_t height){
    tile.adr = adr;
    tile.imgwidth = iwidth;
    tile.imgheight = iheight;
    tile.width = width;
    tile.height = height;
    tile.pixwidth = width * iwidth;
    tile.pixheight = height * iheight;
  }

void drawTile(int16_t x0, int16_t y0){
    int16_t x, y, nx, ny;
    uint16_t imgadr;
    tile.x = x0;
    tile.y = y0;
    for(x = 0; x < tile.width; x++){
      nx = x0 + x * tile.imgwidth;
      for(y = 0; y < tile.height; y++){
        ny = y0 + y * tile.imgheight;
        if(nx >= -tile.width && nx < 128 && ny >= -tile.height && ny < 128){
          imgadr = readInt(tile.adr + (x + y * tile.width) * 2);
          if(imgadr > 0)
            drawImg(imgadr, nx, ny, tile.imgwidth, tile.imgheight); 
        }
      }
    }
  }

inline void drawFVLine(int16_t x, int16_t y1, int16_t y2){
  for(int16_t  i = y1; i <= y2; i++)
    setPix(x, i, color);
}

inline void drawFHLine(int16_t x1, int16_t x2, int16_t y){
  for(int16_t  i = x1; i <= x2; i++)
    setPix(i, y, color);
}

void drwLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
    if(x1 == x2){
      if(y1 > y2)
        drawFVLine(x1, y2, y1);
      else
        drawFVLine(x1, y1, y2);
      return;
    }
    else if(y1 == y2){
      if(x1 > x2)
        drawFHLine(x2, x1, y1);
      else
        drawFHLine(x1, x2, y1);
      return;
    }
    int16_t deltaX = abs(x2 - x1);
    int16_t deltaY = abs(y2 - y1);
    int16_t signX = x1 < x2 ? 1 : -1;
    int16_t signY = y1 < y2 ? 1 : -1;
    int16_t error = deltaX - deltaY;
    int16_t error2;
    setPix(x2, y2, color);
    while(x1 != x2 || y1 != y2){
      setPix(x1, y1, color);
      error2 = error * 2;
      if(error2 > -deltaY){
        error -= deltaY;
        x1 += signX;
      }
      if(error2 < deltaX){
        error += deltaX;
        y1 += signY;
      }
    }
  }

inline void setPix(uint16_t x, uint16_t y, uint8_t c){
  uint8_t xi = x / 2;
  uint8_t b;
  if(x < 128 && y < 128){
    b = screen[SCREEN_ADDR(xi, y)];
    if(x & 1)
      screen[SCREEN_ADDR(xi, y)] = (screen[SCREEN_ADDR(xi, y)] & 0xf0) + c;
    else
      screen[SCREEN_ADDR(xi, y)] = (screen[SCREEN_ADDR(xi, y)] & 0x0f) + ( c << 4);
    if(b != screen[SCREEN_ADDR(xi, y)])
      line_is_draw[y] |= 1 + x / 64;
  }
}

inline void setPix2(uint16_t xi, uint16_t y, uint8_t c){
  if(xi < 64 && y < 128){
    if (screen[SCREEN_ADDR(xi, y)] != c) {
      screen[SCREEN_ADDR(xi, y)] = c;
      line_is_draw[y] |= 1 + xi /32;
    }
  }
}

uint8_t getPix(uint8_t x, uint8_t y){
  uint8_t b = 0;
  uint8_t xi = x / 2;
  if(x >= 0 && x < 128 && y >= 0 && y < 128){
    if(x % 2 == 0)
      b = (screen[SCREEN_ADDR(xi, y)] & 0xf0) >> 4;
    else
      b = (screen[SCREEN_ADDR(xi, y)] & 0x0f);
  }
  return b;
}

void changePalette(uint8_t n, uint16_t c){
  if(n < 16){
    palette[n] = c;
    for(uint8_t y = 0; y < 128; y++){
      for(uint8_t x = 0; x < 64; x++){
        if(((screen[SCREEN_ADDR(x, y)] & 0xf0) >> 4) == n || (screen[SCREEN_ADDR(x, y)] & 0x0f) == n)
          line_is_draw[y] |= 1 + x / 32;
      }
    }
  }
  else if(n < 32)
    sprtpalette[n - 16] = c;
}

void scrollScreen(uint8_t step, uint8_t direction){
    uint8_t bufPixel;
    if(direction == 2){
      for(uint8_t y = 0; y < 128; y++){
        bufPixel = screen[SCREEN_ADDR(0, y)];
        for(uint8_t x = 1; x < 64; x++){
          if(screen[SCREEN_ADDR(x - 1, y)] != screen[SCREEN_ADDR(x,y)])
            line_is_draw[y] |= 1 + x / 32;
          screen[SCREEN_ADDR(x - 1,  y)] = screen[SCREEN_ADDR(x,y)];
        }
        if(screen[SCREEN_ADDR(63, y)] != bufPixel)
            line_is_draw[y] |= 1;
        screen[SCREEN_ADDR(63, y)] = bufPixel;
      }
      for(uint8_t n = 0; n < 32; n++)
        if(SPRITE_IS_SCROLLED(n))
          sprite_table[n].x -= 8;
    }
    else if(direction == 1){
      for(uint8_t x = 0; x < 64; x++){
        bufPixel = screen[SCREEN_ADDR(x, 0)];
        for(uint8_t y = 1; y < 128; y++){
          if(screen[SCREEN_ADDR(x, y-1)] != screen[SCREEN_ADDR(x,y)])
            line_is_draw[y] |= 1 + x / 32;
          screen[SCREEN_ADDR(x, y - 1)] = screen[SCREEN_ADDR(x,y)];
        }
        if(screen[SCREEN_ADDR(x, 127)] != bufPixel)
            line_is_draw[127] |= 2;
        screen[SCREEN_ADDR(x, 127)] = bufPixel;
      }
      for(uint8_t n = 0; n < 32; n++)
        if(SPRITE_IS_SCROLLED(n))
          sprite_table[n].y -= 4;
    }
    else if(direction == 0){
      for(uint8_t y = 0; y < 128; y++){
        bufPixel = screen[SCREEN_ADDR(63, y)];
        for(uint8_t x = 63; x > 0; x--){
          if(screen[SCREEN_ADDR(x,y)] != screen[SCREEN_ADDR(x - 1, y)])
            line_is_draw[y] |= 1 + x / 32;
          screen[SCREEN_ADDR(x,y)] = screen[SCREEN_ADDR(x - 1, y)];
        }
        if(screen[SCREEN_ADDR(0, y)] != bufPixel)
            line_is_draw[y] |= 1;
        screen[SCREEN_ADDR(0, y)] = bufPixel;
      }
      for(uint8_t n = 0; n < 32; n++)
        if(SPRITE_IS_SCROLLED(n))
          sprite_table[n].x += 8;
    }
    else {
      for(uint8_t x = 0; x < 64; x++){
        bufPixel = screen[SCREEN_ADDR(x, 127)];
        for(uint8_t y = 127; y > 0; y--){
          if(screen[SCREEN_ADDR(x,y)] != screen[SCREEN_ADDR(x, y - 1)])
            line_is_draw[y] |= 1 + x / 32;
          screen[SCREEN_ADDR(x,y)] = screen[SCREEN_ADDR(x, y - 1)];
        }
        if(screen[SCREEN_ADDR(x, 0)] != bufPixel)
            line_is_draw[0] |= 1 + x / 32;
        screen[SCREEN_ADDR(x, 0)] = bufPixel;
      }
      for(uint8_t n = 0; n < 32; n++)
        if(SPRITE_IS_SCROLLED(n))
          sprite_table[n].y += 4;
    }
    if(tile.adr > 0)
      tileDrawLine(step, direction);
}

void tileDrawLine(uint8_t step, uint8_t direction){
    int16_t x,y,x0,y0,y1,nx,ny;
    uint16_t imgadr;
    if(direction == 2){
      tile.x -= step*2;
      x0 = tile.x;
      y0 = tile.y;
      x = (127 - x0) / tile.imgwidth;
      nx = x0 + x * tile.imgwidth;
      if(x < tile.width && x >= -tile.width){
        for(y = 0; y < tile.height; y++){
          ny = y0 + y * tile.imgheight;
          if(ny > -tile.height && ny < 128){
            imgadr = readInt(tile.adr + (x + y * tile.width) * 2);
            if(imgadr > 0)
              drawImg(imgadr, nx, ny, tile.imgwidth, tile.imgheight); 
            else
              fillRect(nx, ny, tile.imgwidth, tile.imgheight, bgcolor);
          }
        }
      }
      else if(tile.width * tile.imgwidth + x0 >= 0){
        y0 = (y0 > 0) ? y0 : 0;
        y1 = (tile.y + tile.height * tile.imgheight < 128) ? tile.y + tile.height * tile.imgheight - y0 : 127 - y0;
        if(y0 < 127 && y1 > 0)
          fillRect(127 - step * 2, y0, step * 2, y1, bgcolor);
      }
    }
    else if(direction == 1){
      tile.y -= step;
      x0 = tile.x;
      y0 = tile.y;
      y = (127 - y0) / tile.imgheight;
      ny = y0 + y * tile.imgheight;
      if(y < tile.height  && y >= -tile.height)
        for(x = 0; x < tile.width; x++){
          nx = x0 + x * tile.imgwidth;
          if(nx > -tile.width && nx < 128){
            imgadr = readInt(tile.adr + (x + y * tile.width) * 2);
            if(imgadr > 0)
              drawImg(imgadr, nx, ny, tile.imgwidth, tile.imgheight); 
            else
              fillRect(nx, ny, tile.imgwidth, tile.imgheight, bgcolor);
          }
        }
    }
    else if(direction == 0){
      tile.x += step*2;
      x0 = tile.x;
      y0 = tile.y;
      x = (0 - x0) / tile.imgwidth;
      nx = x0 + x * tile.imgwidth;
      if(x0 < 0 && x >= -tile.width){
        for(y = 0; y < tile.height; y++){
          ny = y0 + y * tile.imgheight;
          if(ny > -tile.height && ny < 128){
            imgadr = readInt(tile.adr + (x + y * tile.width) * 2);
            if(imgadr > 0)
              drawImg(imgadr, nx, ny, tile.imgwidth, tile.imgheight); 
            else
              fillRect(nx, ny, tile.imgwidth, tile.imgheight, bgcolor);
          }
        }
      }
      else if(x0 < 128){
        y0 = (y0 > 0) ? y0 : 0;
        y1 = (tile.y + tile.height * tile.imgheight < 128) ? tile.y + tile.height * tile.imgheight - y0 : 127 - y0;
        if(y0 < 127 && y1 > 0)
          fillRect(0, y0, step * 2, y1, bgcolor);
      }
    }
    else if(direction == 3){
      tile.y += step;
      x0 = tile.x;
      y0 = tile.y;
      y = (0 - y0) / tile.imgheight;
      ny = y0 + y * tile.imgheight;
      if(y0 < 0  && y >= -tile.height)
        for(x = 0; x < tile.width; x++){
          nx = x0 + x * tile.imgwidth;
          if(nx > -tile.width && nx < 128){
            imgadr = readInt(tile.adr + (x + y * tile.width) * 2);
            if(imgadr > 0)
              drawImg(imgadr, nx, ny, tile.imgwidth, tile.imgheight); 
            else
              fillRect(nx, ny, tile.imgwidth, tile.imgheight, bgcolor);
          }
        }
    }
  }

void charLineUp(uint8_t n){
  clearScr(bgcolor);
  for(uint16_t i = 0; i < 336 - n * 21; i++){
    charArray[i] = charArray[i + n * 21];
    putchar(charArray[i], (i % 21) * 6, (i / 21) * 8);
  }
}

void setCharX(int8_t x){
  regx = x;
}

void setCharY(int8_t y){
  regy = y;
}

int8_t getCharY(){
  return regy;
}

void printc(char c, uint8_t fc, uint8_t bc){
  if(c == '\n'){
    fillRect(regx * 6, regy * 8, 127 - regx * 6, 8, bgcolor);
    for(uint8_t i = regx; i <= 21; i++){
      charArray[regx + regy * 21] = ' ';
    }
    regy++;
    regx = 0;
    if(regy > 15){
      regy = 15;
      charLineUp(1);
    }
  }
  else if(c == '\t'){
    for(uint8_t i = 0; i <= regx % 5; i++){
      fillRect(regx * 6, regy * 8, 6, 8, bgcolor);
      charArray[regx + regy * 21] = ' ';
      regx++;
      if(regx > 21){
        regy++;
        regx = 0;
        if(regy > 15){
          regy = 15;
          charLineUp(1);
        }
      }
    }
  }
  else{
    fillRect(regx * 6, regy * 8, 6, 8, bgcolor);
    putchar(c, regx * 6, regy * 8);
    charArray[regx + regy * 21] = c;
    regx++;
    if(regx > 20){
      regy++;
      regx = 0;
      if(regy > 15){
        regy = 15;
        charLineUp(1);
      }
    }
  }
}


void printfix(int16_t value, uint8_t fc, uint8_t bc){
    char sbuffer[10];
    const uint16_t fractPartMask = (1 << MULTIPLY_FP_RESOLUTION_BITS) - 1;
    int16_t j; 
    if(value == 0){
        printc('0', color, bgcolor);
    }
    if (value < 0){
      printc('-', color, bgcolor);
      value = (~value) + 1;
    }
    int16_t intPart = value >> MULTIPLY_FP_RESOLUTION_BITS;
    value &= fractPartMask;
    
    // преобразуем целую часть
    itoa(intPart, sbuffer, 10);
    j = 0;
    while(sbuffer[j]){
      printc(sbuffer[j], color, bgcolor);
      j++;
    }
    char *ptr = sbuffer;
    // если есть дробная часть
    if(value != 0){
        *ptr = '.';
        for(j = 0; j < 3; j++){
            value &= fractPartMask;
            value *= 10;
            //value <<= 1;
            //value += value << 2;
            *++ptr = (uint8_t)(value >> MULTIPLY_FP_RESOLUTION_BITS) + '0';
        }
        // удаляем завершаюшие нули
        while(ptr[0] == '0') --ptr;
        ptr[1] = 0;
    }
    j = 0;
    while(sbuffer[j]){
      printc(sbuffer[j], color, bgcolor);
      j++;
    }
}

void setColor(uint8_t c){
  color = c & 0xf;
}

void setBgColor(uint8_t c){
  bgcolor = c & 0xf;
}

void drwRect(int16_t x0, int16_t y0, int16_t x1, int16_t y1){
  drawFHLine(x0, x1, y0);
  drawFHLine(x0, x1, y1);
  drawFVLine(x0, y0, y1);
  drawFVLine(x1, y1, y1);
}

void fillRect(int8_t x, int8_t y, uint8_t w, uint8_t h, uint8_t c){
   for(int16_t jx = x; jx < x + w; jx++)
     for(int16_t jy = y; jy < y + h; jy++)
      setPix(jx, jy, c);
}

void fllRect(int16_t x0, int16_t y0, int16_t x1, int16_t y1){
  for(int16_t jy = y0; jy <= y1; jy++)
    drawFHLine(x0, x1, jy);
}

void drwCirc(int16_t x0, int16_t y0, int16_t r) {
  int16_t  x  = 0;
  int16_t  dx = 1;
  int16_t  dy = r+r;
  int16_t  p  = -(r>>1);

  // These are ordered to minimise coordinate changes in x or y
  // drawPixel can then send fewer bounding box commands
  setPix(x0 + r, y0, color);
  setPix(x0 - r, y0, color);
  setPix(x0, y0 - r, color);
  setPix(x0, y0 + r, color);

  while(x<r){

    if(p>=0) {
      dy-=2;
      p-=dy;
      r--;
    }

    dx+=2;
    p+=dx;

    x++;

    // These are ordered to minimise coordinate changes in x or y
    // drawPixel can then send fewer bounding box commands
    setPix(x0 + x, y0 + r, color);
    setPix(x0 - x, y0 + r, color);
    setPix(x0 - x, y0 - r, color);
    setPix(x0 + x, y0 - r, color);

    setPix(x0 + r, y0 + x, color);
    setPix(x0 - r, y0 + x, color);
    setPix(x0 - r, y0 - x, color);
    setPix(x0 + r, y0 - x, color);
  }
}

void fllCirc(int16_t x0, int16_t y0, int16_t r) {
  int16_t  x  = 0;
  int16_t  dx = 1;
  int16_t  dy = r+r;
  int16_t  p  = -(r>>1);

  drawFHLine(x0 - r, x0 + r, y0);

  while(x<r){
    if(p>=0) {
      dy-=2;
      p-=dy;
      r--;
    }

    dx+=2;
    p+=dx;

    x++;

    drawFHLine(x0 - r, x0 + r, y0 + x);
    drawFHLine(x0 - r, x0 + r, y0 - x);
    drawFHLine(x0 - x, x0 + x, y0 + r);
    drawFHLine(x0 - x, x0 + x, y0 - r);
  }
}

void drwTriangle( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3){
  drwLine( x1, y1, x2, y2);
  drwLine( x2, y2, x3, y3);
  drwLine( x3, y3, x1, y1);
}

void fllTriangle( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
  int32_t a, b, y, last, t;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
      t = y0;
      y0 = y1;
      y1 = t;
      t = x0;
      x0 = x1;
      x1 = t;
    }
    if (y1 > y2) {
      t = y2;
      y2 = y1;
      y1 = t;
      t = x2;
      x2 = x1;
      x1 = t;
    }
    if (y0 > y1) {
      t = y0;
      y0 = y1;
      y1 = t;
      t = x0;
      x0 = x1;
      x1 = t;
    }

    if (y0 == y2) {
      a = b = x0;
      if (x1 < a)
        a = x1;
      else if (x1 > b)
        b = x1;
      if (x2 < a)
        a = x2;
      else if (x2 > b)
        b = x2;
      drawFHLine(a, b, y0);
      return;
    }

  int32_t
  dx01 = x1 - x0,
  dy01 = y1 - y0,
  dx02 = x2 - x0,
  dy02 = y2 - y0,
  dx12 = x2 - x1,
  dy12 = y2 - y1,
  sa   = 0,
  sb   = 0;

  if(y1 == y2) 
    last = y1;
  else
    last = y1 - 1;

  for (y = y0; y <= last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;

    if (a > b){
      t = a;
      a = b;
      b = t;
    }
    drawFHLine(a, b, y);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for (; y <= y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;

    if (a > b){
      t = a;
      a = b;
      b = t;
    }
    drawFHLine(a, b, y);
  }
}

void putString(char s[], int8_t y){
  int8_t i = 0;
  while(s[i] != 0 && i < 32){
    if(s[i] != '/'){
      if(i < 17)
        putchar(s[i], 25 + i * 6, y);
      else
        putchar(s[i], 25 + (i - 17) * 6, y + 8);
    }
    i++;
    if(s[i] == '.')
      return;
  }
}

void putchar(char c, uint8_t x, uint8_t y) {
    for(int8_t i = 0; i < 5; i++ ) { // Char bitmap = 5 columns
      uint8_t line = pgm_read_byte(&font_a[c * 5 + i]);
      for(int8_t j = 0; j < 8; j++, line >>= 1) {
        if(line & 1)
         setPix(x + i, y + j, color);
      }
  }
}

#pragma GCC pop_options
