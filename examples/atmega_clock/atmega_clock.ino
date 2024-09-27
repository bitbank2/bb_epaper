//
// epaper clock on the humble ATmega328
//
#include <Wire.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/boot.h>
#include <avr/wdt.h>

#include "Roboto_Black_38.h"
#include "Roboto_Black_78.h"
#include <bb_epaper.h>
BBEPAPER bbep(EPD42_400x300);
static int iRunCount = 0;
static int iSec, iMin, iHour, iWDay, iMDay, iMon;
int iSleepTime; // number of microseconds to sleep in each wakeup phase

// I2C base address of various RTCs
#define RTC_DS3231_ADDR 0x68
#define RTC_RV3032_ADDR 0x51
#define RTC_PCF8563_ADDR 0x51
#define RTC_ADDR RTC_DS3231_ADDR
#define RTC_START_REG 0
// Ugly solder job of Xiao ESP32-C6 and my e-paper adapter
#define PIN_SDA 4
#define PIN_SCL 3
#define POWER_PIN 5
#define PIN_DC 14
#define PIN_BUSY 16
#define PIN_RST 15

const char *szDays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char *szMonths[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
// AVR Power savings
// watchdog interrupt
ISR (WDT_vect) 
{        
   wdt_disable();  // disable watchdog
}  // end of WDT_vect
//
// Use the watchdog timer to sleep for a fixed time and then wake up
// 16ms delay shifted left by teh shift value
//
void WDSleepOneSec(void)
{
  ADCSRA = 0; // Disable ADC  
  // clear various "reset" flags
  MCUSR = 0;
  // allow changes, disable reset
  WDTCSR = bit (WDCE) | bit (WDE); // allow changes, disable reset
  /**
	 *	Setting the watchdog pre-scaler value with VCC = 5.0V and 16mHZ
	 *	WDP3 WDP2 WDP1 WDP0 | Number of WDT | Typical Time-out at Oscillator Cycles
	 *	0    0    0    0    |   2K cycles   | 16 ms
	 *	0    0    0    1    |   4K cycles   | 32 ms
	 *	0    0    1    0    |   8K cycles   | 64 ms
	 *	0    0    1    1    |  16K cycles   | 0.125 s
	 *	0    1    0    0    |  32K cycles   | 0.25 s
	 *	0    1    0    1    |  64K cycles   | 0.5 s
	 *	0    1    1    0    |  128K cycles  | 1.0 s
	 *	0    1    1    1    |  256K cycles  | 2.0 s
	 *	1    0    0    0    |  512K cycles  | 4.0 s
	 *	1    0    0    1    | 1024K cycles  | 8.0 s
	*/
  // For some reason, the AVR compiler is broken and these must be done as compile-time
  // constants. Experiments with building a mask from individual bits ended in failure
	WDTCSR  = (1<<WDIE) | (0<<WDP3) | (1<<WDP2) | (1<<WDP1) | (0<<WDP0);
	wdt_reset();  // pat the dog
	
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  noInterrupts ();           // timed sequence follows
  sleep_enable();

  // turn off brown-out enable in software
  MCUCR = bit (BODS) | bit (BODSE);
  MCUCR = bit (BODS);
  interrupts ();             // guarantees next instruction executed
  sleep_cpu ();

  // cancel sleep as a precaution
  sleep_disable();
  Serial.println("woke up!");
} /* WDSleepOneSec() */

int I2CWrite(unsigned char iAddr, unsigned char *pData, int iLen)
{
    Wire.beginTransmission(iAddr);
    Wire.write(pData, (unsigned char)iLen);
    Wire.endTransmission();
}
int I2CReadRegister(unsigned char iAddr, unsigned char u8Register, unsigned char *pData, int iLen)
{
  int rc;
  int i = 0;

  Wire.beginTransmission(iAddr);
  Wire.write(u8Register);
  Wire.endTransmission();
  Wire.requestFrom(iAddr, (unsigned char)iLen);
  while (i < iLen) {
     pData[i++] = Wire.read();
  }
}
//
// Minimal code to read the time and create
// a formatted ASCII string from the hh:mm:ss
//
void rtc_get_time()
{
uint8_t ucTemp[8];

      I2CReadRegister(RTC_ADDR, RTC_START_REG, ucTemp, 7);
      // Convert values from BCD
      iSec  = ((ucTemp[0] >> 4) * 10) + (ucTemp[0] & 0xf);
      iMin  = ((ucTemp[1] >> 4) * 10) + (ucTemp[1] & 0xf);
      // hours are stored in 24-hour format in the tm struct
      if (ucTemp[2] & 64) { // 12 hour format
          iHour = ucTemp[2] & 0xf;
          iHour += ((ucTemp[2] >> 4) & 1) * 10;
          iHour += ((ucTemp[2] >> 5) & 1) * 12; // AM/PM
      } else { // 24 hour format
          iHour = ((ucTemp[2] >> 4) * 10) + (ucTemp[2] & 0xf);
      }
      iWDay = ucTemp[3] - 1; // day of the week 0-6
      iMDay = ucTemp[4] & 0xf;
      iMDay += ((ucTemp[4] >> 4) * 10);
      iMon = ucTemp[5] & 0xf;
      iMon += (((ucTemp[5] >> 4) & 1) * 10);
      iMon--; // returns 1-12 instead of 0-11
} /* rtc_get_time() */

void DrawPlane(int bInvert, int iPlane, char *string1, char *string2)
{
  int iFG, iBG;

    if (bInvert) {
      iFG = BBEP_WHITE; iBG = BBEP_BLACK;
    } else {
      iFG = BBEP_BLACK; iBG = BBEP_WHITE;
    }
    bbep.setPlane(iPlane);
    bbep.fillRect(0,0, bbep.width(), bbep.height(), iBG); // clear both buffers to white for full update
    bbep.setFont(Roboto_Black_78);
    bbep.setTextColor(iFG, iBG);
    bbep.drawString(string1, CENTER_X, 120);
    bbep.setFont(Roboto_Black_38);
    bbep.drawString(szDays[iWDay], CENTER_X, 200);
    bbep.drawString(string2, CENTER_X, 280);
}

void setup()
{
 // Serial.begin(115200);
 // Serial.println("Starting...");
 Wire.begin();
 Wire.setClock(400000);
} /* setup() */

void loop()
{
    char szTemp[32] ,szTemp2[32];
    int bPartial, iSleepTime, bInvert;
    
    bbep.initIO(PIN_DC, PIN_RST, PIN_BUSY);
    while (1) {
      bInvert = 1;
      if ((iRunCount & 1) == 0) { // get time and update
          bPartial = ((iRunCount & 0x1f) != 0);
         rtc_get_time();
//         EPDInit((bPartial) ? MODE_PARTIAL : MODE_NORMAL);
        if (bbep.getChip() == BBEP_CHIP_UC81xx) {
        // On the UC81xx we have to explicitly write the old data and new for a partial update
        // the SSD16xx remembers the old properly and just needs to know the new
          if (bPartial) { // inverted too, with the old time strings
              DrawPlane(bInvert, PLANE_1, szTemp, szTemp2);
              bInvert = 0;
          }
        } // BBEP_CHIP_UC81xx
        // Print the current hour:minute
        sprintf(szTemp, "%02d:%02d", iHour, iMin);
        sprintf(szTemp2, "%s %d", szMonths[iMon], iMDay);

        if (!bPartial) {
            DrawPlane(bInvert, PLANE_1, szTemp, szTemp2);
        }
        DrawPlane(bInvert, PLANE_0, szTemp, szTemp2);
        bbep.refresh((bPartial) ? REFRESH_PARTIAL : REFRESH_FULL, false); // do a full refresh
//        iSleepTime = (bPartial) ? 750 : 4000; // sleep instead of watching the busy signal
        iSleepTime = (bPartial) ? 1 : 4; // sleep instead of watching the busy signal
    } else { // we're waking up after waiting for the update to finish
        bbep.sleep(1); // deep sleep
        iSleepTime = 59; // wake up in 59 seconds
    }
    //EPDDeInit(); // set GPIO to disconnected for lower power
    iRunCount++;
    if (!bPartial && iSleepTime == 59) iSleepTime = 56; // we already slept for 4 seconds
    for (int j=0; j<iSleepTime; j++) {
      WDSleepOneSec();
    }
    } // while (1)
} /* loop() */
