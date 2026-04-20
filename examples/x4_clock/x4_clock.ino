//
// XTEINK X3/X4 clock example
// written by Larry Bank (bitbank@pobox.com)
//
// N.B. For maximum battery life, remove the uSD card
// Select the target device as "ESP32C3 Dev Module"
// Under the Arduino Tools menu, select the Flash mode to "DIO"
// (default is QIO and choosing this will cause a crashing boot loop)
//
// Define this if using the X3, otherwise it will build for the X4
//#define HARDWARE_X3
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
// Change for your local WiFi settings and time zone
//
const char *ssid = "BELL452";
const char *password = "1F6D44DF953E";
// Define your timezone offset in seconds from GMT - e.g. GMT-5 = (-5 * 3600)
#define TIME_OFFSET -(3600 * 4)

// BQ27220 registers (X3 only)
#define BQ_ADDR 0x55
#define BQ_REG_TEMPERATURE 6
#define BQ_REG_VOLTAGE 8
#define BQ_REG_AVG_CURRENT 20
#define BQ_REG_CHARGE_STATE 44
#define BQ_REG_CURRENT 12
#define BQ_REG_REMAINING_CAP 16
#define BQ_REG_FULL_CAP 18
#define BQ_REG_TIME_TO_EMPTY 22

// X4 has a voltage divider on GPIO 0
#define PIN_BATTERY 0

BBEPAPER bbep;

RTC_DATA_ATTR static int iCount = 0;        // number of wakeups from deep sleep
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Define NTP Client to get time
struct tm myTime;

