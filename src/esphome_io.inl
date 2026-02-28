//
// bb_epaper I/O wrapper functions for ESPHome
// Written by Larry Bank (bitbank@pobox.com)
// Project started 1/1/2026
//
// SPDX-FileCopyrightText: 2026 BitBank Software, Inc.
// SPDX-License-Identifier: GPL-3.0-or-later
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef __ESPHOME_IO__
#define __ESPHOME_IO__

// foreward references
void bbepWakeUp(BBEPDISP *pBBEP);
void bbepSendCMDSequence(BBEPDISP *pBBEP, const uint8_t *pSeq);
//
// Set the second CS pin for dual-controller displays
//
void bbepSetCS2(BBEPDISP *pBBEP, uint8_t cs)
{
    pBBEP->iCS1Pin = pBBEP->iCSPin;
    pBBEP->iCS2Pin = cs;
    pinMode(cs, OUTPUT);
    (*pBBEP->pfnSetGPIO)(cs, HIGH); // disable second CS for now
} /* bbepSetCS2() */

void SPI_Write(BBEPDISP *pBBEP, uint8_t *pData, int iLen)
{
int i, j;
uint8_t uc;

    for (i=0; i<iLen; i++) {
        uc = *pData++;
        for (j=0; j<8; j++) {
            (*pBBEP->pfnSetGPIO)(pBBEP->iCLKPin, 0);
            (*pBBEP->pfnSetGPIO)(pBBEP->iMOSIPin, uc & 0x80);
            (*pBBEP->pfnSetGPIO)(pBBEP->iCLKPin, 1);
            uc <<= 1;
        }
    }
} /* SPI_Write() */
//
// Initialize the GPIO pins and SPI for use by bb_eink
//
void bbepInitIO(BBEPDISP *pBBEP, uint8_t u8DC, uint8_t u8RST, uint8_t u8BUSY, uint8_t u8CS, uint8_t u8MOSI, uint8_t u8SCK, uint32_t u32Speed)
{
    pBBEP->iDCPin = u8DC;
    pBBEP->iCSPin = u8CS;
    pBBEP->iMOSIPin = u8MOSI;
    pBBEP->iCLKPin = u8SCK;
    pBBEP->iRSTPin = u8RST;
    pBBEP->iBUSYPin = u8BUSY;

// If we're here it's because the begin() method is being called
// we need to set up the I/O using the Arduino API

    pinMode(u8DC, OUTPUT);
    pinMode(u8CS, OUTPUT);
    pinMode(u8BUSY, INPUT);
    pinMode(u8RST, OUTPUT);

    if (pBBEP->iRSTPin != 0xff) {
        (*pBBEP->pfnSetGPIO)(pBBEP->iRSTPin, LOW);
        delay(100);
        (*pBBEP->pfnSetGPIO)(pBBEP->iRSTPin, HIGH);
        delay(100);
    }
    pBBEP->iSpeed = u32Speed;
    (*pBBEP->pfnSetGPIO)(pBBEP->iCSPin, HIGH); // manually control the CS pin
    SPI.begin(u8SCK, -1, u8MOSI, -1);
    SPI.beginTransaction(SPISettings(u32Speed, MSBFIRST, SPI_MODE0));
    SPI.endTransaction(); // we just needed to set the speed

    pBBEP->is_awake = 1;
// Before we can start sending pixels, many panels need to know the display resolution
    bbepSendCMDSequence(pBBEP, pBBEP->pInitFull);
    if (pBBEP->iFlags & BBEP_7COLOR) { // need to send before you can send it data
        if (pBBEP->iFlags & BBEP_SPLIT_BUFFER) {    
           // Send the same sequence to the second controller
           pBBEP->iCSPin = pBBEP->iCS2Pin;
           bbepSendCMDSequence(pBBEP, pBBEP->pInitFull);
           pBBEP->iCSPin = pBBEP->iCS1Pin;
        }
    }
} /* bbepInitIO() */
//
// Write a single byte as a COMMAND (D/C set low)
//
void bbepWriteCmd(BBEPDISP *pBBEP, uint8_t cmd)
{
//Serial.printf("CMD: 0x%02x\n", cmd);

    if (!pBBEP->is_awake) {
        // if it's asleep, it can't receive commands
        bbepWakeUp(pBBEP);
        pBBEP->is_awake = 1;
    }
    (*pBBEP->pfnSetGPIO)(pBBEP->iDCPin, LOW);
    delay(1);
    (*pBBEP->pfnSetGPIO)(pBBEP->iCSPin, LOW);
    if (pBBEP->iSpeed == 0) { // bit bang
        SPI_Write(pBBEP, &cmd, 1);
    } else {
        (*pBBEP->pfnWrite)(&cmd, 1);
    }
    (*pBBEP->pfnSetGPIO)(pBBEP->iCSPin, HIGH);
    (*pBBEP->pfnSetGPIO)(pBBEP->iDCPin, HIGH); // leave data mode as the default
} /* bbepWriteCmd() */
//
// Write 1 or more bytes as DATA (D/C set high)
//
void bbepWriteData(BBEPDISP *pBBEP, uint8_t *pData, int iLen)
{
    if (pBBEP->iFlags & BBEP_CS_EVERY_BYTE) {
        for (int i=0; i<iLen; i++) { // Arduino clobbers the data (duplex)
            (*pBBEP->pfnSetGPIO)(pBBEP->iCSPin, LOW);
            if (pBBEP->iSpeed == 0) { // bit bang
                SPI_Write(pBBEP, &pData[i], 1);
            } else {
                (*pBBEP->pfnWrite)(&pData[i], 1);
            }
            (*pBBEP->pfnSetGPIO)(pBBEP->iCSPin, HIGH);
        }
    } else {
        (*pBBEP->pfnSetGPIO)(pBBEP->iCSPin, LOW);
        if (pBBEP->iSpeed == 0) { // bit bang
            SPI_Write(pBBEP, pData, iLen);
        } else {
            (*pBBEP->pfnWrite)(pData, iLen);
        }
        (*pBBEP->pfnSetGPIO)(pBBEP->iCSPin, HIGH);
    }
} /* bbepWriteData() */

//
// Convenience function to write a command byte along with a data
// byte (it's single parameter)
//
void bbepCMD2(BBEPDISP *pBBEP, uint8_t cmd1, uint8_t cmd2)
{
    bbepWriteCmd(pBBEP, cmd1);
    bbepWriteData(pBBEP, &cmd2, 1);
} /* bbepCMD2() */

#endif // __ESPHOME_IO__
