//
// C++ example for bb_epaper library
// written by Larry Bank (bitbank@pobox.com)
// Project started 9/23/2024
// Copyright (c) 2024 BitBank Software, Inc.
//
#include "../../../src/bb_epaper.h"
BBEPAPER bbep(EPD27_176x264); // Waveshare 2.7" e-paper HAT
// BCM GPIO numbers
#define PIN_DC 25
#define PIN_RST 17
#define PIN_BUSY 24
#define PIN_CS 8

int main(int argc, const char * argv[]) {
  bbep.initIO(PIN_DC, PIN_RST, PIN_BUSY, PIN_CS, 0, 8000000);
  bbep.fillRect(0,0,128,296, BBEP_WHITE);
  bbep.setRotation(90);
  bbep.setFont(FONT_12x16);
  bbep.setTextColor(BBEP_BLACK, BBEP_WHITE);
  bbep.drawString("Hello World!", 0, 0);
  bbep.refresh(REFRESH_FULL);
  bbep.sleep(DEEP_SLEEP);

    return 0;
} /* main() */


