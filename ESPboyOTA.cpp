/*
ESPboyOTA class -- ESPboy App Store client core
for www.ESPboy.com project by RomanS
https://hackaday.io/project/164830-espboy-games-iot-stem-for-education-fun
thanks to DmitryL (Plague) for help, tests and advices
*/

#include "ESPboyOTA.h"
#define SOUNDPIN D3

const uint8_t ESPboyOTA::keybOnscr[2][3][21] PROGMEM = {
 {"+1234567890abcdefghi", "jklmnopqrstuvwxyz -=", "?!@$%&*()_[]\":;.,^<E",},
 {"+1234567890ABCDEFGHI", "JKLMNOPQRSTUVWXYZ -=", "?!@$%&*()_[]\":;.,^<E",}
};

const char PROGMEM* ESPboyOTA::hostD = "script.google.com";
const char PROGMEM* ESPboyOTA::urlPost = "/macros/s/AKfycbxIfj1Eqi1eupe9Vmkhk0liuNrhSvM1Sx65qxocbBsd4jl0e7yj/exec";
const uint16_t PROGMEM ESPboyOTA::httpsPort = 443;

String *ESPboyOTA::consoleStrings;
uint16_t *ESPboyOTA::consoleStringsColor;


ESPboyOTA::ESPboyOTA(TFT_eSPI* tftOTA, Adafruit_MCP23017* mcpOTA) {
   consoleStrings = new String[OTA_MAX_CONSOLE_STRINGS+1];
   consoleStringsColor = new uint16_t [OTA_MAX_CONSOLE_STRINGS+1];
   keybParam.renderLine = 0;
   keybParam.displayMode = 0;
   keybParam.shiftOn = 0;
   keybParam.selX = 0;
   keybParam.selY = 0;
   keybParam.typing = "";
   wificl.ssid = "1";
   wificl.pass = "1";
   
   tft = tftOTA;
   mcp = mcpOTA;
   toggleDisplayMode(1);
   checkOTA();
}

