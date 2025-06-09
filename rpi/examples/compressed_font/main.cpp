//
// C++ example for bb_epaper library
// written by Larry Bank (bitbank@pobox.com)
// Project started 9/23/2024
// Copyright (c) 2024 BitBank Software, Inc.
//
#include "../../../src/bb_epaper.h"
#include "Roboto_Black_40.h"
BBEPAPER bbep(EP42R_400x300);
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
      printf("Setting rotation to 90\n");
  }
  bbep.fillScreen(BBEP_WHITE);
  bbep.setFont(Roboto_Black_40);
  bbep.setTextColor(BBEP_BLACK);
  bbep.drawString("Unicode!", 0, 60); // the position is from the baseline
  bbep.setTextColor(BBEP_RED); // RED will be black on B/W panels
  bbep.drawString("áàôç¿", 0, 120); // UTF-8 (codepage 1252)
  bbep.setFont(FONT_12x16);
  bbep.setTextColor(BBEP_BLACK);
  bbep.drawString("Built-in fonts can be used too.", 0, 144);
  bbep.fillCircle(200, 250, 50, BBEP_BLACK);
  bbep.fillCircle(200, 250, 25, BBEP_RED);
  bbep.writePlane(); // push the pixels from our RAM buffer to the e-epaper
  printf("Starting full update\n");
  bbep.refresh(REFRESH_FULL); // do a full refresh (only type possible on B/W/R)
  printf("Finished update\n");
  bbep.sleep(DEEP_SLEEP); // turn off the epaper power circuit

  return 0;
} /* main() */


