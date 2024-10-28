//
// 3 color (Black/White/Red) example
//
#include <bb_epaper.h>
BBEPAPER bbep(EPD154R_152x152); // 1.54" 152x152 B/W/R
// My Arduino Nano 33 BLE e-paper adapter
#define DC_PIN 16
#define BUSY_PIN 15
#define RESET_PIN 14
#define CS_PIN 10

void setup()
{
  bbep.initIO(DC_PIN, RESET_PIN, BUSY_PIN, CS_PIN);
//  bbep.allocBuffer(); // use a back buffer
  bbep.fillScreen(BBEP_WHITE);
  bbep.setTextColor(BBEP_RED);
  bbep.setFont(FONT_8x8);
  bbep.println("bb_epaper v1");
  bbep.setTextColor(BBEP_BLACK);
  bbep.println("B/W/R Test");
//  bbep.writePlane();
  bbep.refresh(REFRESH_FULL);
  bbep.wait();
  bbep.sleep(DEEP_SLEEP);
}

void loop()
{

}