uint8_t ESPboyOTA::keysAction() {
  uint8_t longActPress = 0;
  uint8_t keyState = getKeys();

  if (keyState) {
    tone(SOUNDPIN, 100, 10);
    if (!keybParam.displayMode) {
      if (keyState & OTA_PAD_LEFT && keyState & OTA_PAD_UP) {  // shift
        keybParam.shiftOn = !keybParam.shiftOn;
        drawKeyboard(keybParam.selX, keybParam.selY, 0);
        waitKeyUnpressed();
      } else {
        if ((keyState & OTA_PAD_RIGHT) && keybParam.selX < 20) keybParam.selX++;
        if ((keyState & OTA_PAD_LEFT) && keybParam.selX > -1) keybParam.selX--;
        if ((keyState & OTA_PAD_DOWN) && keybParam.selY < 3) keybParam.selY++;
        if ((keyState & OTA_PAD_UP) && keybParam.selY > -1) keybParam.selY--;
        if ((keyState & OTA_PAD_LEFT) && keybParam.selX == -1) keybParam.selX = 19;
        if ((keyState & OTA_PAD_RIGHT) && keybParam.selX == 20) keybParam.selX = 0;
        if ((keyState & OTA_PAD_UP) && keybParam.selY == -1) keybParam.selY = 2;
        if ((keyState & OTA_PAD_DOWN) && keybParam.selY == 3) keybParam.selY = 0;
      }

      if ((keyState & OTA_PAD_ACT && keyState & OTA_PAD_ESC) ||
          (keyState & OTA_PAD_RGT && keyState & OTA_PAD_LFT)) {
        if (keybParam.renderLine > OTA_MAX_CONSOLE_STRINGS - OTA_MAX_STRINGS_ONSCREEN_FULL)
          keybParam.renderLine = OTA_MAX_CONSOLE_STRINGS - OTA_MAX_STRINGS_ONSCREEN_FULL;
        toggleDisplayMode(1);
        waitKeyUnpressed();
      } else if (keyState & OTA_PAD_RGT && keybParam.renderLine) {
        keybParam.renderLine--;
        drawConsole(0);
      } else if (keyState & OTA_PAD_LFT &&
                 keybParam.renderLine <
                     OTA_MAX_CONSOLE_STRINGS - OTA_MAX_STRINGS_ONSCREEN_SMALL) {
        keybParam.renderLine++;
        drawConsole(0);
      }

      if ((((keyState & OTA_PAD_ACT) && (keybParam.selX == 19 && keybParam.selY == 2)) || (keyState & OTA_PAD_RGT && keyState & OTA_PAD_LFT))) {  // enter
        if (keybParam.typing.length() > 0) longActPress = 1;
      } else if ((keyState & OTA_PAD_ACT) && (keybParam.selX == 18 && keybParam.selY == 2)) {  // back space
        if (keybParam.typing.length() > 0) keybParam.typing.remove(keybParam.typing.length() - 1);
      } else if ((keyState & OTA_PAD_ACT) && (keybParam.selX == 17 && keybParam.selY == 1)) {  // SPACE
            if (keybParam.typing.length() < OTA_MAX_TYPING_CHARS) keybParam.typing += " ";
      } else if ((keyState & OTA_PAD_ACT) && (keybParam.selX == 17 && keybParam.selY == 2)) {
        keybParam.shiftOn = !keybParam.shiftOn;
        drawKeyboard(keybParam.selX, keybParam.selY, 0);
        waitKeyUnpressed();
      } else if (keyState & OTA_PAD_ACT){
        if (waitKeyUnpressed() > OTA_KEY_PRESSED_DELAY_TO_SEND)
          longActPress = 1;
        else if (keybParam.typing.length() < OTA_MAX_TYPING_CHARS)
          keybParam.typing += (char)pgm_read_byte(&keybOnscr[keybParam.shiftOn][keybParam.selY][keybParam.selX]);
      }

      if (keyState & OTA_PAD_ESC) {
        if (waitKeyUnpressed() > OTA_KEY_PRESSED_DELAY_TO_SEND)
          keybParam.typing = "";
        else if (keybParam.typing.length() > 0)
          keybParam.typing.remove(keybParam.typing.length() - 1);
      }
    }

    else {
      if ((keyState & OTA_PAD_ACT && keyState & OTA_PAD_ESC) ||
          (keyState & OTA_PAD_RGT && keyState & OTA_PAD_LFT)) {
        toggleDisplayMode(0);
        waitKeyUnpressed();
      } else

          if (((keyState & OTA_PAD_RGT || keyState & OTA_PAD_RIGHT ||
                keyState & OTA_PAD_DOWN)) &&
              keybParam.renderLine > 0) {
        keybParam.renderLine--;
        drawConsole(0);
      } else

          if (((keyState & OTA_PAD_LFT || keyState & OTA_PAD_LEFT ||
                keyState & OTA_PAD_UP)) &&
              keybParam.renderLine < OTA_MAX_CONSOLE_STRINGS - OTA_MAX_STRINGS_ONSCREEN_FULL) {
        keybParam.renderLine++;
        drawConsole(0);
      } else

          if (keyState & OTA_PAD_ESC)
        toggleDisplayMode(0);
    }
    if (!keybParam.displayMode) drawKeyboard(keybParam.selX, keybParam.selY, 1);
  }

  if (!keybParam.displayMode) drawBlinkingCursor();
  return (longActPress);
}

void ESPboyOTA::toggleDisplayMode(uint8_t mode) {
  keybParam.displayMode = mode;
  tft->fillScreen(TFT_BLACK);
  tft->drawRect(0, 0, 128, 128, TFT_NAVY);
  if (!keybParam.displayMode) {
    tft->drawRect(0, 128 - 3 * 8 - 5, 128, 3 * 8 + 5, TFT_NAVY);
    tft->drawRect(0, 0, 128, 10 * 8 + 5, TFT_NAVY);
  }
  if (!keybParam.displayMode) {
    drawKeyboard(keybParam.selX, keybParam.selY, 0);
  }
  drawConsole(0);
}

String ESPboyOTA::getUserInput() {
  String userInput;
  toggleDisplayMode(0);
  while (1) {
    while (!keysAction()) delay(OTA_KEYB_CALL_DELAY);
    if (keybParam.typing != "") break;
  }
  toggleDisplayMode(1);
  userInput = keybParam.typing;
  keybParam.typing = "";
  return (userInput);
}

void ESPboyOTA::printConsole(String bfrstr, uint16_t color, uint8_t ln, uint8_t noAddLine) {
  String toprint;

  keybParam.renderLine = 0;

  if (!ln)
    if (bfrstr.length() > 20) {
      bfrstr = bfrstr.substring(0, 20);
      toprint = bfrstr;
    }

  for (uint8_t i = 0; i <= (bfrstr.length() / 21); i++) {
    toprint = bfrstr.substring(i * 20);
    toprint = toprint.substring(0, 20);

    if (!noAddLine) {
      for (uint8_t j = 0; j < OTA_MAX_CONSOLE_STRINGS; j++) {
        consoleStrings[j] = consoleStrings[j + 1];
        consoleStringsColor[j] = consoleStringsColor[j + 1];
      }
    }

    consoleStrings[OTA_MAX_CONSOLE_STRINGS] = toprint;
    consoleStringsColor[OTA_MAX_CONSOLE_STRINGS] = color;
  }
  drawConsole(noAddLine);
}

