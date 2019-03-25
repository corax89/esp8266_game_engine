#include "font_a.c"

#define PARTICLE_COUNT 32

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
  uint8_t solid;
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

uint8_t screen[64][128];
uint8_t line_is_draw[128];
uint8_t sprite_screen[64][128];
char charArray[340];
uint16_t pix_buffer[256];
struct sprite sprite_table[32];
struct Particle particles[PARTICLE_COUNT];
struct Emitter emitter;
struct Tile tile;
int8_t imageSize = 1;
int8_t regx = 0;
int8_t regy = 0;

int16_t getCos(int16_t g){
  if(g >= 360)
    g = g % 360;
  return (int16_t)(int8_t)pgm_read_byte_near(cosT + g);
}

int16_t getSin(int16_t g){
  if(g >= 360)
    g = g % 360;
  return (int16_t)(int8_t)pgm_read_byte_near(sinT + g);
}

#define MULTIPLY_FP_RESOLUTION_BITS  6

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
  for(byte i = 0; i < 32; i++){
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
    sprite_table[i].solid = 0;
    sprite_table[i].gravity = 0;
    sprite_table[i].oncollision = 0;
  }
  emitter.time = 0;
  emitter.timer = 0;
  tile.adr = 0;
  for(byte i = 0; i < PARTICLE_COUNT; i++)
    particles[i].time = 0;
  for(uint16_t i = 0; i < 340; i++)
      charArray[i] = 0;
  imageSize = 1;
  regx = 0;
  regy = 0;
}

int8_t randomD(int8_t a, int8_t b) {
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
  emitter.time = time;
  emitter.speedx = (int8_t)((speed * getCos(dir)) >> 6);
  emitter.speedy = (int8_t)((speed * getSin(dir)) >> 6);
  emitter.speedx1 = (int8_t)((speed * getCos(dir1)) >> 6);
  emitter.speedy1 = (int8_t)((speed * getSin(dir1)) >> 6);
}

void drawParticle(int16_t x, int16_t y, uint8_t color){
  emitter.x = x;
  emitter.y = y;
  emitter.color = color;
  emitter.timer = emitter.time;
}

