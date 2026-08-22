//
// C++ example for bb_epaper library
// written by Larry Bank (bitbank@pobox.com)
// Project started 9/23/2024
// Copyright (c) 2024 BitBank Software, Inc.
//
#include "../../../src/bb_epaper.h"
#include "Roboto_Black_32.h"
//BBEPAPER bbep(EP29R_128x296);
BBEPAPER bbep(EP213B_122x250); // Waveshare 2.13" ePaper HAT
// BCM GPIO numbers used by Pimoroni e-paper "HATs"
//#define CHIP 0
//#define PIN_DC 22
//#define PIN_RST 27
//#define PIN_BUSY 17
//#define PIN_CS 8
//#define SPI_BUS 0
//#define PIN_PWR -1
// BCM GPIO numbers for the Waveshare 2.13" e-paper HAT
//#define CHIP 0
//#define PIN_DC 25
//#define PIN_CS 8
//#define PIN_RST 17
//#define PIN_BUSY 24
//#define SPI_BUS 0
//#define PIN_PWR 18
//#define CHIP 0
// GPIOD numbers for Rock Pi 4B+ (RK3399)
#define CHIP 4
#define PIN_DC 29
#define PIN_RST 18
#define PIN_BUSY 28
#define PIN_CS -1
#define SPI_BUS 0
#define PIN_PWR -1
int main(int argc, const char * argv[]) {

  if (PIN_PWR != -1) {
      pinMode(PIN_PWR, OUTPUT);
      digitalWrite(PIN_PWR, 1);
  }
  bbep.initIO(PIN_DC, PIN_RST, PIN_BUSY, PIN_CS, SPI_BUS, CHIP, 8000000);
  bbep.allocBuffer(); // draw into RAM first
  bbep.fillScreen(BBEP_WHITE);
  bbep.setRotation(90);
  bbep.setFont(FONT_12x16);
  bbep.setTextColor(BBEP_BLACK);
  bbep.drawString("Hello World!", 0, 0);
  bbep.setFont(Roboto_Black_32);
  bbep.setTextColor(BBEP_RED);
  bbep.drawString("Big", 0, 60);
  bbep.drawString("Fonts", 0, 120);
  bbep.writePlane(); // push the pixels from our RAM buffer to the e-epaper
  bbep.refresh(REFRESH_FULL); // do a full refresh (only type possible on B/W/R)
  bbep.sleep(DEEP_SLEEP); // turn off the epaper power circuit
  if (PIN_PWR != -1) {
       digitalWrite(PIN_PWR, 0);
  }
  return 0;
} /* main() */