void ESPboyOTA::drawConsole(uint8_t onlyLastLine) {
  uint8_t lines;

  if (keybParam.displayMode)
    lines = OTA_MAX_STRINGS_ONSCREEN_FULL;
  else
    lines = OTA_MAX_STRINGS_ONSCREEN_SMALL;

  if (!onlyLastLine)
    tft->fillRect(1, 1, 126, lines * 8 + 3, TFT_BLACK);
  else
    tft->fillRect(1, (lines - 1) * 8 + 3, 126, 8, TFT_BLACK);

  uint8_t offsetY = 3;
  if (!onlyLastLine) {
    for (uint8_t i = OTA_MAX_CONSOLE_STRINGS - lines - keybParam.renderLine + 1;
         i < OTA_MAX_CONSOLE_STRINGS - keybParam.renderLine + 1; i++) {
      tft->setTextColor(consoleStringsColor[i], TFT_BLACK);
      tft->drawString(consoleStrings[i], 4, offsetY);
      offsetY += 8;
    }
  } else {
    tft->setTextColor(consoleStringsColor[OTA_MAX_CONSOLE_STRINGS], TFT_BLACK);
    tft->drawString(consoleStrings[OTA_MAX_CONSOLE_STRINGS], 4, 8 * (lines - 1) + 3);
  }
}

uint8_t ESPboyOTA::getKeys() { return (~mcp->readGPIOAB() & 255); }

uint32_t ESPboyOTA::waitKeyUnpressed() {
  uint32_t timerStamp = millis();
  while (getKeys() && (millis() - timerStamp) < OTA_KEY_UNPRESSED_TIMEOUT) delay(1);
  return (millis() - timerStamp);
}

void ESPboyOTA::drawKeyboard(uint8_t slX, uint8_t slY, uint8_t onlySelected) {
  static char chr[2]={0,0};
  static uint8_t prevX = 0, prevY = 0;

  if (!onlySelected) {
    tft->fillRect(1, 128 - 24, 126, 23, TFT_BLACK);
    tft->setTextColor(TFT_YELLOW, TFT_BLACK);
    for (uint8_t j = 0; j < 3; j++)
      for (uint8_t i = 0; i < 20; i++) {
        chr[0] = pgm_read_byte(&keybOnscr[keybParam.shiftOn][j][i]);
        tft->drawString(&chr[0], i * 6 + 4, 128 - 2 - 8 * (3 - j));
      }
  }

  tft->setTextColor(TFT_YELLOW, TFT_BLACK);
  chr[0] = pgm_read_byte(&keybOnscr[keybParam.shiftOn][prevY][prevX]);
  tft->drawString(&chr[0], prevX * 6 + 4, 128 - 24 + prevY * 8 - 2);

  tft->setTextColor(TFT_WHITE, TFT_BLACK);
  tft->drawString("^<E", 6 * 17 + 4, 128 - 24 + 2 * 8 - 2);

  tft->setTextColor(TFT_YELLOW, TFT_RED);
  chr[0] = pgm_read_byte(&keybOnscr[keybParam.shiftOn][slY][slX]);
  tft->drawString(&chr[0], slX * 6 + 4, 128 - 24 + slY * 8 - 2);

  prevX = slX;
  prevY = slY;

  drawTyping(0);
}

void ESPboyOTA::drawTyping(uint8_t changeCursor) {
  static char cursorType[2] = {220, '_'};
  static uint8_t cursorTypeFlag=0;

  if(changeCursor) cursorTypeFlag=!cursorTypeFlag;
  tft->fillRect(1, 128 - 5 * 8, 126, 10, TFT_BLACK);
  if (keybParam.typing.length() < 20) {
    tft->setTextColor(TFT_WHITE, TFT_BLACK);
    tft->drawString(keybParam.typing + cursorType[cursorTypeFlag], 4, 128 - 5 * 8 + 1);
  } else {
    tft->setTextColor(TFT_WHITE, TFT_BLACK);
    tft->drawString("<" + keybParam.typing.substring(keybParam.typing.length() - 18) +cursorType[cursorTypeFlag], 4, 128 - 5 * 8 + 1);
  }
}