void redrawScreen(){
  uint8_t i;
  cadr_count++;
  for(uint8_t y = 0; y < 128; y++){
    i = 0;
    if(line_is_draw[y] == 1){
      if(y < 8)
        tft.setAddrWindow(32 + 0, y, 32 + 127, y  + 1);
      else if(y > 120)
        tft.setAddrWindow(32 + 0, 232 - 120 + y, 32 + 127, 232 - 120 + y  + 1);
      else
        tft.setAddrWindow(32 + 0, y * 2 - 8, 32 + 127, y * 2 + 2 - 8);
      //в одной ячейке памяти содержится два пикселя
      for(uint8_t x = 0; x < 32; x++){
          if((sprite_screen[x][y] & 0xf0) > 0)
            pix_buffer[i] = palette[(sprite_screen[x][y] & 0xf0) >> 4]; 
          else
            pix_buffer[i] = palette[(screen[x][y] & 0xf0) >> 4];
          i++;
          pix_buffer[i] = pix_buffer[i - 1];
          i++;
          if((sprite_screen[x][y] & 0x0f) > 0)
            pix_buffer[i] = palette[sprite_screen[x][y] & 0x0f];
          else
            pix_buffer[i] = palette[screen[x][y] & 0x0f];
          i++;
          pix_buffer[i] = pix_buffer[i - 1];
          i++;
        }
      tft.pushColors(pix_buffer, 128);
      if(y >= 8 && y <= 120)
        tft.pushColors(pix_buffer, 128);
      line_is_draw[y] = 0;
    } 
    else if(line_is_draw[y] == 2){
      if(y < 8)
        tft.setAddrWindow(32 + 128, y, 32 + 255, y  + 1);
      else if(y > 120)
        tft.setAddrWindow(32 + 128, 232 - 120 + y, 32 + 255, 232 - 120 + y  + 1);
      else
        tft.setAddrWindow(32 + 128, y * 2 - 8, 32 + 255, y * 2 + 2 - 8);
      //в одной ячейке памяти содержится два пикселя
      for(uint8_t x = 0; x < 32; x++){
          if((sprite_screen[x + 32][y] & 0xf0) > 0)
            pix_buffer[i] = palette[(sprite_screen[x + 32][y] & 0xf0) >> 4]; 
          else
            pix_buffer[i] = palette[(screen[x + 32][y] & 0xf0) >> 4];
          i++;
          pix_buffer[i] = pix_buffer[i - 1];
          i++;
          if((sprite_screen[x + 32][y] & 0x0f) > 0)
            pix_buffer[i] = palette[sprite_screen[x + 32][y] & 0x0f];
          else
            pix_buffer[i] = palette[screen[x + 32][y] & 0x0f];
          i++;
          pix_buffer[i] = pix_buffer[i - 1];
          i++;
        }
      tft.pushColors(pix_buffer, 128);
      if(y >= 8 && y <= 120)
        tft.pushColors(pix_buffer, 128);
      line_is_draw[y] = 0;
    } 
    else if(line_is_draw[y] == 3){
      if(y < 8)
        tft.setAddrWindow(32 + 0, y, 32 + 255, y  + 1);
      else if(y > 120)
        tft.setAddrWindow(32 + 0, 232 - 120 + y, 32 + 255, 232 - 120 + y  + 1);
      else
        tft.setAddrWindow(32 + 0, y * 2 - 8, 32 + 255, y * 2 + 2 - 8);
      //в одной ячейке памяти содержится два пикселя
      for(uint8_t x = 0; x < 64; x++){
          if((sprite_screen[x][y] & 0xf0) > 0)
            pix_buffer[i] = palette[(sprite_screen[x][y] & 0xf0) >> 4]; 
          else
            pix_buffer[i] = palette[(screen[x][y] & 0xf0) >> 4];
          i++;
          pix_buffer[i] = pix_buffer[i - 1];
          i++;
          if((sprite_screen[x][y] & 0x0f) > 0)
            pix_buffer[i] = palette[sprite_screen[x][y] & 0x0f];
          else
            pix_buffer[i] = palette[screen[x][y] & 0x0f];
          i++;
          pix_buffer[i] = pix_buffer[i - 1];
          i++;
        }
      tft.pushColors(pix_buffer, 256);
      if(y >= 8 && y <= 120)
        tft.pushColors(pix_buffer, 256);
      line_is_draw[y] = 0;
    } 
  }
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
        x = (particles[n].x & 127) / 2;
        y = particles[n].y & 127;
        if(particles[n].x & 1)
          sprite_screen[x][y] = (sprite_screen[x][y] & 0xf0) + (particles[n].color & 0x0f);
        else
          sprite_screen[x][y] = (sprite_screen[x][y] & 0x0f) + ((particles[n].color & 0x0f) << 4);
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
        if(particles[n].x < 0 || particles[n].x > 128 || particles[n].y < 0 || particles[n].y > 128)
            particles[n].time = 0;
      }
}

void redrawSprites(){
  for(byte i = 0; i < 32; i++){
    if(sprite_table[i].lives > 0){   
      sprite_table[i].speedy += sprite_table[i].gravity;
      sprite_table[i].x += sprite_table[i].speedx;
      sprite_table[i].y += sprite_table[i].speedy;
      if(sprite_table[i].x + sprite_table[i].width < 0 || sprite_table[i].x > 127 || sprite_table[i].y + sprite_table[i].height < 0 || sprite_table[i].y > 127){
        if(sprite_table[i].onexitscreen > 0)
           setinterrupt(sprite_table[i].onexitscreen, i);
      }
      else
        drawSpr(i, sprite_table[i].x, sprite_table[i].y);
    }
  }
}

uint16_t getTail(int16_t x, int16_t y){
  if(x < 0 || x >= tile.width || y < 0 || y >= tile.height)
    return 0;
  return readInt(tile.adr + (x + y * tile.width) * 2);
}

