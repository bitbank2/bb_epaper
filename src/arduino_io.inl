//
// bb_epaper I/O wrapper functions for Arduino
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
#ifndef __BB_EP_IO__
#define __BB_EP_IO__

#include <Arduino.h>
#include <SPI.h>
//
// Initialize the GPIO pins and SPI for use by bb_eink
//
void bbepInitIO(BBEPDISP *pBBEP, uint32_t u32Speed)
{
    pinMode(pBBEP->iDCPin, OUTPUT);
    pinMode(pBBEP->iRSTPin, OUTPUT);
    digitalWrite(pBBEP->iRSTPin, LOW);
    delay(100);
    digitalWrite(pBBEP->iRSTPin, HIGH);
    delay(100);
    if (pBBEP->iBUSYPin != 0xff) {
        pinMode(pBBEP->iBUSYPin, INPUT);
    }
    pBBEP->iSpeed = u32Speed;
#ifdef ARDUINO_ARCH_ESP32
    SPI.begin(pBBEP->iCLKPin, -1, pBBEP->iMOSIPin, pBBEP->iCSPin);
#else
    pinMode(pBBEP->iCSPin, OUTPUT);
    digitalWrite(pBBEP->iCSPin, HIGH); // we have to manually control the CS pin
    SPI.begin(); // other architectures have fixed SPI pins
#endif
    SPI.beginTransaction(SPISettings(u32Speed, MSBFIRST, SPI_MODE0));
    SPI.endTransaction(); // N.B. - if you call beginTransaction() again without a matching endTransaction(), it will hang on ESP32
} /* bbepInitIO() */
//
// Toggle the reset line to wake up the eink from deep sleep
//
void bbepWakeUp(BBEPDISP *pBBEP)
{
    digitalWrite(pBBEP->iRSTPin, LOW);
    delay(10);
    digitalWrite(pBBEP->iRSTPin, HIGH);
    delay(10);
} /* bbepWakeUp() */
//
// Wait for the busy status line to show idle
// The polarity of the busy signal is reversed on the UC81xx compared
// to the SSD16xx controllers
//
void bbepWaitBusy(BBEPDISP *pBBEP)
{
    uint8_t busy_idle =  (pBBEP->chip_type == BBEP_CHIP_UC81xx) ? HIGH : LOW;
    delay(1); // some panels need a short delay before testing the BUSY line
    while (1) {
        if (digitalRead(pBBEP->iBUSYPin) == busy_idle) break;
    }
} /* bbepWaitBusy() */
//
// Convenience function to write a command byte along with a data
// byte (it's single parameter)
//
void bbepCMD2(BBEPDISP *pBBEP, uint8_t cmd1, uint8_t cmd2)
{
    if (!pBBEP->is_awake) {
        // if it's asleep, it can't receive commands
        bbepWakeUp(pBBEP);
        pBBEP->is_awake = 1;
    }
    digitalWrite(pBBEP->iDCPin, LOW);
#ifndef ARDUINO_ARCH_ESP32
    digitalWrite(pBBEP->iCSPin, LOW);
#endif
    SPI.transfer(cmd1);
    digitalWrite(pBBEP->iDCPin, HIGH);
    SPI.transfer(cmd2); // second byte is data
#ifndef ARDUINO_ARCH_ESP32
    digitalWrite(pBBEP->iCSPin, HIGH);
#endif
//    digitalWrite(pBBEP->iDCPin, HIGH); // leave data mode as the default
} /* bbepCMD2() */
//
// Write a single byte as a COMMAND (D/C set low)
//
void bbepWriteCmd(BBEPDISP *pBBEP, uint8_t cmd)
{
    if (!pBBEP->is_awake) {
        // if it's asleep, it can't receive commands
        bbepWakeUp(pBBEP);
        pBBEP->is_awake = 1;
    }
    digitalWrite(pBBEP->iDCPin, LOW);
#ifndef ARDUINO_ARCH_ESP32
    digitalWrite(pBBEP->iCSPin, LOW);
#endif
    SPI.transfer(cmd);
#ifndef ARDUINO_ARCH_ESP32
    digitalWrite(pBBEP->iCSPin, HIGH);
#endif
    digitalWrite(pBBEP->iDCPin, HIGH); // leave data mode as the default
} /* bbepWriteCmd() */
//
// Write 1 or more bytes as DATA (D/C set high)
//
void bbepWriteData(BBEPDISP *pBBEP, uint8_t *pData, int iLen)
{
//    digitalWrite(pBBEP->iDCPin, HIGH);
#ifdef ARDUINO_ARCH_ESP32
    SPI.transferBytes(pData, NULL, iLen);
#else
    digitalWrite(pBBEP->iCSPin, LOW);
    SPI.transfer(pData, iLen);
    digitalWrite(pBBEP->iCSPin, HIGH);
#endif
} /* bbepWriteData() */

#endif // __BB_EP_IO__