void ESPboyOTA::drawBlinkingCursor() {
 static uint32_t cursorBlinkMillis = 0; 
  if (millis() > (cursorBlinkMillis + OTA_CURSOR_BLINKING_PERIOD)) {
    cursorBlinkMillis = millis();
    drawTyping(1);
  }
}

uint16_t ESPboyOTA::scanWiFi() {
  printConsole(F("Scaning WiFi...\n"), TFT_MAGENTA, 1, 0);
  int16_t WifiQuantity = WiFi.scanNetworks();
  if (WifiQuantity != -1 && WifiQuantity != -2 && WifiQuantity != 0) {
    for (uint8_t i = 0; i < WifiQuantity; i++) wfList.push_back(wf());
    if (!WifiQuantity) {
      printConsole(F("WiFi not found"), TFT_RED, 1, 0);
      delay(3000);
      ESP.reset();
    } else
      for (uint8_t i = 0; i < wfList.size(); i++) {
        wfList[i].ssid = WiFi.SSID(i);
        wfList[i].rssi = WiFi.RSSI(i);
        wfList[i].encription = WiFi.encryptionType(i);
      }
    sort(wfList.begin(), wfList.end(), lessRssi());
    return (WifiQuantity);
  } else
    return (0);
}

String ESPboyOTA::getWiFiStatusName() {
  String stat;
  switch (WiFi.status()) {
    case WL_IDLE_STATUS:
      stat = (F("Idle"));
      break;
    case WL_NO_SSID_AVAIL:
      stat = (F("No SSID available"));
      break;
    case WL_SCAN_COMPLETED:
      stat = (F("Scan completed"));
      break;
    case WL_CONNECTED:
      stat = (F("WiFi connected"));
      break;
    case WL_CONNECT_FAILED:
      stat = (F("Wrong passphrase"));
      break;
    case WL_CONNECTION_LOST:
      stat = (F("Connection lost"));
      break;
    case WL_DISCONNECTED:
      stat = (F("Wrong password"));
      break;
    default:
      stat = (F("Unknown"));
      break;
  };
  return stat;
}

boolean ESPboyOTA::connectWifi() {
  uint16_t wifiNo = 0;
  uint32_t timeOutTimer;

  if (wificl.ssid == "1" && wificl.pass == "1" && !(getKeys()&OTA_PAD_ESC)) {
    wificl.ssid = WiFi.SSID();
    wificl.pass = WiFi.psk();
    printConsole(F("Last network:"), TFT_MAGENTA, 0, 0);
    printConsole(wificl.ssid, TFT_MAGENTA, 0, 0);
  } else {
    if (scanWiFi())
      for (uint8_t i = wfList.size(); i > 0; i--) {
        String toPrint =
            (String)(i) + " " + wfList[i - 1].ssid + " [" + wfList[i - 1].rssi +
            "]" + ((wfList[i - 1].encription == ENC_TYPE_NONE) ? "" : "*");
        printConsole(toPrint, TFT_GREEN, 0, 0);
      }

    while (!wifiNo) {
      printConsole(F("Choose WiFi No:"), TFT_MAGENTA, 0, 0);
      wifiNo = getUserInput().toInt();
      if (wifiNo < 1 || wifiNo > wfList.size()) wifiNo = 0;
    }

    wificl.ssid = wfList[wifiNo - 1].ssid;
    printConsole(wificl.ssid, TFT_YELLOW, 1, 0);

    while (!wificl.pass.length()) {
      printConsole(F("Password:"), TFT_MAGENTA, 0, 0);
      wificl.pass = getUserInput();
    }
    printConsole(/*pass*/F("******"), TFT_YELLOW, 0, 0);
  }

  wfList.clear();

  WiFi.mode(WIFI_STA);
  WiFi.begin(wificl.ssid, wificl.pass);

  printConsole(F("Connection..."), TFT_MAGENTA, 0, 0);
  timeOutTimer = millis();
  String dots = "";
  while (WiFi.status() != WL_CONNECTED &&
         (millis() - timeOutTimer < OTA_TIMEOUT_CONNECTION)) {
    delay(700);
    printConsole(dots, TFT_MAGENTA, 0, 1);
    dots += ".";
  }

  if (WiFi.status() != WL_CONNECTED) {
    wificl.ssid = "";
    wificl.pass = "";
    printConsole(getWiFiStatusName(), TFT_RED, 0, 1);
    return (false);
  } else {
    // Serial.println(WiFi.localIP());
    printConsole(getWiFiStatusName(), TFT_MAGENTA, 0, 1);
    return (true);
  }
}