void testSpriteCollision(){
  byte n, i;
  int16_t x0, y0, newspeed;
  //int16_t adr;
  for(n = 0; n < 32; n++)
    sprite_table[n].collision = -1;
  for(n = 0; n < 32; n++){
    if(sprite_table[n].lives > 0){
      for(i = 0; i < n; i++){
        if(sprite_table[i].lives > 0){
          if(sprite_table[n].x < sprite_table[i].x + sprite_table[i].width && 
          sprite_table[n].x + sprite_table[n].width > sprite_table[i].x &&
          sprite_table[n].y < sprite_table[i].y + sprite_table[i].height && 
          sprite_table[n].y + sprite_table[n].height > sprite_table[i].y){
            sprite_table[n].collision = i;
            sprite_table[i].collision = n;
            if(sprite_table[n].oncollision > 0)
              setinterrupt(sprite_table[n].oncollision, n);
            if(sprite_table[i].oncollision > 0)
              setinterrupt(sprite_table[i].oncollision, i);
            if(sprite_table[n].solid != 0 && sprite_table[i].solid != 0){
              if((sprite_table[n].speedx >= 0 && sprite_table[i].speedx <= 0) || (sprite_table[n].speedx <= 0 && sprite_table[i].speedx >= 0)){
                newspeed = (abs(sprite_table[n].speedx) + abs(sprite_table[i].speedx)) / 2;
                if(sprite_table[n].x > sprite_table[i].x){
                  sprite_table[n].speedx = newspeed;
                  sprite_table[i].speedx = -newspeed;
                }
                else{
                  sprite_table[n].speedx = -newspeed;
                  sprite_table[i].speedx = newspeed;
                }
                sprite_table[n].x -= 2;
              }
              if((sprite_table[n].speedy >= 0 && sprite_table[i].speedy <= 0) || (sprite_table[n].speedy <= 0 && sprite_table[i].speedy >= 0)){
                newspeed = (abs(sprite_table[n].speedy) + abs(sprite_table[i].speedy)) / 2;
                if(sprite_table[n].y > sprite_table[i].y){
                  sprite_table[n].speedy = newspeed;
                  sprite_table[i].speedy = -newspeed;
                }
                else{
                  sprite_table[n].speedy = -newspeed;
                  sprite_table[i].speedy = newspeed;
                }
                sprite_table[n].y -=  2;
              }
            }
          }
        }
      }
      if(sprite_table[n].solid != 0 && tile.adr > 0){
          x0 = ((sprite_table[n].x + sprite_table[n].width / 2 - tile.x) / (int16_t)tile.imgwidth);
          y0 = ((sprite_table[n].y + sprite_table[n].height / 2 - tile.y + tile.imgheight) / (int16_t)tile.imgheight) - 1;
          if(x0 >= -1 && x0 <= tile.width && y0 >= -1 && y0 <= tile.height){
            if(getTail(x0, y0) != 0){
              if(sprite_table[n].speedx != 0){
                if(sprite_table[n].speedx > 0){
                  sprite_table[n].x = tile.x + x0 * tile.imgwidth - sprite_table[n].width ;
                  sprite_table[n].speedx /= 2;
                }
                else{
                  sprite_table[n].x = tile.x + (x0 + 1) * tile.imgwidth;
                  sprite_table[n].speedx /= 2;
                }
              }
              if(sprite_table[n].speedy != 0){
                if(sprite_table[n].speedy > 0){
                  sprite_table[n].y = tile.y + y0 * tile.imgheight - sprite_table[n].height ;
                  sprite_table[n].speedy /= 2;
                }
                else{
                  sprite_table[n].y = tile.y + (y0 + 1) * tile.imgheight;
                  sprite_table[n].speedy /= 2;
                }
              }
            }
            else{
              if(sprite_table[n].speedy > 0 && getTail(x0, y0 + 1) != 0){
                if((tile.y + (y0 + 1) * tile.imgheight) - (sprite_table[n].y  + sprite_table[n].height) < sprite_table[n].speedy * 2){
                  sprite_table[n].y = tile.y + (y0 + 1) * tile.imgheight - sprite_table[n].height;  
                  sprite_table[n].speedy = 0;
                }
              }
              else if(sprite_table[n].speedy < 0 && getTail(x0, y0 - 1) != 0){
                if(sprite_table[n].y - (tile.y + y0 * tile.imgheight) < sprite_table[n].speedy * 2){
                  sprite_table[n].y = tile.y + y0 * tile.imgheight;  
                  sprite_table[n].speedy = 0;
                }
              }
              if(sprite_table[n].speedx > 0  && getTail(x0 + 1, y0) != 0){
                if((tile.x + (x0 + 1) * tile.imgwidth - sprite_table[n].width) - sprite_table[n].x < sprite_table[n].speedx * 2){
                  sprite_table[n].x = tile.x + (x0 + 1) * tile.imgwidth - sprite_table[n].width;  
                  sprite_table[n].speedx = 0;
                }
              }
              else if(sprite_table[n].speedx < 0 && getTail(x0 - 1, y0) != 0){
                if(sprite_table[n].x - (tile.x + x0 * tile.imgwidth) < sprite_table[n].speedx * 2){
                  sprite_table[n].x = tile.x + x0 * tile.imgwidth; 
                  sprite_table[n].speedx = 0;
                }
              } 
            } 
          }
        }
        
    }
  }
}

