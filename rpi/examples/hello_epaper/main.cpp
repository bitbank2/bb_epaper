//
// C++ example for bb_epaper library
// written by Larry Bank (bitbank@pobox.com)
// Project started 9/23/2024
// Copyright (c) 2024 BitBank Software, Inc.
//
#include "../../../src/bb_epaper.h"
//BBEPAPER bbep(EPD213_104x212); // InkyPHAT 2.13"
BBEPAPER bbep(EPD295_128x296);
//BBEPAPER bbep(EPD154R_152x152);
// BCM GPIO numbers used by Pimoroni e-paper "HATs"
#define PIN_DC 22
#define PIN_RST 27
#define PIN_BUSY 17
#define PIN_CS 8
#define SPI_BUS 0

int main(int argc, const char * argv[]) {

  bbep.initIO(PIN_DC, PIN_RST, PIN_BUSY, PIN_CS, SPI_BUS, 8000000);
  bbep.allocBuffer(); // draw into RAM first
  bbep.fillScreen(BBEP_WHITE);
  bbep.setRotation(90);
  bbep.setFont(FONT_12x16);
  bbep.setTextColor(BBEP_BLACK);
  bbep.drawString("Hello World!", 0, 0);
  bbep.setTextColor(BBEP_RED); // will be black on B/W panels
  bbep.drawString("In red!", 0, 16);
  bbep.writePlane(); // push the pixels from our RAM buffer to the e-epaper
  bbep.refresh(REFRESH_FULL); // do a full refresh (only type possible on B/W/R)
  bbep.sleep(DEEP_SLEEP); // turn off the epaper power circuit

  return 0;
} /* main() */


