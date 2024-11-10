//
// Example sketch showing how to display uncompressed and compressed images without a backbuffer
// written by Larry Bank (bitbank@pobox.com)
//
#include <bb_epaper.h>
#include "bomb.h" // uncompressed 40x40 image
#include "bart.h" // compressed 128x64 image

BBEPAPER bbep(EP295_128x296);
// Mike Rankin's C3 e-ink PCB
#define PIN_DC 3
#define PIN_BUSY 1
#define PIN_RST 10
#define PIN_CS 7
#define PIN_MOSI 6
#define PIN_SCK 4

void setup()
{
  bbep.initIO(PIN_DC, PIN_RST, PIN_BUSY, PIN_CS, PIN_MOSI, PIN_SCK, 8000000);
  // Do everything with no local buffer
  bbep.fillScreen(BBEP_WHITE);
  bbep.setCursor(0,4); // start a few pixels away from the top edge
  bbep.setFont(FONT_8x8); // use the internal 8x8 font
  bbep.setTextColor(BBEP_BLACK);
  bbep.println("   bb_epaper");
  bbep.println("Icons and Images");
  // without a backbuffer, bb_epaper can only draw graphics in the native memory direction
  for (int x=0; x<=80; x += 40) {
      // draw 3x uncompressed icon at x,24
      bbep.drawSprite(ucBombMask, SPRITE_WIDTH, SPRITE_HEIGHT, SPRITE_PITCH, x, 24, BBEP_BLACK);
  }
  for (int y=80; y<272; y+=64) {
      bbep.loadG5Image(bart, 0, y, BBEP_BLACK, BBEP_WHITE); // draw 3x 128x64 compressed Bart image at 0,y
  }
  bbep.refresh(REFRESH_FULL);
  bbep.wait();
  bbep.sleep(DEEP_SLEEP);
}

void loop()
{
}
