#include "settings.h"
#define NEXT_CHAR rtttl.startposition++; c = (char)readMem(rtttl.startposition); if(c == 0) return 0;
#define NEXT_CHAR_IN_P rtttl.position++; c = (char)readMem(rtttl.startposition + rtttl.position); if(c == 0) return 50;
#define OCTAVE_OFFSET 0

int notes[] = { 0,
  262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
  523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,
  1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
  2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951
};

unsigned int millisec = (unsigned int)millis();

struct RTTTL {
  uint16_t address;
  uint16_t position;
  uint16_t startposition;
  uint8_t loop;
  uint8_t play;
  uint8_t default_dur;
  uint8_t default_oct;
  uint16_t bpm;
  uint32_t wholenote;
  uint16_t this_tone;
  int16_t delay;
  uint8_t isPlayed;
};

struct PLAY_TONE {
  uint16_t freq;
  int16_t time;
};

struct RTTTL rtttl;
struct PLAY_TONE play_tone;

inline void addTone(uint16_t f, uint16_t t){
  play_tone.freq = f;
  play_tone.time = t;
}

uint8_t loadRtttl(){
  uint16_t num;
  char c;
  rtttl.default_dur = 4;
  rtttl.default_oct = 6;
  rtttl.bpm = 63;
  rtttl.startposition = rtttl.address;
  c = readMem(rtttl.startposition);
  while(c != ':'){
    // ignore name
    NEXT_CHAR
  }
  NEXT_CHAR                     // skip ':'
  // get default duration
  if(c == 'd'){
    NEXT_CHAR
    NEXT_CHAR// skip "d="
    num = 0;
    while(isdigit(c)){
      num = (num * 10) + (c - '0');
      NEXT_CHAR
    }
    if(num > 0) 
      rtttl.default_dur = num;
    NEXT_CHAR                   // skip comma
  }
  // get default octave
  if(c == 'o'){
    NEXT_CHAR
    NEXT_CHAR// skip "o="
    num = c - '0';
    NEXT_CHAR
    if(num >= 3 && num <=7) 
      rtttl.default_oct = num;
    NEXT_CHAR                   // skip comma
  }
  // get BPM
  if(c == 'b'){
    NEXT_CHAR
    NEXT_CHAR// skip "b="
    num = 0;
    while(isdigit(c)){
      num = (num * 10) + (c - '0');
      NEXT_CHAR
    }
    rtttl.bpm = num;
  }
  rtttl.wholenote = (60 * 1000 / rtttl.bpm) * 4;
  NEXT_CHAR
  rtttl.position = 0;
  rtttl.delay = 0;
  return 1;
}

void setRtttlAddress(uint16_t adr){
  rtttl.address = adr;
  loadRtttl();
}

void setRtttlLoop(uint8_t loop){
  rtttl.loop = loop;
}

void setRtttlPlay(uint8_t play){
  if(play == 0){
    rtttl.play = 0;
    noTone(SOUNDPIN);
  }
  else if(play == 1){
    rtttl.play = 1;
  }
  else{
    rtttl.play = 0;
    rtttl.position = 0;
    noTone(SOUNDPIN);
  }
  rtttl.isPlayed = 0;
  rtttl.delay = 0;
}

inline void updateRtttl(){
  if(rtttl.delay > 0)
    rtttl.delay--;
  if(play_tone.time > 0)
    play_tone.time--;
  //play single tone
  if(play_tone.time > 0){
    if(rtttl.delay <= 0){
      tone(SOUNDPIN, play_tone.freq, 128);
      rtttl.isPlayed = 0;
      return;
    }
    if(play_tone.time & 1){
      tone(SOUNDPIN, play_tone.freq, 128);
      rtttl.isPlayed = 0;
      return;
    }
  }
  //player
  if(rtttl.play == 0){
    return;
  }
  if(rtttl.delay > 0){
    if(!rtttl.isPlayed){
      rtttl.isPlayed = 1;
      tone(SOUNDPIN, rtttl.this_tone, rtttl.delay);
    }
    return;
  }
}

int playRtttl(){
  uint16_t num;
  uint32_t duration;
  uint8_t note;
  uint8_t scale;
  char c;
  //first, get note duration, if available
  noTone(SOUNDPIN);
  if(rtttl.play == 0 || rtttl.startposition == 0){
    return 50;
  }
  num = 0;
  c = (char)readMem(rtttl.startposition + rtttl.position);
  if(c == 0){
    if(!rtttl.loop){
      rtttl.play = 0;
      rtttl.isPlayed = 0;
      rtttl.this_tone = 0;
      rtttl.delay = 0;
    }
    rtttl.position = 0;
    c = (char)readMem(rtttl.startposition + rtttl.position);
  }
  while(isdigit(c)){
    num = (num * 10) + (c - '0');
    NEXT_CHAR_IN_P
  }
  if(num) 
    duration = rtttl.wholenote / num;
  else 
    duration = rtttl.wholenote / rtttl.default_dur;  // we will need to check if we are a dotted note after
  //now get the note
  note = 0;
  switch(c){
    case 'c':
    case 'C':
      note = 1;
      break;
    case 'd':
    case 'D':
      note = 3;
      break;
    case 'e':
    case 'E':
      note = 5;
      break;
    case 'f':
    case 'F':
      note = 6;
      break;
    case 'g':
    case 'G':
      note = 8;
      break;
    case 'a':
    case 'A':
      note = 10;
      break;
    case 'b':
    case 'B':
      note = 12;
      break;
    case 'p':
    case 'P':
    default:
      note = 0;
  }
  NEXT_CHAR_IN_P
  // now, get optional '#' sharp
  if(c == '#'){
    note++;
    NEXT_CHAR_IN_P
  }
  // now, get optional '.' dotted note
  if(c == '.'){
    duration += duration/2;
    NEXT_CHAR_IN_P
  }
  // now, get scale
  if(isdigit(c)){
    scale = c - '0';
    NEXT_CHAR_IN_P
  }
  else{
    scale = rtttl.default_oct;
  }
  scale += OCTAVE_OFFSET;
  if(c == ',')
    NEXT_CHAR_IN_P       // skip comma for next note (or we may be at the end)
  // now play the note
  rtttl.delay = duration;
  if(note){
    rtttl.this_tone = notes[note];
    tone(SOUNDPIN, rtttl.this_tone, rtttl.delay);
  }
  else{
    rtttl.this_tone = 0;
  }
  rtttl.isPlayed = 1;
  return duration;
}
