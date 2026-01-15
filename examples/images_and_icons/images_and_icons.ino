//
// Example sketch showing how to display uncompressed and compressed images without a backbuffer
// written by Larry Bank (bitbank@pobox.com)
//
#include <bb_epaper.h>
#include "bomb.h" // uncompressed 40x40 image
#include "bart.h" // compressed 128x64 image
#include "smiley.h" // 100x100 compressed image
BBEPAPER bbep;

void setup()
{
  bbep.begin(EPD_XTEINK_X4);
  bbep.allocBuffer();
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
  bbep.writePlane();
  bbep.refresh(REFRESH_FULL);
  delay(3000);
  bbep.fillScreen(BBEP_WHITE);
  for (int y=0; y<4; y++) {
      bbep.loadG5Image(bart, (y&1)*128, (y&2)*32, BBEP_WHITE, BBEP_BLACK); // draw 3x 128x64 compressed Bart image at 0,y
      bbep.writePlane(PLANE_FALSE_DIFF); // force update of all pixels
      bbep.refresh(REFRESH_PARTIAL);
  }
  delay(3000);
  bbep.fillScreen(BBEP_WHITE);
  bbep.setFont(FONT_12x16);
  bbep.setCursor(0,0);
  bbep.print("Scaled G5 images");
  float f = 0.5;
  for (int i=0; i<5; i++) {
    bbep.loadG5Image(smiley, (int)(i*50), 16+(int)(i*12.5*f), BBEP_TRANSPARENT, BBEP_BLACK, f);
    f += 0.25;
  }
  bbep.writePlane();
  bbep.refresh(REFRESH_FAST);
  bbep.sleep(DEEP_SLEEP);
}

void loop()
{
}
