//
// C++ example for bb_epaper library
// written by Larry Bank (bitbank@pobox.com)
// This example shows how to do partial updates for any type of
// black and white epaper display even if the original pixels are
// lost (e.g. deep deep sleep or power failure)
// The target display is a 400x300 4.2" B/W UC8151
// You can easily select a different display by adjusting the
// coordinates and using a different font size
//
// Project started 9/23/2024
// Copyright (c) 2024 BitBank Software, Inc.
//
#include "../../../src/bb_epaper.h"
#include "Roboto_Black_78.h"
#include <time.h>
BBEPAPER bbep(EP42_400x300); // 4.2" UC81xx controller
// BCM GPIO numbers used by Pimoroni e-paper "HATs"
#define PIN_DC 22
#define PIN_RST 27
#define PIN_BUSY 17
#define PIN_CS 8
#define SPI_BUS 0
struct tm myTime;
time_t t;
int iFrame = 0;

void UpdateDisplay(int iFrame)
{
char szTemp[16];
    bbep.fillScreen(BBEP_WHITE);
    bbep.setFont(Roboto_Black_78);
    bbep.setTextColor(BBEP_BLACK);
    if (iFrame & 0xf) { // partial updates for 15 out of 16 frames
        // write the old pixels (old time) into the old 1-bit plane
	bbep.setPlane(PLANE_1);
	sprintf(szTemp, "%02d:%02d", myTime.tm_hour, myTime.tm_min);
	bbep.drawString(szTemp, 0, 200);
	bbep.writePlane(PLANE_1);
	bbep.setPlane(PLANE_0);
	bbep.fillScreen(BBEP_WHITE);
	t = time(NULL);
	myTime = *localtime(&t);
	sprintf(szTemp, "%02d:%02d", myTime.tm_hour, myTime.tm_min);
	bbep.drawString(szTemp, 0, 200);
	bbep.writePlane(PLANE_0); // write the new pixels
	bbep.refresh(REFRESH_PARTIAL);
    } else { // full update, just display the current time
        t = time(NULL);
	myTime = *localtime(&t);
        sprintf(szTemp, "%02d:%02d", myTime.tm_hour, myTime.tm_min);
	bbep.drawString(szTemp, 0, 200);
	bbep.writePlane();
	bbep.refresh(REFRESH_FULL);
    }
    bbep.sleep(DEEP_SLEEP); // extends the life of the display
} /* UpdateDisplay() */

int main(int argc, const char * argv[]) {

  bbep.initIO(PIN_DC, PIN_RST, PIN_BUSY, PIN_CS, SPI_BUS, 8000000);
  bbep.allocBuffer(); // Use RAM since we have plenty
  while (1) { // loop forever
      time_t tt;
      struct tm testTime;

      UpdateDisplay(iFrame++); // show the latest time
      testTime.tm_min = myTime.tm_min;
      // On a system with power saving features, you would sleep here
      while (testTime.tm_min == myTime.tm_min) { // wait for minute to change
          tt = time(NULL);
          testTime = *localtime(&tt);
      }
  }
  return 0; // it never gets here
} /* main() */


