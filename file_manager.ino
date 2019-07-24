static const uint8_t iconBin[] PROGMEM = {
  0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x1c,0xcc,0xcc,0xcc,0xcc,0xcc,
  0xcc,0x11,0x11,0x11,0x11,0x11,0x1c,0x11,0x11,0x11,0x11,0x11,0x1c,0x11,0x11,0x11,0x11,0x11,0x1c,0x11,
  0x11,0x11,0x11,0x11,0x1c,0x11,0x11,0x11,0x11,0x11,0x1c,0x11,0x11,0x11,0x11,0x11,0x1c,0x11,0x11,0x11,
  0x11,0x11,0x1c,0x1c,0xc1,0x1c,0x1c,0xc1,0x1c,0x11,0x11,0x11,0x11,0x11,0x1c,0x1c,0x1c,0x11,0x1c,0x1c,
  0x1c,0x11,0x11,0x11,0x11,0x11,0x1c,0x1c,0xc1,0x1c,0x1c,0x1c,0x1c,0x11,0x11,0x11,0x11,0x11,0x1c,0x1c,
  0x1c,0x1c,0x1c,0x1c,0x1c,0x11,0x11,0x11,0x11,0x11,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x11,0x11,0x11,
  0x11,0x11,0x1c,0x1c,0xc1,0x1c,0x1c,0x1c,0x1c,0x11,0x11,0x11,0x11,0x11,0x1c,0x11,0x11,0x11,0x11,0x11,
  0x1c,0x11,0x11,0x11,0x11,0x11,0x1c,0x11,0x11,0x11,0x11,0x11,0x1c,0x11,0x11,0x11,0x11,0x11,0x1c,0x11,
  0x11,0x11,0x11,0x11,0x1c,0x11,0x11,0x11,0x11,0x11,0x1c,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0x11,0x11,0x11,
  0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11
};

void fileList(String path) {
  fs::Dir dir = SPIFFS.openDir(path);
  char s[32];
  char thisF[32];
  int16_t lst = 1;
  int16_t pos = 0;
  int16_t startpos = 0;
  int16_t fileCount = 0;
  int16_t skip = 0;
  uint8_t i;
  for(i = 0; i < 192; i++)
    mem[i + 1024 + 192] = pgm_read_byte_near(iconBin + i);
  setImageSize(1);
  while (dir.next()) {
    fs::File entry = dir.openFile("r");
    entry.close();
    fileCount++;
  }
  Serial.print(F("find "));
  Serial.print(fileCount);
  Serial.println(F(" files"));
  while(1){
    skip = startpos;
    lst = 1;
    dir = SPIFFS.openDir(path);
    setColor(8);
    for(i = 1; i < 17; i++)
      drwLine(2, (pos - startpos) * 17 + i, 124,  (pos - startpos) * 17 + i);
    setColor(1);
    while (dir.next() && lst < 8) {
      fs::File entry = dir.openFile("r");
      if(skip > 0){
        skip--;
      }
      else{
        strcpy(s, entry.name());
        if(lst + startpos - 1 == pos)
          strcpy(thisF, entry.name());
        putString(s, lst * 17 - 16);
        i = 0;
        while(i < 28 && s[i] != '.')
          i++;
        i++;
        if(s[i] == 'l' && s[i + 1] == 'g' && s[i + 2] == 'e'){
          entry.seek(5, SeekSet);
          for(i = 0; i < 192; i++)
            if(entry.available())
              mem[i + 1024] = (uint8_t)entry.read(); 
          drawImg(1024, 0, lst * 17 - 16, 24, 16);
        }
        else if(s[i] == 'b' && s[i + 1] == 'i' && s[i + 2] == 'n')
          drawImg(1024 + 192, 0, lst * 17 - 16, 24, 16);
        lst++;
      }
      entry.close();   
    }
    if(lst + startpos - 1 < pos){
      if(fileCount > pos)
        startpos++;
      else
        pos--;
    }
    else if(startpos > pos){
      startpos = pos;
    }
    redrawScreen();
    clearScr(0);  
    getKey();
    delay(200);
    while(thiskey == 0){   
      getKey();
      delay(100);
      changeSettings();
      if(fileIsLoad)
        return;
    }
    if(thiskey & 16){//ok
      cpuInit();
      i = 0;
      while(i < 28 && thisF[i] != '.')
        i++;
      i++;
      if(thisF[i] == 'b' && thisF[i + 1] == 'i' && thisF[i + 2] == 'n')
        loadBinFromSPIFS(thisF);
      else if(thisF[i] == 'l' && thisF[i + 1] == 'g' && thisF[i + 2] == 'e')
        loadLgeFromSPIFS(thisF);
      return;
    }
    else if(thiskey & 2){//down
      if(pos < fileCount - 1)
        pos++;
      if(pos - startpos > 5)
        startpos++;
    }
    else if(thiskey & 1){//up
      if(pos > 0)
        pos--;
      if(pos - startpos < 0)
        startpos--;
    }
    if(thiskey & 4){//left
      cpuInit();
      return;
    }
  }
}

void loadBinFromSPIFS(char fileName[]){
  int i;
  fs::File f = SPIFFS.open(fileName, "r");
  if(f.size() < RAM_SIZE)
    for(i = 0; i < f.size(); i++){
      mem[i] = (uint8_t)f.read();
    }
  Serial.print(F("loaded "));
  Serial.print(i);
  Serial.println(F(" byte"));
  Serial.print(F("free heap "));
  Serial.println(system_get_free_heap_size());
  f.close();  //Close file
}

void loadLgeFromSPIFS(char fileName[]){
  int n,j = 0;
  uint8_t b,l;
  int16_t length, position, point;
  fs::File f = SPIFFS.open(fileName, "r");
  if((char)f.read() == 'l' && (char)f.read() == 'g' && (char)f.read() == 'e'){
    l = (uint8_t)f.read();
  }
  else
    return;
  n = (uint8_t)f.read();
  Serial.print(F("offset "));
  Serial.print(n);
  f.seek(n, SeekSet);
  n = 0;
  while(f.available()){
    b = (uint8_t)f.read();
    if((b & 128) == 0){
      length = ((b & 127) << 8) + (uint8_t)f.read();
      for( j = 0; j < length; j ++){
        if(n < RAM_SIZE)
          mem[n] = (uint8_t)f.read();
        n++;
      }
    }
    else{
      length = (b & 127) >> 1;
      position = (((b & 1) << 8) + (uint8_t)f.read());
      point = n - position;
      for( j = 0; j < length; j ++){
        if(n < RAM_SIZE && point + j < RAM_SIZE)
          mem[n] = mem[point + j];
        n++;
      }
    }
  }
  Serial.print(F("loaded "));
  Serial.print(n);
  Serial.println(F(" byte"));
  Serial.print(F("free heap "));
  Serial.println(system_get_free_heap_size());
  f.close();  //Close file
}