void clearSpriteScr(){
  for(byte y = 0; y < 128; y ++)
    for(byte x = 0; x < 64; x++){
      if(sprite_screen[x][y] > 0)
        line_is_draw[y] |= 1 + x / 32;
      sprite_screen[x][y] = 0;
    }
}

void clearScr(uint8_t color){
  for(byte y = 0; y < 128; y ++){
    //line_is_draw[y] = 3;
    for(byte x = 0; x < 128; x++)
      setPix(x, y, color);
      //screen[x][y] = color + (color << 4);
  }
}

void setImageSize(uint8_t size){
  imageSize = size;
}

void setSpr(uint16_t n, uint16_t adr){
  sprite_table[n].address = adr;
}

void setSprPosition(int8_t n, uint16_t x, uint16_t y){
  sprite_table[n].x = x;
  sprite_table[n].y = y;
}

void spriteSetDirectionAndSpeed(int8_t n, uint16_t speed, int16_t dir){
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
      return sprite_table[n].x;
    case 1:
      return sprite_table[n].y;
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
      return sprite_table[n].solid;
    case 10:
      return sprite_table[n].gravity;
  }
  return 0;
}

void setSpriteValue(int8_t n, uint8_t t, int16_t v){
  if(t == 0)
    sprite_table[n].x = v;
  else if(t == 1)
    sprite_table[n].y = v;
  else if(t == 2){
    sprite_table[n].speedx = (int8_t) v;
  }
  else if(t == 3){
    sprite_table[n].speedy = (int8_t) v;
  }
  else if(t == 4)
    sprite_table[n].width = v;
  else if(t == 5)
    sprite_table[n].height = v;
  else if(t == 6)
    sprite_table[n].angle = (v % 360) & 0x01ff;
  else if(t == 7)
    sprite_table[n].lives = v;
  else if(t == 9)
    sprite_table[n].solid = v;
  else if(t == 10)
    sprite_table[n].gravity = v;
  else if(t == 11)
    sprite_table[n].oncollision = (uint16_t)v;
  else if(t == 12)
    sprite_table[n].onexitscreen = (uint16_t)v;
}

void drawRotateSprPixel(int8_t pixel, int8_t x0, int8_t y0, int16_t x, int16_t y, int16_t hw, int16_t hh, int16_t c, int16_t s){
  //int16_t nx = ((x * c - y * s) >> 6);
  //int16_t ny = ((y * c  + x * s) >> 6);
  int16_t nx = hw + (((x - hw) * c - (y - hh) * s) >> 6);
  int16_t ny = hh + (((y - hh) * c + (x - hw) * s) >> 6);
  int16_t nnx = nx / 2;
  int8_t nnx0 = x0 / 2;
  if(nnx0 + nnx >= 0 && nnx0 + nnx < 64 && y0 + ny >= 0 && y0 + ny < 128){
    if((x0 + nx) & 1)
      sprite_screen[nnx0 + nnx][y0 + ny] = (sprite_screen[nnx0 + nnx][y0 + ny] & 0x0f) + (pixel << 4);
    else
      sprite_screen[nnx0 + nnx][y0 + ny] = (sprite_screen[nnx0 + nnx][y0 + ny] & 0xf0) + pixel;
    line_is_draw[y0 + ny] |= 1 + (nnx0 + nnx) / 32;
  }
}

inline void drawSprPixel(int8_t pixel, int8_t x0, int8_t y0, int16_t x, int16_t y){
  if(x0 + x >= 0 && x0 + x < 128 && y0 + y >= 0 && y0 + y < 128){
    if((x0 + x) & 1)
      sprite_screen[(x0 + x) / 2][y0 + y] = (sprite_screen[(x0 + x) / 2][y0 + y] & 0xf0) + pixel;
    else
      sprite_screen[(x0 + x) / 2][y0 + y] = (sprite_screen[(x0 + x) / 2][y0 + y] & 0x0f) + (pixel << 4);
    line_is_draw[y0 + y] |= 1 + (x0 + x) / 64;
  }
}

