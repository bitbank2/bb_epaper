//
// 3 color (Black/White/Red) example
//
#include <bb_epaper.h>
#include "smiley_hearts.h"
BBEPAPER bbep(EP29YR_168x384);
// My Xiao ESP32C3 + LCD Adapter
#define DC_PIN 4
#define BUSY_PIN 20
#define RESET_PIN 5
#define CS_PIN 21

void setup()
{
  bbep.initIO(DC_PIN, RESET_PIN, BUSY_PIN, CS_PIN, -1, -1);
  bbep.allocBuffer(); // use a back buffer
 // bbep.setRotation(270);
  bbep.fillScreen(BBEP_WHITE);
  bbep.setTextColor(BBEP_RED);
  bbep.setFont(FONT_12x16);
  bbep.println("bb_epaper v1");
  bbep.setTextColor(BBEP_BLACK);
  bbep.println("B/W/Y/R Test");
  for (int i=1; i<44; i++) {
    bbep.drawCircle(bbep.width()/2, 80, i, (i < 22) ? BBEP_BLACK : BBEP_RED);
  }
  bbep.loadG5Image(smiley_hearts, 0, 160, 0,0,1.0f);
  bbep.writePlane();
  bbep.refresh(REFRESH_FULL);
  bbep.wait();
  bbep.sleep(DEEP_SLEEP);
}

void loop()
{

}