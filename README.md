# ESPboy_little_game_engine

LGE is a fantasy console for making, sharing and playing tiny games and other computer programs. 
You can make you game or program using online compiler and run compiled binary on portable ESPboy console thouse feels like a regular console. Check few, colored, nostalgic LGE games from the package, by uploading them to SPIFFS.

Games sources:
https://github.com/corax89/game_engine_for_esp8266_with_compiler/tree/master/example

Hackaday:
https://hackaday.io/project/164205-esp-little-game-engine

Article:
https://hackaday.com/2019/03/11/esp8266-gets-its-game-on-with-open-source-engine/

User Guide:
https://corax89.github.io/esp8266Game/user_guide/index.html

Online ESP-LGE SDK, compiler, emulator:
https://corax89.github.io/esp8266Game/index.html

How to compile for ESPboy:
1. Install Arduino IDE
2. Clone and download "ESPboy_little_game_engine" and rename the folder "ESPboy_little_game_engine-master" to "ESPboy_little_game_engine"
3. Unpack two libraries (a_coos and TFT_eSPI) from file "libraries.7z" to the Arduino/Libraries folder
4. Change settings in file «User_Setup.h» of TFT_eSPI library
  - 50  #define TFT_WIDTH  128
  - 53  #define TFT_HEIGHT 128
  - 67  #define ST7735_GREENTAB3
  - 149 #define TFT_CS   -1
  - 150 #define TFT_DC   PIN_D8
  - 152 #define TFT_RST  -1  
  - 224 #define LOAD_GLCD
  - 255 #define SPI_FREQUENCY  27000000
5. Open "esp8266_game_engine.ino" in Arduino IDE and compile it for "Lolin/WeMos D1 mini ESP8266" board
6. Upload the games to your ESPboy SPIFFS 
7. Enjoy )

How to upload games to SPIFFS:
https://www.youtube.com/watch?v=25eLIdLKgHs


How to recompile CHIP8 games:
1. Use source file https://github.com/corax89/game_engine_for_esp8266_with_compiler/blob/master/example/chip8.c
2. Put your CHIP8 bit code to char "Memory[3583] = {};" at string ¹58
3. Check buttons remap at strings ¹9-20
4. Compile this .c to .bin with "Online ESP-LGE SDK, compiler, emulator" https://corax89.github.io/esp8266Game/index.html
5. Upload .bit to your ESPboy SPIFFS


Games in package:

1. Original ESP-LGE by Igor: FourInaRow, NinjaEscape, 1916, WormBlast, ESProgue, Galaxies, CityRunner, Memories, MarsAttack, Columns, Mines, Breackout, TowerDefense,
...




![logo](/logo.png)
# esp8266_little_game_engine
Used ili9341. Contains a virtual machine running games from RAM.

Used library:

[https://github.com/akouz/a_coos](https://github.com/akouz/a_coos)

[https://github.com/Bodmer/TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)

Online emulator with compiler:

[https://corax89.github.io/esp8266Game/index.html](https://corax89.github.io/esp8266Game/index.html)

Video:

[![video](http://img.youtube.com/vi/roOQHuXNVoI/0.jpg)](https://www.youtube.com/watch?v=roOQHuXNVoI "ESP8266 game engine")
