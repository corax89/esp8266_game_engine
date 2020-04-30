#include <stdint.h>
#include "settings.h"

uint8_t *lge_mem __attribute__ ((aligned));

void memoryAlloc(){
  lge_mem = (uint8_t*)malloc(RAM_SIZE * sizeof(uint8_t));
  if(lge_mem == NULL)
    Serial.println(F("Out of memory"));
  screenMemoryAlloc();
}

void memoryFree(){
  screenMemoryFree();
  free(lge_mem);
}

inline void writeMem(uint16_t adr, int16_t n){
  if(adr < RAM_SIZE)
    lge_mem[adr] = n;
}

inline uint8_t readMem(uint16_t adr){
  return (adr < RAM_SIZE) ? lge_mem[adr] : 0;
}

inline void writeInt(uint16_t adr, int16_t n){
  int8_t *nPtr;
  nPtr = (int8_t*)&n;
  writeMem(adr, *nPtr);
  nPtr++;
  adr++;
  writeMem(adr, *nPtr);
}

inline int16_t readInt(uint16_t adr){
  int16_t n;
  int8_t *nPtr;
  nPtr = (int8_t*)&n;
  *nPtr = readMem(adr);
  nPtr++;
  adr++;
  *nPtr = readMem(adr);
  return n;
}