void ESPboyOTA::OTAstarted() {
  printConsole(F("Starting download..."), TFT_MAGENTA, 0, 0);
  printConsole("", TFT_MAGENTA, 0, 0);
}

void ESPboyOTA::OTAfinished() {
  printConsole(F("Downloading OK"), TFT_GREEN, 0, 0);
  printConsole(F("Restarting..."), TFT_MAGENTA, 0, 0);
  printConsole(F("And then reset it again by yourself"), TFT_MAGENTA, 1, 0);
  ESP.reset();
}

void ESPboyOTA::OTAprogress(int cur, int total) {
  printConsole((String)(cur * 100 / total) + "%", TFT_GREEN, 0, 1);
}

void ESPboyOTA::OTAerror(int err) {
  // Serial.print(F("Error: ")); Serial.print(err);
  printConsole("Error: "+String(err), TFT_RED, 1, 0);
  printConsole(ESPhttpUpdate.getLastErrorString(), TFT_RED, 1, 0);
  delay(3000);
  ESP.reset();
}

void ESPboyOTA::updateOTA(String otaLink) {
  BearSSL::WiFiClientSecure updateClient;
  updateClient.setInsecure();
  // ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
  ESPhttpUpdate.onStart([this](){this->OTAstarted();});
  ESPhttpUpdate.onEnd([this](){this->OTAfinished();});
  ESPhttpUpdate.onProgress([this](int a, int b){this->OTAprogress(a,b);});
  ESPhttpUpdate.onError([this](int a){this->OTAerror(a);});
  ESPhttpUpdate.update(updateClient, otaLink);
}

String ESPboyOTA::fillPayload(String downloadID, String downloadName) {
  String payload = "{\"values\": \"";
  payload += "0";   // session
  payload += ", ";  // date/time
  payload += ", " + WiFi.localIP().toString();
  payload += ", " + downloadID;    // download ID
  payload += ", " + downloadName;  // download name
  payload += ", " + (String)ESP.getFreeHeap();
  payload += ", " + (String)ESP.getFreeContStack();
  payload += ", " + (String)ESP.getChipId();
  payload += ", " + (String)ESP.getFlashChipId();
  payload += ", " + (String)ESP.getCoreVersion();
  payload += ", " + (String)ESP.getSdkVersion();
  payload += ", " + (String)ESP.getCpuFreqMHz();
  payload += ", " + (String)ESP.getSketchSize();
  payload += ", " + (String)ESP.getFreeSketchSpace();
  payload += ", " + (String)ESP.getSketchMD5();
  payload += ", " + (String)ESP.getFlashChipSize();
  payload += ", " + (String)ESP.getFlashChipRealSize();
  payload += ", " + (String)ESP.getFlashChipSpeed();
  payload += ", " + (String)ESP.getCycleCount();
  payload += ", " + WiFi.SSID();
  payload += "\"}";

  return (payload);
}

void ESPboyOTA::postLog(String downloadID, String downloadName) {
  wificl.clientD = new HTTPSRedirect(httpsPort);
  wificl.clientD->setInsecure();
  wificl.clientD->setPrintResponseBody(false);
  wificl.clientD->setContentTypeHeader("application/json");
  wificl.clientD->setPrintResponseBody(true);

  int connectionAttempts = 0;
  while (wificl.clientD->connect(hostD, httpsPort) != 1 && connectionAttempts++ < 5)
    ;
  if (connectionAttempts > 4) {
    printConsole(F("Server failed"), TFT_RED, 0, 0);
    delay(5000);
    ESP.reset();
  }

  printConsole(F("Server OK"), TFT_MAGENTA, 0, 0);
  String payload = fillPayload(downloadID, downloadName);
  // Serial.println(payload);
  wificl.clientD->POST(urlPost, hostD, payload, false);
  // Serial.println(wificl.clientD->getResponseBody());

  wificl.clientD->stop();
  delete wificl.clientD;
}

