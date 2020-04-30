/*
ESPboyOTA class -- ESPboy App Store client core
for www.ESPboy.com project by RomanS
https://hackaday.io/project/164830-espboy-games-iot-stem-for-education-fun
thanks to DmitryL (Plague) for help, tests and advices
*/

#ifndef ESPboy_OTA
#define ESPboy_OTA

#include <Adafruit_MCP23017.h>
#include <TFT_eSPI.h>
#include <HTTPSRedirect.h>
#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>

#define OTA_TIMEOUT_CONNECTION 10000
#define OTA_MAX_CONSOLE_STRINGS 16
#define OTA_MAX_STRINGS_ONSCREEN_FULL 15
#define OTA_MAX_STRINGS_ONSCREEN_SMALL 10
#define OTA_MAX_TYPING_CHARS 60
#define OTA_KEY_UNPRESSED_TIMEOUT 700
#define OTA_KEY_PRESSED_DELAY_TO_SEND 500
#define OTA_CURSOR_BLINKING_PERIOD 500 
#define OTA_KEYB_CALL_DELAY 150 //auto repeat

#define OTA_PAD_LEFT        0x01
#define OTA_PAD_UP          0x02
#define OTA_PAD_DOWN        0x04
#define OTA_PAD_RIGHT       0x08
#define OTA_PAD_ACT         0x10
#define OTA_PAD_ESC         0x20
#define OTA_PAD_LFT         0x40
#define OTA_PAD_RGT         0x80
#define OTA_PAD_ANY         0xff

struct firmware{
    String firmwareName;
    String firmwareLink;
};

struct wf {
    String ssid;
    uint8_t rssi;
    uint8_t encription;
};

struct fw {
    String fwName;
    String fwLink;
};

struct lessRssi{
    inline bool operator() (const wf& str1, const wf& str2) {return (str1.rssi > str2.rssi);}
};


class ESPboyOTA{

private:
  Adafruit_MCP23017 *mcp; 
  TFT_eSPI *tft;

  std::vector<wf> wfList;  // WiFi list
  std::vector<fw> fwList;  // Firmware list

  struct keyboardParameters{
    uint8_t renderLine;
    uint8_t displayMode;
    uint8_t shiftOn;
    int8_t selX;
    int8_t selY;
    String typing;
  }keybParam;


  struct wificlient{
    String ssid;
    String pass;
  HTTPSRedirect *clientD;
  }wificl;

  const static uint8_t keybOnscr[2][3][21] PROGMEM;
  const static char PROGMEM *hostD;
  const static char PROGMEM *urlPost;
  const static uint16_t PROGMEM httpsPort;

  static String *consoleStrings;
  static uint16_t *consoleStringsColor;


	uint8_t keysAction();
	void toggleDisplayMode(uint8_t mode);
	void drawConsole(uint8_t onlyLastLine);
	uint8_t getKeys();
	uint32_t waitKeyUnpressed();
	void drawKeyboard(uint8_t slX, uint8_t slY, uint8_t onlySelected);
	void drawTyping(uint8_t);
	void drawBlinkingCursor();
	
	uint16_t scanWiFi();
	String getWiFiStatusName();
	boolean connectWifi();
	void OTAstarted();
	void OTAfinished();
	void OTAprogress(int cur, int total);
	void OTAerror(int err);
	void updateOTA(String otaLink);
	String fillPayload(String downloadID, String downloadName);
	void postLog(String downloadID, String downloadName);
	firmware getFirmware();
	void checkOTA();
	void printConsole(String bfrstr, uint16_t color, uint8_t ln, uint8_t noAddLine);
  String getUserInput();
  
public:
	ESPboyOTA(TFT_eSPI *tftOTA, Adafruit_MCP23017 *mcpOTA);
};

#endif
