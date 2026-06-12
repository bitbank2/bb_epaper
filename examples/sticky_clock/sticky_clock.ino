//
// Seeed Studio 'Sticky' clock example
// written by Larry Bank (bitbank@pobox.com)
//
// Select the target device as "ESP32S3 Dev Module"
//
#include <NTPClient.h>           //https://github.com/taranais/NTPClient
#include <WiFi.h>
#include <Wire.h>
#include <esp_wifi.h>
#include <HTTPClient.h>
HTTPClient http;
#include <bb_epaper.h>
#include "Roboto_Black_40.h"
#include "Roboto_Black_140.h"
//
// *** Change these settings for your local WiFi settings and time zone ***
//
const char *ssid = "your_ssid";
const char *password = "your_password";
// Define your timezone offset in seconds from GMT - e.g. GMT-5 = (-5 * 3600)
#define TIME_OFFSET -3600*8

// BQ27220 (battery guage) registers
#define SDA_PIN 1
#define SCL_PIN 0
#define BQ_ADDR 0x55
#define BQ_REG_TEMPERATURE 6
#define BQ_REG_VOLTAGE 8
#define BQ_REG_AVG_CURRENT 20
#define BQ_REG_CHARGE_STATE 44
#define BQ_REG_CURRENT 12
#define BQ_REG_REMAINING_CAP 16
#define BQ_REG_FULL_CAP 18
#define BQ_REG_TIME_TO_EMPTY 22

BBEPAPER bbep;

RTC_DATA_ATTR static int iCount = 0;        // number of wakeups from deep sleep
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Define NTP Client to get time
struct tm myTime;

