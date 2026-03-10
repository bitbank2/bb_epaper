//
// Compressed font example
// written by Larry Bank
//
// This sketch demonstrates how to work with BBF (BitBank Font) files
//
#include <bb_epaper.h>
#include "../Fonts/Roboto_Black_24.h" // use one of the provided TTF->BBF fonts

BBEPAPER bbep; // class instance of bb_epaper display library

void setup()
{
  bbep.begin(EPD_CROWPANEL42); // One of the many pre-defined IoT configurations
  bbep.allocBuffer(); // allocate a local buffer to hold the graphics
  bbep.fillScreen(BBEP_WHITE); // clear all pixels to white
  bbep.setTextColor(BBEP_BLACK, BBEP_WHITE); // foreground / background
  bbep.setFont(Roboto_Black_24);
  bbep.setCursor(0, 60); // TrueType fonts start from the baseline, not the top
  bbep.println("bb_epaper font\nsupport is easy.");
  bbep.setItalic(true); // you can enable italic mode for any font
  bbep.println("It has italics too!");
  bbep.setItalic(false);
  bbep.println("Plus EU áçêñts!");
  bbep.writePlane(); // write the local framebuffer to the epaper panel
  bbep.refresh(REFRESH_FULL); // full flashing refresh
  bbep.sleep(LIGHT_SLEEP); // turn off the panel's power (saves RAM contents)
} /* setup() */

void loop()
{
} /* loop() */

