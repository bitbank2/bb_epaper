//
// bb_epaper demo for Spectra6 panels
// In this case, we're using the M5Stack Paper Color (4" 400x600 Spectra6 panel)
//
#include <bb_epaper.h>
// Use one of the built-in fonts
#include "Fonts/Roboto_Black_24.h"
BBEPAPER bbep;

void setup(void)
{
int rc;

   Serial.begin(115200);
   delay(3000); // give time for USB->CDC to start
   Serial.println("Starting...");
   // For IoT devices with built-in ePaper displays, bb_epaper includes the complete configuration
   // (GPIO connections and panel type) so that you can initialize them with a single constant
   rc = bbep.begin(EPD_M5_PAPER_COLOR); // M5Stack Paper Color (ESP32-S3, 4" 400x600 Spectra6)
   if (rc == BBEP_SUCCESS) {
      bbep.allocBuffer(); // Optional local copy of the framebuffer; best to use when using text & gfx functions
      bbep.fillScreen(BBEP_WHITE);
      bbep.setTextColor(BBEP_GREEN);
      bbep.setFont(Roboto_Black_24);
      bbep.setCursor(0, 40); // TrueType font baseline is not the top edge of the character
      bbep.println("bb_epaper");
      bbep.setTextColor(BBEP_BLUE);
      bbep.println("M5 Paper Color!");
      bbep.setTextColor(BBEP_BLACK);
      bbep.println("Spectra6 400x600");
      bbep.fillCircle(100, 300, 60, BBEP_YELLOW);
      bbep.fillCircle(200, 300, 60, BBEP_BLACK);
      bbep.fillCircle(300, 300, 60, BBEP_RED);
      bbep.writePlane(); // Send the local graphics copy to the EPD memory
      bbep.refresh(REFRESH_FULL); // This panel only supports full (slow) refresh - for now :)
      bbep.sleep(LIGHT_SLEEP); // put the panel into light sleep (retains RAM contents)
   }
} /* setup() */

void loop(void)
{
} /* loop() */

