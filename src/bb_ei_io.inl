//
// bb_eink I/O wrapper functions for Arduino
// Copyright (c) 2024 BitBank Software, Inc.
// Written by Larry Bank (bitbank@pobox.com)
// Project started 9/11/2024
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Adapt these functions to whatever target platform you're using
// and the rest of the code can remain unchanged
//
#ifndef __BB_EI_IO__
#define __BB_EI_IO__
#ifdef ARDUINO
#include <Arduino.h>
#include <SPI.h>
//
// Initialize the GPIO pins and SPI for use by bb_eink
//
void bbeiInitIO(BBEIDISP *pBBEI, uint32_t u32Speed)
{
    pinMode(pBBEI->iDCPin, OUTPUT);
    pinMode(pBBEI->iRSTPin, OUTPUT);
    digitalWrite(pBBEI->iRSTPin, LOW);
    delay(100);
    digitalWrite(pBBEI->iRSTPin, HIGH);
    delay(100);
    if (pBBEI->iBUSYPin != 0xff) {
        pinMode(pBBEI->iBUSYPin, INPUT);
    }
    pBBEI->iSpeed = u32Speed;
#ifdef ARDUINO_ARCH_ESP32
    SPI.begin(pBBEI->iCLKPin, -1, pBBEI->iMOSIPin, pBBEI->iCSPin);
#else
    pinMode(pBBEI->iCSPin, OUTPUT);
    digitalWrite(pBBEI->iCSPin, HIGH); // we have to manually control the CS pin
    SPI.begin(); // other architectures have fixed SPI pins
#endif
    SPI.beginTransaction(SPISettings(u32Speed, MSBFIRST, SPI_MODE0));
    SPI.endTransaction(); // N.B. - if you call beginTransaction() again without a matching endTransaction(), it will hang on ESP32
} /* bbeiInitIO() */
//
// Toggle the reset line to wake up the eink from deep sleep
//
void bbeiWakeUp(BBEIDISP *pBBEI)
{
    digitalWrite(pBBEI->iRSTPin, LOW);
    delay(10);
    digitalWrite(pBBEI->iRSTPin, HIGH);
    delay(10);
} /* bbeiWakeUp() */
//
// Wait for the busy status line to show idle
// The polarity of the busy signal is reversed on the UC81xx compared
// to the SSD16xx controllers
//
void bbeiWaitBusy(BBEIDISP *pBBEI)
{
    uint8_t busy_idle =  (pBBEI->chip_type == BBEI_CHIP_UC81xx) ? HIGH : LOW;
    delay(1); // some panels need a short delay before testing the BUSY line
    while (1) {
        if (digitalRead(pBBEI->iBUSYPin) == busy_idle) break;
    }
} /* bbeiWaitBusy() */
//
// Convenience function to write a command byte along with a data
// byte (it's single parameter)
//
void bbeiCMD2(BBEIDISP *pBBEI, uint8_t cmd1, uint8_t cmd2)
{
    if (!pBBEI->is_awake) {
        // if it's asleep, it can't receive commands
        bbeiWakeUp(pBBEI);
        pBBEI->is_awake = 1;
    }
    digitalWrite(pBBEI->iDCPin, LOW);
#ifndef ARDUINO_ARCH_ESP32
    digitalWrite(pBBEI->iCSPin, LOW);
#endif
    SPI.transfer(cmd1);
    digitalWrite(pBBEI->iDCPin, HIGH);
    SPI.transfer(cmd2); // second byte is data
#ifndef ARDUINO_ARCH_ESP32
    digitalWrite(pBBEI->iCSPin, HIGH);
#endif
//    digitalWrite(pBBEI->iDCPin, HIGH); // leave data mode as the default
} /* bbeiCMD2() */
//
// Write a single byte as a COMMAND (D/C set low)
//
void bbeiWriteCmd(BBEIDISP *pBBEI, uint8_t cmd)
{
    if (!pBBEI->is_awake) {
        // if it's asleep, it can't receive commands
        bbeiWakeUp(pBBEI);
        pBBEI->is_awake = 1;
    }
    digitalWrite(pBBEI->iDCPin, LOW);
#ifndef ARDUINO_ARCH_ESP32
    digitalWrite(pBBEI->iCSPin, LOW);
#endif
    SPI.transfer(cmd);
#ifndef ARDUINO_ARCH_ESP32
    digitalWrite(pBBEI->iCSPin, HIGH);
#endif
    digitalWrite(pBBEI->iDCPin, HIGH); // leave data mode as the default
} /* bbeiWriteCmd() */
//
// Put the eink into light or deep sleep
//
void bbeiSleep(BBEIDISP *pBBEI, int bDeep)
{
    if (pBBEI->chip_type == BBEI_CHIP_UC81xx) {
        bbeiCMD2(pBBEI, UC8151_CDI, 0x17); // border floating
        bbeiWriteCmd(pBBEI, UC8151_POFF); // power off
        bbeiWaitBusy(pBBEI);
        if (bDeep) {
            bbeiCMD2(pBBEI, UC8151_DSLP, 0xa5); // deep sleep
        }
    } else {
        bbeiCMD2(pBBEI, SSD1608_DEEP_SLEEP, (bDeep) ? 0x02 : 0x01); // deep sleep mode 1 keeps RAM, mode 2 loses RAM
    }
    pBBEI->is_awake = 0;
} /* bbeiSleep() */
//
// Write 1 or more bytes as DATA (D/C set high)
//
void bbeiWriteData(BBEIDISP *pBBEI, uint8_t *pData, int iLen)
{
//    digitalWrite(pBBEI->iDCPin, HIGH);
#ifdef ARDUINO_ARCH_ESP32
    SPI.transferBytes(pData, NULL, iLen);
#else
    digitalWrite(pBBEI->iCSPin, LOW);
    SPI.transfer(pData, iLen);
    digitalWrite(pBBEI->iCSPin, HIGH);
#endif
} /* bbeiWriteData() */

#else // LINUX
#endif // ARDUINO / LINUX
#endif // __BB_EI_IO__