void drawSpr(int8_t n, uint16_t x, uint16_t y){
  uint16_t adr = sprite_table[n].address;
  uint8_t w = sprite_table[n].width;
  uint8_t h = sprite_table[n].height;
  int16_t c = getCos(sprite_table[n].angle);
  int16_t s = getSin(sprite_table[n].angle);
  uint8_t pixel;
  w = w / 2;
  if(sprite_table[n].angle == 0){
    for(byte y1 = 0; y1 < h; y1 ++)
      if(y1 + y >= -h && y1 + y < 128 + h){
        for(byte x1 = 0; x1 < w; x1++){
          pixel = readMem(adr + x1 + y1 * w);
          if((pixel & 0xf0) > 0)
            drawSprPixel(pixel >> 4, x, y, x1 * 2, y1);
          if((pixel & 0x0f) > 0)
            drawSprPixel(pixel & 0xf, x, y, x1 * 2 + 1, y1);
        }
      }
  }
  else{
    for(byte y1 = 0; y1 < h; y1 ++)
      if(y1 + y >= -h && y1 + y < 128 + h){
        for(byte x1 = 0; x1 < w; x1++)
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

void drawImg(int16_t a, uint16_t x, uint16_t y, int16_t w, int16_t h){
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

void drawImgRLE(int16_t adr, uint16_t x1, uint16_t y1, int16_t w, int16_t h){
    if(imageSize > 1){
      drawImgRLES(adr, x1, y1, w, h);
      return;
    }
    int16_t i = 0;
    byte repeat = readMem(adr);
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

void drawImgS(int16_t a, uint16_t x, uint16_t y, int16_t w, int16_t h){
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

void drawImgRLES(int16_t adr, uint16_t x1, uint16_t y1, int16_t w, int16_t h){
    int16_t i = 0;
    uint8_t jx, jy, s;
    s = imageSize;
    byte repeat = readMem(adr);
    adr++;
    int8_t color1 = (readMem(adr) & 0xf0) >> 4;
    int8_t color2 = readMem(adr) & 0xf;
    while(i < w * h){
      if(repeat > 0x81){
        if(color1 > 0){
          for(jx = 0; jx < s; jx++)
            for(jy = 0; jy < s; jy++)
              setPix(x1 + (i % w) * s + jx, y1 + i / w * s + jy, color1);
        }
        if(color2 > 0){
          for(jx = 0; jx < s; jx++)
            for(jy = 0; jy < s; jy++)
              setPix(x1 + (i % w) * s + s + jx, y1 + i / w * s + jy, color2);
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
          for(jx = 0; jx < s; jx++)
                for(jy = 0; jy < s; jy++)
                  setPix(x1 + (i % w) * s + jx, y1 + i / w * s + jy, color1);
        }
        if(color2 > 0){
          for(jx = 0; jx < s; jx++)
                for(jy = 0; jy < s; jy++)
                  setPix(x1 + (i % w) * s + s + jx, y1 + i / w * s + jy, color2);;
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

void loadTile(int16_t adr, uint8_t iwidth, uint8_t iheight, uint8_t width, uint8_t height){
    tile.adr = adr;
    tile.imgwidth = iwidth;
    tile.imgheight = iheight;
    tile.width = width;
    tile.height = height;
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
        if(nx >= 0 && nx < 128 && ny >= 0 && ny < 128){
          imgadr = readInt(tile.adr + (x + y * tile.width) * 2);
          if(imgadr > 0)
            drawImg(imgadr, nx, ny, tile.imgwidth, tile.imgheight); 
        }
      }
    }
  }

void drawFVLine(int16_t x, int16_t y1, int16_t y2){
  for(int16_t  i = y1; i <= y2; i++)
    setPix(x, i, color);
}

void drawFHLine(int16_t x1, int16_t x2, int16_t y){
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

inline void setPix(byte x, byte y, byte c){
  byte xi = x / 2;
  uint8_t b;
  if(x >= 0 && x < 128 && y >= 0 && y < 128){
    b = screen[xi][y];
    if(x & 1)
      screen[xi][y] = (screen[xi][y] & 0xf0) + c;
    else
      screen[xi][y] = (screen[xi][y] & 0x0f) + ( c << 4);
    if(b != screen[xi][y])
      line_is_draw[y] |= 1 + x / 64;
  }
}

byte getPix(byte x, byte y){
  byte b = 0;
  byte xi = x / 2;
  if(x >= 0 && x < 128 && y >= 0 && y < 128){
    if(x % 2 == 0)
      b = (screen[xi][y] & 0xf0) >> 4;
    else
      b = (screen[xi][y] & 0x0f);
  }
  return b;
}

void changePalette(uint8_t n, uint16_t c){
  palette[n] = c;
}

void scrollScreen(uint8_t step, uint8_t direction){
    uint8_t bufPixel;
    if(direction == 2){
      for(uint8_t y = 0; y < 128; y++){
        bufPixel = screen[0][ y];
        for(uint8_t x = 1; x < 64; x++){
          if(screen[x - 1][y] != screen[x][y])
            line_is_draw[y] |= 1 + x / 32;
          screen[x - 1][ y] = screen[x][y];
        }
        if(screen[63][y] != bufPixel)
            line_is_draw[y] |= 1;
        screen[63][ y] = bufPixel;
      }
      for(uint8_t n = 0; n < 32; n++)
        sprite_table[n].x-=2;
    }
    else if(direction == 1){
      for(uint8_t x = 0; x < 64; x++){
        bufPixel = screen[x][0];
        for(uint8_t y = 1; y < 128; y++){
          if(screen[x][y-1] != screen[x][y])
            line_is_draw[y] |= 1 + x / 32;
          screen[x][y - 1] = screen[x][y];
        }
        if(screen[x][127] != bufPixel)
            line_is_draw[127] |= 2;
        screen[x][127] = bufPixel;
      }
      for(uint8_t n = 0; n < 32; n++)
        sprite_table[n].y--;
    }
    else if(direction == 0){
      for(uint8_t y = 0; y < 128; y++){
        bufPixel = screen[63][y];
        for(uint8_t x = 63; x > 0; x--){
          if(screen[x][y] != screen[x - 1][y])
            line_is_draw[y] |= 1 + x / 32;
          screen[x][y] = screen[x - 1][y];
        }
        if(screen[0][y] != bufPixel)
            line_is_draw[y] |= 1;
        screen[0][y] = bufPixel;
      }
      for(uint8_t n = 0; n < 32; n++)
        sprite_table[n].x+=2;
    }
    else {
      for(uint8_t x = 0; x < 64; x++){
        bufPixel = screen[x][127];
        for(uint8_t y = 127; y > 0; y--){
          if(screen[x][y] != screen[x][y - 1])
            line_is_draw[y] |= 1 + x / 32;
          screen[x][y] = screen[x][y - 1];
        }
        if(screen[x][0] != bufPixel)
            line_is_draw[0] |= 1 + x / 32;
        screen[x][0] = bufPixel;
      }
      for(uint8_t n = 0; n < 32; n++)
        sprite_table[n].y++;
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
      if(x < tile.width && x >= 0){
        for(y = 0; y < tile.height; y++){
          ny = y0 + y * tile.imgheight;
          if(ny > 0 && ny < 128){
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
      if(y < tile.height  && y >= 0)
        for(x = 0; x < tile.width; x++){
          nx = x0 + x * tile.imgwidth;
          if(nx > 0 && nx < 128){
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
      if(x0 < 0 && x >= 0){
        for(y = 0; y < tile.height; y++){
          ny = y0 + y * tile.imgheight;
          if(ny > 0 && ny < 128){
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
      if(y < tile.height  && y >= 0)
        for(x = 0; x < tile.width; x++){
          if(nx > 0 && nx < 128){
            imgadr = readInt(tile.adr + (x + y * tile.width) * 2);
            if(imgadr > 0)
              drawImg(imgadr, nx, ny, tile.imgwidth, tile.imgheight); 
            else
              fillRect(nx, ny, tile.imgwidth, tile.imgheight, bgcolor);
          }
        }
    }
  }

void charLineUp(byte n){
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

void printc(char c, byte fc, byte bc){
  if(c == '\n'){
    fillRect(regx * 6, regy * 8, 127 - regx * 6, 8, bgcolor);
    for(byte i = regx; i <= 21; i++){
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
    for(byte i = 0; i <= regx % 5; i++){
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

void setColor(uint8_t c){
  color = c & 0xf;
}

void fillRect(int8_t x, int8_t y, uint8_t w, uint8_t h, uint8_t c){
   for(int16_t jx = x; jx < x + w; jx++)
     for(int16_t jy = y; jy < y + h; jy++)
      setPix(jx, jy, c);
}

void putString(char s[], int8_t y){
  int8_t i = 0;
  while(s[i] != 0 && i < 21){
    putchar(s[i], i * 6, y);
    i++;
  }
}

void putchar(char c, uint8_t x, uint8_t y) {
    for(int8_t i=0; i<5; i++ ) { // Char bitmap = 5 columns
      uint8_t line = pgm_read_byte(&font_a[c * 5 + i]);
      for(int8_t j=0; j<8; j++, line >>= 1) {
        if(line & 1)
         setPix(x+i, y+j, color);
      }
  }
}

