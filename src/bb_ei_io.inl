//
// bb_eink I/O functions
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
#ifndef __BB_EI_IO__
#define __BB_EI_IO__
#ifdef ARDUINO
#include <Arduino.h>
#include <SPI.h>

void bbeiInitIO(BBEIDISP *pBBEI, uint32_t u32Speed)
{
    iDCPin, iMOSIPin, iCLKPin, iCSPin, iRSTPin, iBUSYPin
    pinMode(pBBEI->iDCPin, OUTPUT);
    if (pBBEI->iRSTPin != 0xff) {
        pinMode(pBBEI->iRSTPin, OUTPUT);
        digitalWrite(pBBEI->iRSTPin, LOW);
        delay(100);
        digitalWrite(pBBEI->iRSTPin, HIGH);
        delay(1000);
    }
    if (pBBEI->iBUSYPin != 0xff) {
        pinMode(pBBEI->iBUSYPin, INPUT);
    }
    pBBEI->iSpeed = u32Speed;
    SPI.begin(pBBEI->iCLKPin, -1, pBBEI->iMOSIPin, pBBEI->iCSPin);
    SPI.beginTransaction(SPISettings(u32Speed, MSBFIRST, SPI_MODE0));
    SPI.endTransaction(); // N.B. - if you call beginTransaction() again without a matching endTransaction(), it will hang on ESP32
} /* bbeiInitIO() */

void bbeiCMD2(BBEIDISP *pBBEI, uint8_t cmd1, uint8_t cmd2)
{
    digitalWrite(pBBEI->iDCPin, LOW);
    SPI.transfer(cmd1);
    SPI.transfer(cmd2);
    digitalWrite(pBBEI->iDCPin, HIGH); // leave data mode as the default

} /* bbeiCMD2() */

void bbeiWriteCmd(BBEIDISP *pBBEI, uint8_t cmd)
{
    digitalWrite(pBBEI->iDCPin, LOW);
    SPI.transfer(cmd);
    digitalWrite(pBBEI->iDCPin, HIGH); // leave data mode as the default
} /* bbeiWriteCmd() */

void bbeiWriteData(BBEIDISP *pBBEI, uint8_t *pData, int iLen)
{
    SPI.transferBytes(pData, NULL, iLen);
} /* bbeiWriteData() */

#else // LINUX
#endif // ARDUINO
#endif // __BB_EI_IO__