const char *szMonths[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
const char *szDays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

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

// Display the battery level
void DisplayBattery(void)
{
int iPercent;

#ifdef HARDWARE_X3
int iFull, iRemain, iAvgCurrent;
// Initialize I2C and read the charge info from the BQ27220 battery mgmt chip
  Wire.begin(20, 0);
  iAvgCurrent = readReg16(BQ_REG_AVG_CURRENT);
  if (iAvgCurrent < 1000) { // positive value means the battery is charging
    iPercent = 110; // show it as currently charging
  } else {
    iFull = readReg16(BQ_REG_FULL_CAP);
    iRemain = readReg16(BQ_REG_REMAINING_CAP);
    iPercent = (iRemain*100)/iFull;
  }
#else
// The X4 has a 50/50 voltage divider connected to GPIO 0
int32_t adc;
int iVoltage;

// Read the value from the ADC on GPIO 0, then double it to get the battery voltage
// When converted to a battery remaining percent value, this will show a
// non-linear discharge curve, but it's better than nothing
  adc = 0;
  analogRead(PIN_BATTERY); // This is needed to properly initialize the ADC BEFORE calling analogReadMilliVolts()
  for (uint8_t i = 0; i < 8; i++) {
      adc += analogReadMilliVolts(PIN_BATTERY);
  }
  iVoltage = (adc / 8) * 2;
  iPercent = (4200 * 100)/iVoltage;
#endif
  bbep.setFont(FONT_12x16);
  bbep.setCursor(bbep.width() - 72, bbep.height() - 24); // put text slightly away from the edge
#ifndef HARDWARE_X3
  if (iPercent > 100) { // X4 charging?
     pinMode(20, INPUT); // USB power present?
     if (digitalRead(20) == 0) { // no, just high battery charge
        iPercent = 100;
     }
  }
#endif
  if (iPercent > 100) {
      bbep.print("*CHG*");
  } else {
     bbep.printf("%d%%", iPercent);
  }
} /* DisplayBattery() */

void  DisplayTime(bool bPartial)
{
char szTemp[32];
BB_RECT rect;

  bbep.setFont(Roboto_Black_140);
  bbep.setTextColor(BBEP_BLACK, BBEP_WHITE);
  // Get the current time
  struct timeval tv;
  gettimeofday(&tv, NULL);
  localtime_r(&tv.tv_sec, &myTime);
  sprintf(szTemp, "%02d:%02d", myTime.tm_hour, myTime.tm_min);
  bbep.getStringBox(szTemp, &rect);
  bbep.setCursor((bbep.width() - rect.w)/2, 220); // horizontal center
  bbep.print(szTemp);
  bbep.setFont(Roboto_Black_40);
  sprintf(szTemp, "%s", szDays[myTime.tm_wday]);
  bbep.getStringBox(szTemp, &rect);
  bbep.setCursor((bbep.width() - rect.w)/2, 340); // horizontal center
  bbep.print(szTemp); // print the day of the week
  sprintf(szTemp, "%s %d, %d", szMonths[myTime.tm_mon], myTime.tm_mday, myTime.tm_year+1900);
//  bbep.setFont(Roboto_Black_20);
  bbep.getStringBox(szTemp, &rect);
  bbep.setCursor((bbep.width() - rect.w)/2, 420); // horizontal center
  bbep.print(szTemp); // print the day of the week

  DisplayBattery(); // draw the battery % in the lower right corner

  if (bPartial) {
      bbep.writePlane(PLANE_FALSE_DIFF);
  } else {
      bbep.writePlane();
  }
  bbep.refresh((bPartial) ? REFRESH_PARTIAL : REFRESH_FAST);
  bbep.sleep(LIGHT_SLEEP);
} /* DisplayTime() */

void setup() {
bool res;
int iTimeout;

  // Enable the correct line for your device
#ifdef HARDWARE_X3
  bbep.begin(EPD_XTEINK_X3);
#else
  bbep.begin(EPD_XTEINK_X4);
#endif
  bbep.allocBuffer();
  bbep.fillScreen(BBEP_WHITE);

  if (iCount == 0) { // First time starting from reset - show WiFi status
    bbep.setFont(FONT_12x16);
    bbep.setCursor(0, 16);
    bbep.setTextColor(BBEP_BLACK, BBEP_WHITE);
    bbep.printf("Connecting to WiFi AP: %s\n", ssid);
    bbep.writePlane();
    bbep.refresh(REFRESH_FAST);
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
// Initialize a NTPClient to get time
      timeClient.begin();
      timeClient.setTimeOffset(TIME_OFFSET);  //My timezone
      timeClient.update();
      unsigned long epochTime = timeClient.getEpochTime();
      //Get a time structure
      struct tm *ptm = gmtime ((time_t *)&epochTime);
      memcpy(&myTime, ptm, sizeof(myTime)); // get the current time struct into a local copy
      timeval tv;
      tv.tv_sec = epochTime;
      settimeofday(&tv, nullptr); // set internal RTC with the correct time
      timeClient.end(); // don't need it any more
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
  } // need to sync the time

} /* setup() */

void deepSleep(uint64_t time_in_ms)
{
  esp_sleep_enable_timer_wakeup(time_in_ms * 1000L);
  // Need to keep the power on during deep sleep
#ifdef HARDWARE_X3
  pinMode(13, OUTPUT); // LDO power enable
  digitalWrite(13, HIGH);
  gpio_hold_en(GPIO_NUM_5); // EPD reset line
  gpio_hold_en(GPIO_NUM_13); // MOSFET enabling the battery power
#else
  esp_deep_sleep_enable_gpio_wakeup(1ULL << 3, ESP_GPIO_WAKEUP_GPIO_LOW);  // Power button pin
#endif
//  gpio_hold_en(GPIO_NUM_5); // hold EPD reset high in deep sleep
  gpio_deep_sleep_hold_en(); // needed to keep the X3/X4's LDO enabled & EPD reset high
  esp_deep_sleep_start();
}

void loop() {
  if ((iCount % 10) == 0) {
    DisplayTime(false); // do a fast update every 10 updates
  } else {
    DisplayTime(true); // otherwise do a partial update
  }
  iCount++;
  deepSleep(60000); // sleep for 1 minute
} /* loop() */
