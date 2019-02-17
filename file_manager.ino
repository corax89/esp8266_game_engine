void fileList(String path) {
  Dir dir = SPIFFS.openDir(path);
  char s[32];
  char thisF[32];
  int16_t lst = 1;
  int16_t pos = 0;
  int16_t startpos = 0;
  int16_t fileCount = 0;
  int16_t skip = 0;
  while (dir.next()) {
    File entry = dir.openFile("r");
    strcpy(s, entry.name());
    Serial.println(s);
    entry.close();
    fileCount++;
  }
  while(1){
    skip = startpos;
    lst = 1;
    dir = SPIFFS.openDir(path);
    setColor(1);
    while (dir.next() && lst < 14) {
      File entry = dir.openFile("r");
      if(skip > 0){
        skip--;
      }
      else{
        strcpy(s, entry.name());
        if(lst + startpos - 1 == pos)
          strcpy(thisF, entry.name());
        putString(s, lst * 8);
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
    setColor(8);
    drwLine(2, (pos - startpos + 1) * 8, 124,  (pos - startpos + 1) * 8);
    drwLine(2, (pos - startpos + 1) * 8 + 7, 124,  (pos - startpos + 1) * 8 + 7);
    redrawScreen();
    clearScr();
    while(thiskey != 0){   
      getKey();
      delay(100);
    }
    while(thiskey == 0){   
      getKey();
      delay(100);
    }
    if(thiskey & 16){//ok
      loadFromSPIFS(thisF);
      return;
    }
    else if(thiskey & 2){//down
      if(pos < fileCount - 1)
        pos++;
    }
    else if(thiskey & 1){//up
      if(pos > 0)
        pos--;
    }
    if(thiskey & 4)//left
      return;
  }
}

void loadFromSPIFS(char fileName[]){
  int i;
  File f = SPIFFS.open(fileName, "r");
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