const char *szMonths[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
const char *szDays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//
// Read a 16-bit I2C register from the BQ27220
//
uint16_t readReg16(uint8_t u8Reg)
{
  uint16_t u16;
  Wire.beginTransmission(BQ_ADDR);
  Wire.write(u8Reg);
  Wire.endTransmission();
  Wire.requestFrom(BQ_ADDR, 2);
  u16 = Wire.read();
  u16 |= (Wire.read() << 8);
  return u16;
} /* readReg16() */
//
// Display the battery level as a percentage
// If > 100%, it will show that it is charging "*CHG*"
//
void DisplayBattery(void)
{
int iPercent;

int iFull, iRemain, iAvgCurrent;
// Initialize I2C and read the charge info from the BQ27220 battery mgmt chip
  Wire.begin(SDA_PIN, SCL_PIN);
  iAvgCurrent = readReg16(BQ_REG_AVG_CURRENT);
  if (iAvgCurrent < 1000) { // positive value means the battery is charging
    iPercent = 110; // show it as currently charging
  } else {
    iFull = readReg16(BQ_REG_FULL_CAP);
    iRemain = readReg16(BQ_REG_REMAINING_CAP);
    iPercent = (iRemain*100)/iFull;
  }
  bbep.setFont(FONT_12x16);
  bbep.setCursor(bbep.width() - 72, bbep.height() - 24); // put text slightly away from the edge
  if (iPercent > 100) {
      bbep.print("*CHG*");
  } else {
     bbep.printf("%d%%", iPercent);
  }
} /* DisplayBattery() */
//
// Display the current time and date in an attractive, bold font
// bPartial = true signifies a non-flickering/partial update
//
void  DisplayTime(bool bPartial)
{
char szTemp[32];
BB_RECT rect;
//
// Clear the current memory plane to white and draw the new time. For partial updates
// the EPD retains the previous memory plane contents so that it can do a
// differential update (only push on the changing pixels)
//
  bbep.fillScreen(BBEP_WHITE);
  bbep.setFont(Roboto_Black_140);
  bbep.setTextColor(BBEP_BLACK, BBEP_WHITE);
  // Get the current time
  struct timeval tv;
  gettimeofday(&tv, NULL);
  localtime_r(&tv.tv_sec, &myTime);
  snprintf(szTemp, sizeof(szTemp), "%02d:%02d", myTime.tm_hour, myTime.tm_min);
  bbep.getStringBox(szTemp, &rect);
  bbep.setCursor((bbep.width() - rect.w)/2, 220); // horizontal center
  bbep.print(szTemp);
  bbep.setFont(Roboto_Black_40);
  snprintf(szTemp, sizeof(szTemp), "%s", szDays[myTime.tm_wday]);
  bbep.getStringBox(szTemp, &rect);
  bbep.setCursor((bbep.width() - rect.w)/2, 340); // horizontal center
  bbep.print(szTemp); // print the day of the week
  snprintf(szTemp, sizeof(szTemp), "%s %d, %d", szMonths[myTime.tm_mon], myTime.tm_mday, myTime.tm_year+1900);
//  bbep.setFont(Roboto_Black_20);
  bbep.getStringBox(szTemp, &rect);
  bbep.setCursor((bbep.width() - rect.w)/2, 420); // horizontal center
  bbep.print(szTemp); // print the day of the week

  DisplayBattery(); // draw the battery % in the lower right corner

  if (bPartial) {
      bbep.writePlane(PLANE_FALSE_DIFF);
  } else {
      bbep.writePlane(PLANE_DUPLICATE);
  }
  bbep.refresh((bPartial) ? REFRESH_PARTIAL : REFRESH_FULL);
  bbep.sleep(LIGHT_SLEEP);
} /* DisplayTime() */

void setup() {
bool res;
int iTimeout;
//
// The Sticky has a 1-bit buffer to hold the battery power on after
// being woken up by the power button. Set it to ON to keep running from
// battery power when not connected to USB power.
//
  pinMode(45, OUTPUT); // power hold (DATA)
  pinMode(46, OUTPUT); // power lock (CLK)
  digitalWrite(45, 1);
  digitalWrite(46, 0);
  digitalWrite(46, 1); // hold battery power enabled

  bbep.begin(EPD_SEEED_STICKY);
  bbep.allocBuffer();
  bbep.fillScreen(BBEP_WHITE);
//
// The iCount variable is stored in RTC RAM which is preserved in deep sleep
// This is used to know how often to use a full update to clear any possible ghosting
//
  if (iCount == 0) { // First time starting from reset - show WiFi status
    bbep.setFont(FONT_12x16);
    bbep.setCursor(0, 16);
    bbep.setTextColor(BBEP_BLACK, BBEP_WHITE);
    bbep.printf("Connecting to WiFi AP: %s\n", ssid);
    bbep.writePlane();
    bbep.refresh(REFRESH_FULL);
  }

  if ((iCount & 1023) == 0) { // sync the internal RTC to NTP time every 17 hours
      WiFi.begin(ssid, password);
      iTimeout = 0;
      while (WiFi.status() != WL_CONNECTED && WiFi.status() != WL_CONNECT_FAILED && iTimeout < 20) {
          delay(500); // allow up to 10 seconds to connect
          iTimeout++;
         // Serial.print(".");
      }
      if (WiFi.status() == WL_CONNECTED) {
          if (iCount == 0) {
            bbep.println("\nConnected!");
            bbep.writePlane();
            bbep.refresh(REFRESH_PARTIAL);
            bbep.fillScreen(BBEP_WHITE);
          }
      } else {
          if (iCount == 0) {
            bbep.println("\nConnection failed!");
            bbep.writePlane();
            bbep.refresh(REFRESH_PARTIAL);
          }
          while (1) {}
      }
// Initialize a NTPClient to get the time
      timeClient.begin();
      timeClient.setTimeOffset(TIME_OFFSET);  //My timezone
      timeClient.update();
      unsigned long epochTime = timeClient.getEpochTime();
      // Get a time structure
      struct tm *ptm = gmtime ((time_t *)&epochTime);
      memcpy(&myTime, ptm, sizeof(myTime)); // get the current time struct into a local copy
      timeval tv;
      tv.tv_sec = epochTime;
      settimeofday(&tv, nullptr); // set internal RTC with the correct time
      timeClient.end(); // don't need it any more
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
  } // needed to sync the time

} /* setup() */
//
// Put the ESP32S3 into deep sleep (lowest power mode)
// Use the ESP32's RTC to wake us up after approximately 1 minute
// This preserves the RTC RAM contents and uses the lowest power of all
// of the sleep modes (~14uA not including any non-sleeping peripherals)
//
void deepSleep(uint64_t time_in_ms)
{
  esp_sleep_enable_timer_wakeup(time_in_ms * 1000L);
  esp_deep_sleep_start();
}

void loop() {
  if ((iCount % 10) == 0) {
    DisplayTime(false); // do a full update every 10 updates
  } else {
    DisplayTime(true); // otherwise do a partial update
  }
  iCount++;
  deepSleep(60000); // sleep for 1 minute
} /* loop() */
