int16_t serialBegin(){
  serial_used = 1;
  return 1;
}

int16_t serialAvailable(){
  if(Serial.available() && serial_used)
    return 1;
  return 0;
}

int16_t serialRead(){
  if(Serial.available() && serial_used)
    return Serial.read();
  return 0;
}

int16_t serialWrite(int16_t n){
  Serial.write((char) n);
  return 1;
}
