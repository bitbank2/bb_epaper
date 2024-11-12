#include <OneBitDisplay.h>
#include <bb_epaper.h>
#include "Roboto_Thin_48.h"
#include "bart_47x128.h"
ONE_BIT_DISPLAY oled;
BBEPAPER epd(EP29_128x296);
int iDataTime, iOpTime;

#ifdef HAL_ESP32_HAL_H_
// These values are for the Laska_Kit ESPInk board
// with a pushbutton and pull down resistor attached
// to the RX0/TX0 signals exposed on the 8-pin female programming header
#if defined (ARDUINO_FEATHERS3) || defined(ARDUINO_FEATHERS2)
#define CS_PIN 5
#define DC_PIN 6
#define RESET_PIN 12
#define BUSY_PIN 14
#define CLK_PIN 36
#define MOSI_PIN 35
#define POWER_PIN 39
#else // must be Laska_Kit ESPInk
#define CS_PIN 5
#define DC_PIN 17
#define RESET_PIN 16
#define BUSY_PIN 4
#define CLK_PIN 18
#define MOSI_PIN 23
#define POWER_PIN 2
#endif

#define BUTTON1 1
#define BUTTON2 3
#else
// These pin assignments are for a custom e-paper
// adapter add-on with 2 push buttons
// for the Arduino Nano 33 BLE
#define CS_PIN 10
#define DC_PIN 16
#define RESET_PIN 14
#define BUSY_PIN 15
#define CLK_PIN -1
#define MOSI_PIN -1
#define POWER_PIN -1

#define BUTTON1 2
#define BUTTON2 3
#endif
int iPanel = 0;
int iMode = 0; // 0=EPD test

// Friendly name for the OneBitDisplay library e-paper panel types
// The first 2 digits signify the panel size (e.g. 42 = 4.2")
// A letter "R" after the size indicates a Black/White/RED panel
// The X*Y indicate the native pixel resolution
const char *szPanelNames[] = {
  "EP42_400x300   ", // WFT0420CZ15
  "EP42B_400x300  ",
  "EP213_122x250  ", // waveshare
  "EP213B_122x250 ", // GDEY0213B74
  "EP293_128x296  ",
  "EP294_128x296  ",
  "EP295_128x296  ",
  "EP266_152x296  ", // GDEY0266T90
  "EP102_80x128   ",
  "EP27B_176x264  ", // GDEY027T91
  "EP29R_128x296  ",
  "EP122_192x176  ", // GDEM0122T61
  "EP154R_152x152 ",
  "EP42R_400x300  ",
  "EP42R2_400x300 ",
  "EP37_240x416   ", // DEPG0370BS
  "EP213_104x212  ",
  "EP75_800x480   ",
  "EP29_128x296   ",
  "EP213R_122x250 ",
  "EP154_200x200  ",
  "EP154B_200x200 ",
  "EP266YR_184x360", // GDEY0266F51
  "EP29YR_128x296 ", // GDEY029F51
  "EP29YR_168x384 ", // GDEY029F51H
  NULL
};
// names of the operating modes
const char *szMode[] = {"Full upd wo/RAM ", "Full upd w/RAM  ", "Fast Update     ", "Partial Update  ", "Clear to white  "};
enum {
  MODE_FULL_NO_RAM = 0,
  MODE_FULL_RAM,
  MODE_FAST,
  MODE_PARTIAL,
  MODE_CLEAR,
  MODE_COUNT
};

// List of supported colors for each panel type
// 2 = Black/White, 3 = Black/White/Red
const uint8_t u8PanelColors[] = {2,2,2,2,2,2,2,2,2,2,3,2,3,3,3,2,2,2,2,3,2,2,4,4,4};

void epdBegin()
{
  if (POWER_PIN != -1) {
    pinMode(POWER_PIN, OUTPUT);
    digitalWrite(POWER_PIN, HIGH);
    delay(100); // allow time to settle
  }
  epd.initIO(DC_PIN, RESET_PIN, BUSY_PIN, CS_PIN, MOSI_PIN, CLK_PIN);
} /* epdBegin() */