firmware ESPboyOTA::getFirmware() {
  uint16_t firmwareNo;
  String readedData = "";
  firmware fmw;
  
  wificl.clientD = new HTTPSRedirect(httpsPort);
  wificl.clientD->setInsecure();
  wificl.clientD->setPrintResponseBody(false);

  int connectionAttempts = 0;
  while (wificl.clientD->connect(hostD, httpsPort) != 1 && connectionAttempts++ < 5)
    ;
  if (connectionAttempts > 4) {
    printConsole(F("Server faild"), TFT_RED, 0, 0);
    delay(5000);
    ESP.reset();
  }

  printConsole(F("Loading Apps..."), TFT_MAGENTA, 0, 0);

  if (wificl.clientD->GET((String)urlPost + F("?read"), hostD)) {
    readedData = wificl.clientD->getResponseBody();

    uint16_t countVector = 0;
    char* prs = strtok((char*)readedData.c_str(), ";\n");
    while (prs != NULL) {
      fwList.push_back(fw());
      fwList[countVector].fwName = prs;
      prs = strtok(NULL, ";\n");
      fwList[countVector].fwLink = prs;
      prs = strtok(NULL, ";\n");
      countVector++;
    }
  } else
    printConsole(F("Loading failed"), TFT_RED, 0, 0);

  for (uint8_t i = 0; i < fwList.size(); i++) {
    String toprint = (String)(i + 1) + " " + fwList[i].fwName;
    printConsole(toprint, TFT_GREEN, 0, 0);
  }

  firmwareNo = 0;
  while (!firmwareNo) {
    printConsole(F("Choose App:"), TFT_MAGENTA, 0, 0);
    firmwareNo = getUserInput().toInt();
    if (firmwareNo < 1 || firmwareNo > fwList.size()) firmwareNo = 0;
  }

  fmw.firmwareName = fwList[firmwareNo - 1].fwName;
  fmw.firmwareLink = fwList[firmwareNo - 1].fwLink;

  printConsole(fmw.firmwareName, TFT_YELLOW, 0, 0);
  printConsole(F("Loading info..."), TFT_MAGENTA, 0, 0);

  if (wificl.clientD->GET((String)urlPost + F("?info=") + String(firmwareNo + 1),
                   hostD)) {
    readedData = wificl.clientD->getResponseBody();
    char* prs = strtok((char*)readedData.c_str(), ";\n");
    printConsole(F("App name:"), TFT_MAGENTA, 0, 0);
    printConsole((String)prs, TFT_GREEN, 1, 0);
    prs = strtok(NULL, ";\n");
    printConsole(F("Type:"), TFT_MAGENTA, 0, 0);
    printConsole((String)prs, TFT_GREEN, 1, 0);
    prs = strtok(NULL, ";\n");
    printConsole(F("Genre:"), TFT_MAGENTA, 0, 0);
    printConsole((String)prs, TFT_GREEN, 1, 0);
    prs = strtok(NULL, ";\n");
    printConsole(F("Author:"), TFT_MAGENTA, 0, 0);
    printConsole((String)prs, TFT_GREEN, 1, 0);
    prs = strtok(NULL, ";\n");
    printConsole(F("License:"), TFT_MAGENTA, 0, 0);
    printConsole((String)prs, TFT_GREEN, 1, 0);
    prs = strtok(NULL, ";\n");
    printConsole(F("Info:"), TFT_MAGENTA, 0, 0);
    printConsole((String)prs, TFT_GREEN, 1, 0);
  } else
    printConsole(F("Failed"), TFT_RED, 0, 0);

  char approve = 0;
  while (approve == 0) {
    printConsole(F("Download App?  [y/n]"), TFT_MAGENTA, 0, 0);
    keybParam.typing="y";
    approve = getUserInput()[0];
  }


  if (approve == 'y' || approve == 'Y') {
    printConsole(F("YES"), TFT_YELLOW, 0, 0);
  } else {
    printConsole(F("NO"), TFT_YELLOW, 0, 0);
    fmw.firmwareName="";
  }

  fwList.clear();
  wificl.clientD->stop();
  delete wificl.clientD;
  return (fmw);
}

void ESPboyOTA::checkOTA() {
  firmware fmw;
  fmw.firmwareName="";
  WiFi.setAutoConnect(true);
  WiFi.mode(WIFI_STA);
  wifi_station_disconnect();
  printConsole(F("ESPboy App store"), TFT_YELLOW, 1, 0);
  // Serial.println(F("\n\nWiFi init OK"));
  while (!connectWifi()) delay(1500);
  postLog("no", "no");
  while (fmw.firmwareName == "") {
    fmw = getFirmware();
    delay(500);
  }
  postLog("no", fmw.firmwareName);
  updateOTA(fmw.firmwareLink);
  wifi_station_disconnect();
  delay(5000);
}
