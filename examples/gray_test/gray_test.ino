//
// 4-gray test sketch
//
#include <bb_epaper.h>
#include "Fonts/Roboto_Black_24.h"
BBEPAPER bbep;

void setup(void)
{
   bbep.begin(EPD_BADGER2350_4GRAY);
   bbep.allocBuffer();
   bbep.setFont(Roboto_Black_24);
   bbep.fillScreen(BBEP_GRAY3); // white
   bbep.setCursor(0, 36);
   bbep.setTextColor(BBEP_GRAY0, BBEP_GRAY3);
   bbep.println("Pimoroni");
   bbep.setTextColor(BBEP_GRAY1, BBEP_GRAY3);
   bbep.println("Badger2350");
   bbep.setTextColor(BBEP_GRAY2, BBEP_GRAY3);
   bbep.println("4-Gray Test");
   bbep.writePlane();
   bbep.refresh(REFRESH_FULL);
   delay(2000);
   bbep.fillScreen(BBEP_GRAY3);
   bbep.setCursor(0,36);
   bbep.setTextColor(BBEP_GRAY0, BBEP_GRAY3);
   bbep.println("Gray 0");
   bbep.setTextColor(BBEP_GRAY1, BBEP_GRAY3);
   bbep.println("Gray 1 (fast)");
   bbep.setTextColor(BBEP_GRAY2, BBEP_GRAY3);
   bbep.println("Gray 2");
   bbep.writePlane();
   bbep.refresh(REFRESH_FAST);
   delay(2000);
   bbep.fillScreen(BBEP_GRAY3);
   bbep.writePlane();
   bbep.refresh(REFRESH_FULL);
   bbep.sleep(LIGHT_SLEEP);
} /* setup() */

void loop(void)
{

} /* loop() */
