//
// XTEINK X4 clock example
// written by Larry Bank (bitbank@pobox.com)
//
#include <NTPClient.h>           //https://github.com/taranais/NTPClient
#include <WiFi.h>
#include <esp_wifi.h>
#include <HTTPClient.h>
HTTPClient http;
#include <bb_epaper.h>
#include "Roboto_Black_40.h"
#include "Roboto_Black_140.h"
//
// Change for your local WiFi settings and time zone
//
const char *ssid = "your_ssid";
const char *password = "your_pw";
// Define your timezone offset in seconds from GMT - e.g. GMT-5 = (-5 * 3600)
#define TIME_OFFSET 3600

BBEPAPER bbep;

RTC_DATA_ATTR static int iCount = 0;        // number of wakeups from deep sleep
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Define NTP Client to get time
struct tm myTime;

const char *szMonths[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
const char *szDays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

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

  if ((iCount & 1023) == 0) { // sync the internal RTC to NTP time every 17 hours
      Serial.begin(115200);
      delay(3000);
      Serial.println("Connecting to WiFi");
      WiFi.begin(ssid, password);
      iTimeout = 0;
      while (WiFi.status() != WL_CONNECTED && WiFi.status() != WL_CONNECT_FAILED && iTimeout < 20) {
          delay(500); // allow up to 10 seconds to connect
          iTimeout++;
          Serial.print(".");
      }
      if (WiFi.status() == WL_CONNECTED) {
          Serial.println("\nConnected!");
      } else {
          Serial.println("\nConnection failed!");
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
  bbep.begin(EPD_XTEINK_X4);
  bbep.allocBuffer();
  bbep.fillScreen(BBEP_WHITE);
} /* setup() */

void lightSleep(uint64_t time_in_ms)
{
  esp_sleep_enable_timer_wakeup(time_in_ms * 1000L);
  esp_light_sleep_start();
}

void deepSleep(uint64_t time_in_ms)
{
//  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF); // hibernation mode - only RTC powered
//  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
//  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
//  esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL,         ESP_PD_OPTION_OFF);
  esp_sleep_enable_timer_wakeup(time_in_ms * 1000L);
  esp_deep_sleep_enable_gpio_wakeup(1ULL << 3, ESP_GPIO_WAKEUP_GPIO_LOW);  // Power button pin
  esp_deep_sleep_start();
}

void loop() {
  if ((iCount % 10) == 0) {
    DisplayTime(false); // do a fast update every 10 updates
  } else {
    DisplayTime(true); // otherwise do a partial update
  }
  iCount++;
  lightSleep(60000);
  //deepSleep(60000); // sleep for 1 minute
} /* loop() */
