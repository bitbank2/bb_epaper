//
// C++ example for bb_epaper library
// written by Larry Bank (bitbank@pobox.com)
// Project started 9/23/2024
// Copyright (c) 2024 BitBank Software, Inc.
//
#include <bb_epaper.h>
BBEPAPER bbep(EP213_122x250);
// BCM GPIO numbers used by Pimoroni e-paper "HATs"
#define PIN_DC 22
#define PIN_RST 27
#define PIN_BUSY 17
#define PIN_CS 8
#define SPI_BUS 0

int main(int argc, const char * argv[]) {

  bbep.initIO(PIN_DC, PIN_RST, PIN_BUSY, PIN_CS, SPI_BUS, 8000000);
  bbep.allocBuffer(); // draw into RAM first
  if (bbep.width() < bbep.height()) { // rotate to landscape mode
      bbep.setRotation(90);
  }
  bbep.fillScreen(BBEP_WHITE);
  bbep.setFont(FONT_12x16);
  bbep.setTextColor(BBEP_BLACK);
  bbep.setCursor(0,0);
  bbep.println("arrrrrgh Pimoroni");
  bbep.println("Inkyphat is easy to");
  bbep.println("use in bb_epaper!");
  for (int i=20; i<50; i += 5) {
     bbep.drawRoundRect(bbep.width()/2 - i, bbep.height()/2, i*2, i, 10, BBEP_BLACK);
  }
  bbep.writePlane(); // push the pixels from our RAM buffer to the e-epaper
  bbep.refresh(REFRESH_FULL); // do a full refresh (only type possible on B/W/R)
  bbep.sleep(DEEP_SLEEP); // turn off the epaper power circuit
  return 0;
} /* main() */


