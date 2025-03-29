//
// Minimal Example CO2 Clock Project
//
// Written by Larry Bank
// Copyright (c) 2025 BitBank Software, Inc.
// email: bitbank@pobox.com
//
// Creative Commons license:
// You are free to:
// Share — copy and redistribute the material in any medium or format
// Adapt — remix, transform, and build upon the material for any purpose, even commercially.
//
// The purpose of this example is to show a practical use for bb_epaper and sensor data
// The components required are Sensirion SCD40/41 CO2 sensor and a compatible RTC
// (DS3231, BM8563, RV3032)
//
#include <bb_epaper.h> // https://github.com/bitbank2/bb_epaper
#include <bb_scd41.h> // https://github.com/bitbank2/bb_scd41
#include <bb_rtc.h>  // https://github.com/bitbank2/bb_rtc
// Fonts generated with the bb_epaper font converter from Google Roboto-Black truetype
#include "Roboto_Black_50.h"
#include "Roboto_Black_30.h"
BBRTC rtc;
BBEPAPER epd(EP42B_400x300); // 4.2" 400x300 black/white
SCD41 mySensor;
struct tm myTime;
// GPIO connections for your e-ink panel and I2C bus
#define EPD_MOSI 0
#define EPD_CLK 1
#define EPD_CS 2
#define EPD_DC 3
#define EPD_BSY 4
#define EPD_RST 5
#define SDA_PIN 7
#define SCL_PIN 6

const char *szDays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char *szMonths[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

//
// Show the current time and CO2 concentration
//
void ShowInfo(int iPlane)
{
  char szTemp[32];
  int iCO2;
  int16_t x1, y1;
  uint16_t w, h;

  epd.setPlane(iPlane);
  epd.fillScreen(BBEP_WHITE, iPlane);
  rtc.getTime(&myTime); // Read the current time from the RTC into our time structure
  epd.setFont(Roboto_Black_50);
  sprintf(szTemp, "%02d:%02d", myTime.tm_hour, myTime.tm_min);
  epd.getTextBounds(szTemp, 0, 0, &x1, &y1, &w, &h); // need to center the text
  epd.setCursor((epd.width() - w)/2, 72);
  epd.print(szTemp);
  epd.setFont(Roboto_Black_30);
  epd.getTextBounds(szDays[myTime.tm_wday], 0, 0, &x1, &y1, &w, &h); // need to center the text
  epd.setCursor((epd.width() - w)/2,124);
  epd.print(szDays[myTime.tm_wday]);

  sprintf(szTemp, "%s %02d", szMonths[myTime.tm_mon-1], myTime.tm_mday); //myTime.tm_year-100
  epd.getTextBounds(szTemp, 0, 0, &x1, &y1, &w, &h); // need to center the text
  epd.setCursor((epd.width() - w)/2, 188);
  epd.print(szTemp);
  mySensor.getSample();
  iCO2 = mySensor.co2();
  epd.setFont(Roboto_Black_50);
  sprintf(szTemp, "%d", iCO2);
  epd.setCursor(40, epd.height() - 20);
  epd.print(szTemp);
  epd.setFont(Roboto_Black_30);
  epd.print("ppm");
// future
//    sprintf(szTemp, "Temperature %dC ", mySensor.temperature()/10);
//    sprintf(szTemp, "Humidity %d%%", mySensor.humidity());
} /* ShowInfo() */

void setup() {
  // Initialize bb_epaper with the I/O connections and speed
  epd.initIO(EPD_DC, EPD_RST, EPD_BSY, EPD_CS, EPD_MOSI, EPD_CLK, 8000000);
  // Allocate buffers on the MCU to hold the image data
  epd.allocBuffer();
  // erase to white
  epd.fillScreen(BBEP_WHITE);
  epd.writePlane(); // write the data to the EPD framebuffer
  epd.refresh(REFRESH_FULL); // full (flashing) refresh to start
  // Initialize the realtime clock
  rtc.init(SDA_PIN, SCL_PIN, false, 400000);
  // Initialize the CO2 sensor
  if (mySensor.init(SDA_PIN, SCL_PIN, true, 400000) == SCD41_SUCCESS) {
     mySensor.start(); // start 30-second sampling
  } else { // can't find the sensor, stop
  } // no sensor connected or some error
  rtc.getTime(&myTime); // see if the clock is set
  if (myTime.tm_hour == 0 && myTime.tm_min == 0) { // time is not set, give it a time/date
    myTime.tm_sec = 0;
    myTime.tm_hour = 19;
    myTime.tm_min = 10;
    myTime.tm_year = 2025;
    myTime.tm_mon = 3;
    myTime.tm_mday = 29;
    myTime.tm_wday = 6;
    rtc.setTime(&myTime);
  }
} /* setup() */

void loop() {
int iPlane = 0;
int iCount = 0;

// Start by writing the data to both planes and do a full refresh
  epd.setTextColor(BBEP_BLACK, BBEP_WHITE);
  ShowInfo(PLANE_DUPLICATE);
  epd.writePlane(PLANE_DUPLICATE);
  epd.refresh(REFRESH_FULL); // start with a full refresh
  // Loop forever
  while (1) {
    delay(60000); // update once a minute
    epd.backupPlane(); // copy last data to "old plane"
    ShowInfo(0); // update plane 0 (with new data)
    epd.writePlane(PLANE_BOTH); // UC8151 needs both planes written to do partial updates
    // Do 15 partial (non-flashing) updates followed by a full update to remove ghosting
    epd.refresh(((iCount & 15) == 0) ? REFRESH_FULL : REFRESH_PARTIAL);
    epd.wait();
    epd.sleep(LIGHT_SLEEP); // extends the life of the panel to sleep it between updates
    iCount++;
  } // while (1)
} /* loop() */
