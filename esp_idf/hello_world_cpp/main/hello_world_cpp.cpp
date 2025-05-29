#include <stdio.h>
#include <bb_epaper.h>
BBEPAPER bbep(EP102_80x128);

// Pinout for the LilyGo mini-epaper S3
#define PIN_BUSY 10
#define PIN_RST 11
#define PIN_DC 12
#define PIN_CS 13
#define PIN_SCK 14
#define PIN_MOSI 15

extern "C" void app_main(void)
{
  bbep.initIO(PIN_DC, PIN_RST, PIN_BUSY, PIN_CS, PIN_MOSI, PIN_SCK, 4000000);
  bbep.allocBuffer();
  bbep.fillScreen(BBEP_WHITE);
  bbep.setTextColor(BBEP_BLACK, BBEP_WHITE);
  bbep.setFont(FONT_12x16);
  bbep.println("Hello");
  bbep.println("C++!");
  bbep.writePlane();
  bbep.refresh(REFRESH_FULL);
  bbep.sleep(1);
}