// Display info about the currently selected panel
void ShowInfo(bool bErase)
{
  if (bErase) {
    oled.fillScreen(OBD_WHITE);
  }
  oled.setCursor(0, 0);
  oled.setFont(FONT_8x8);
  oled.println("EPD Panel Tester");
  oled.setCursor(0, 16);
  oled.print(szPanelNames[iPanel]);
  oled.print("      "); // erase old name if longer
  oled.setFont(FONT_6x8);
  oled.setCursor(0, 24);
  oled.print("                    "); // erase any old text
  oled.setCursor(0, 24);
  if (epd.hasFastRefresh()) {
    oled.print(" fast");
  }
  if (epd.hasPartialRefresh()) {
    oled.print(" partial");
  }
  if (epd.capabilities() & BBEP_3COLOR) {
    oled.print(" 3 colors");
  }
  oled.setCursor(0, 32);
  oled.print(" controller: ");
  oled.print((epd.getChip() == BBEP_CHIP_SSD16xx) ? "SSD16xx" : "UC8xxx ");
  oled.setCursor(0, 48);
  oled.setFont(FONT_8x8);
  oled.println(szMode[iMode]);
  oled.setFont(FONT_6x8);
  oled.print("(press&hold to exec)");
} /* ShowInfo() */

void DrawScene(int i, int iPlane)
{
  char szTemp[16];
  epd.setPlane(iPlane);
  epd.fillScreen(BBEP_WHITE, iPlane);
  epd.setFont(FONT_12x16);
  epd.setTextColor(BBEP_BLACK);
  epd.setCursor(0,0);
  epd.println("bb_epaper");
  epd.println("partial");
  epd.println("Update demo");
  sprintf(szTemp, "%04d", i);
  epd.setCursor(0, epd.height()-4);
  epd.setFont(Roboto_Thin_48);
  epd.println(szTemp);
}  /* DrawScene() */

void EPDTest(int iMode)
{
  oled.fillScreen(OBD_WHITE);
  oled.setFont(FONT_12x16);
  oled.println("Starting...");
  oled.setFont(FONT_8x8);
  oled.println(szMode[iMode]);
  if (iMode == MODE_PARTIAL) {
    int i;

    if (!epd.hasPartialRefresh()) {
      oled.setFont(FONT_8x8);
      oled.println("This EPD doesn't");
      oled.println("Support partial\nupdates");
      iDataTime = iOpTime = 0;
      return;
    }
    epdBegin();
    if (epd.width() < epd.height()) {
       epd.setRotation(90);
    }
    epd.allocBuffer();
    epd.fillScreen(BBEP_WHITE, PLANE_DUPLICATE);
    DrawScene(0, PLANE_0);
    epd.writePlane(PLANE_BOTH);
    epd.refresh(REFRESH_FULL, true);
  // count from 0 to 9 using partial updates
  i = 0;
  while (i<10) {
    DrawScene(i, PLANE_1); // draw scene to old plane 1 (not always needed, but doesn't hurt to do)
    i++;
    DrawScene(i, PLANE_0); // new scene drawn to plane 0
    epd.writePlane(PLANE_BOTH);
    epd.refresh(REFRESH_PARTIAL, true);
    epd.sleep(LIGHT_SLEEP);
  } // for i
    iDataTime = epd.dataTime();
    iOpTime = epd.opTime();
    epd.freeBuffer();
    epd.fillScreen(BBEP_WHITE);
    epd.refresh(REFRESH_FULL, true);
  } else { // slow/fast
    epdBegin();
    if (epd.width() < epd.height()) {
       epd.setRotation(90);
    }
    if (iMode == MODE_FAST && !epd.hasFastRefresh()) {
      oled.setFont(FONT_8x8);
      oled.println("This EPD doesn't");
      oled.println("Support fast\nupdates");
      iDataTime = iOpTime = 0;
      return;
    }
    if (iMode == MODE_FULL_RAM) {
        epd.allocBuffer();
    }
    epd.fillScreen(BBEP_WHITE);
    epd.setTextColor(BBEP_BLACK);
    if (epd.width() < 296)
      epd.setFont(FONT_8x8);
    else
      epd.setFont(FONT_12x16);
    epd.println("EPD Test");
    epd.print("Name: ");
    epd.println(szPanelNames[iPanel]);
    epd.print("Size: ");
    epd.print(epd.width(), DEC);
    epd.print("x");
    epd.println(epd.height(), DEC);
    if (epd.getBuffer()) {
      epd.println("With backbuffer");
      epd.loadBMP(bart_47x128, epd.width()-47, 0, BBEP_RED, BBEP_WHITE);
    } else {
      epd.println("No backbuffer (0 RAM)");
    }
    if (u8PanelColors[iPanel] == 2)
      epd.println("Two colors (B/W)");
    else if (u8PanelColors[iPanel] == 3) {
      epd.setTextColor(BBEP_RED, BBEP_WHITE);
      epd.println("Three colors (B/W/R)");
    } else {
      epd.setTextColor(BBEP_RED, BBEP_WHITE);
      epd.print("Four ");
      epd.setTextColor(BBEP_YELLOW, BBEP_WHITE);
      epd.println("colors (B/W/Y/R)");
    }
    if (epd.getBuffer()) {
      epd.writePlane();
    }
    epd.refresh((iMode == MODE_FAST) ? REFRESH_FAST : REFRESH_FULL, true);
    if (epd.getBuffer()) {
      epd.freeBuffer();
    }
    iDataTime = epd.dataTime();
    iOpTime = epd.opTime();
  } // slow/fast
  epd.sleep(DEEP_SLEEP);
} /* EPDTest() */

void EPDClear(void)
{
  oled.fillScreen(OBD_WHITE);
  oled.setFont(FONT_12x16);
  oled.println("Clear EPD");
  oled.println("Starting...");
  epdBegin();
  if (epd.width() < epd.height() || !epd.getBuffer()) {
       epd.setRotation(90);
  }
  epd.fillScreen(BBEP_WHITE);
  epd.refresh(REFRESH_FULL, true); // display the white buffer and return
  iDataTime = epd.dataTime();
  iOpTime = epd.opTime();
  epd.sleep(DEEP_SLEEP);
} /* EPDClear() */

void ShowTime(void)
{
  if (iDataTime || iOpTime) {
    oled.fillScreen(OBD_WHITE);
    oled.setFont(FONT_8x8);
    oled.println("Perf Report for:");
    oled.println(szPanelNames[iPanel]);
    oled.println("Data xfer @ 8MHz");
    oled.print((int)iDataTime, DEC);
    oled.println(" ms");
    oled.println("\nEPD Op Time");
    oled.print((int)iOpTime, DEC);
    oled.println(" ms");
  }
  oled.setFont(FONT_6x8);
  oled.setCursor(0,56);
  oled.println("(Any button to exit)");
  while (digitalRead(BUTTON1) == 1 && digitalRead(BUTTON2) == 1) {
    delay(10);
  }
  // wait for button release
  while (digitalRead(BUTTON1) == 0 || digitalRead(BUTTON2) == 0) {
    delay(10);
  }
} /* ShowTime() */

void setup() {
  oled.I2Cbegin();
  oled.fillScreen(OBD_WHITE);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
//  Serial.begin(115200);
//  delay(3000);
//  Serial.println("EPD BLE Tester");
} /* setup() */

void loop() {
  int iOldButt1 = 1;
  int iButt1, iButt2;
  long iTime;

  ShowInfo(true);  
  while (1) {
    iButt1 = digitalRead(BUTTON1);
    iButt2 = digitalRead(BUTTON2);
    delay(25); // allow time for debounce
    // Panel selection button
    if (iButt1 == 0 && iOldButt1 == 1) {
      iPanel++;
      if (iPanel >= EP_PANEL_COUNT-1) iPanel = 0;
      epd.setPanelType(iPanel+1);
      ShowInfo(false);
      iTime = millis();
      while (digitalRead(BUTTON1) == 0 && millis() - iTime < 1000) { // if user holds it down, go FAST
         delay(20);
      } // wait for button to be held for 1 second before starting
      while (digitalRead(BUTTON1) == 0) {
         iPanel++;
         if (iPanel >= EP_PANEL_COUNT-1) iPanel = 0;
         ShowInfo(false);
         delay(200); // 5 per second should be fast enough
      } // while button1 held down
    }
    // Mode selection/start button
    if (iButt2 == 0) {
       iTime = millis();
       while (digitalRead(BUTTON2) == 0 && (millis() - iTime < 1000)) { // measure how long button is held
         delay(10);
       }
       iTime = millis() - iTime;
       if (iTime < 1000) {
         iMode++;
         if (iMode >= MODE_COUNT) iMode = 0;
         ShowInfo(false);
       } else {
         // start the operation
         iTime = millis();
         switch (iMode) {
         case MODE_FULL_NO_RAM: // full refresh EPD test without using the back buffer
          case MODE_FULL_RAM:
          case MODE_FAST: // needs backbuffer too
          case MODE_PARTIAL:
            EPDTest(iMode);
            break;
         case MODE_CLEAR:
            EPDClear();
            break;
         } // switch on mode
         ShowTime();
         ShowInfo(true); // repaint the main menu info
       }
    }
    iOldButt1 = iButt1;
  }
} /* loop() */
