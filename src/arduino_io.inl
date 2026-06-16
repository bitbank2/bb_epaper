//
// bb_epaper I/O wrapper functions for Arduino
// Written by Larry Bank (bitbank@pobox.com)
// Project started 9/11/2024
//
// SPDX-FileCopyrightText: 2024 BitBank Software, Inc.
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
#ifndef __ARDUINO_IO__
#define __ARDUINO_IO__

#include <Arduino.h>
#include <SPI.h>
// foreward references
void bbepWakeUp(BBEPDISP *pBBEP);
void bbepSendCMDSequence(BBEPDISP *pBBEP, const uint8_t *pSeq);
void bbepWaitBusy(BBEPDISP *pBBEP);
//
// Set the second CS pin for dual-controller displays
//
void bbepSetCS2(BBEPDISP *pBBEP, uint8_t cs)
{
    pBBEP->iCS1Pin = pBBEP->iCSPin;
    pBBEP->iCS2Pin = cs;
    pinMode(cs, OUTPUT);
    digitalWrite(cs, HIGH); // disable second CS for now
} /* bbepSetCS2() */

void SPI_Write(BBEPDISP *pBBEP, uint8_t *pData, int iLen)
{
int i, j;
uint8_t uc;

    for (i=0; i<iLen; i++) {
        uc = *pData++;
        for (j=0; j<8; j++) {
            digitalWrite(pBBEP->iCLKPin, 0);
            digitalWrite(pBBEP->iMOSIPin, uc & 0x80);
            digitalWrite(pBBEP->iCLKPin, 1);
            uc <<= 1;
        }
    }
} /* SPI_Write() */

static void bbepT133A01WriteCommandData(BBEPDISP *pBBEP, uint8_t cmd, const uint8_t *pData, int iLen, uint8_t bBoth)
{
    if (pBBEP->iSpeed != 0) {
        SPI.beginTransaction(SPISettings(pBBEP->iSpeed, MSBFIRST, SPI_MODE0));
    }
    if (bBoth) {
        digitalWrite(pBBEP->iCS2Pin, LOW);
    }
    digitalWrite(pBBEP->iCS1Pin, LOW);
    digitalWrite(pBBEP->iDCPin, LOW);
    if (pBBEP->iSpeed == 0) {
        SPI_Write(pBBEP, &cmd, 1);
    } else {
        SPI.transfer(cmd);
    }
    digitalWrite(pBBEP->iDCPin, HIGH);
    for (int i = 0; i < iLen; i++) {
        uint8_t data = pData[i];
        if (pBBEP->iSpeed == 0) {
            SPI_Write(pBBEP, &data, 1);
        } else {
            SPI.transfer(data);
        }
    }
    digitalWrite(pBBEP->iCS1Pin, HIGH);
    if (pBBEP->iSpeed != 0) {
        SPI.endTransaction();
    }
    if (bBoth) {
        digitalWrite(pBBEP->iCS2Pin, HIGH);
    }
} /* bbepT133A01WriteCommandData() */

static void bbepT133A01InitIO(BBEPDISP *pBBEP)
{
    static const uint8_t r74DataBuf[9] = {0x00, 0x0c, 0x0c, 0xd9, 0xdd, 0xdd, 0x15, 0x15, 0x55};
    static const uint8_t rf0DataBuf[6] = {0x49, 0x55, 0x13, 0x5d, 0x05, 0x10};
    static const uint8_t psrDataBuf[2] = {0xdf, 0x69};
    static const uint8_t dcdcDataBuf[3] = {0x44, 0x54, 0x00};
    static const uint8_t cdiDataBuf[1] = {0x37};
    static const uint8_t r60DataBuf[2] = {0x03, 0x03};
    static const uint8_t r86DataBuf[1] = {0x10};
    static const uint8_t pwsDataBuf[1] = {0x22};
    static const uint8_t tresDataBuf[4] = {0x04, 0xb0, 0x03, 0x20};
    static const uint8_t pwrDataBuf[6] = {0x0f, 0x00, 0x28, 0x2c, 0x28, 0x38};
    static const uint8_t rb6DataBuf[1] = {0x07};
    static const uint8_t btstPDataBuf[2] = {0xe0, 0x20};
    static const uint8_t rb7DataBuf[1] = {0x01};
    static const uint8_t btstNDataBuf[2] = {0xe0, 0x20};
    static const uint8_t rb0DataBuf[1] = {0x01};
    static const uint8_t rb1DataBuf[1] = {0x02};

    digitalWrite(pBBEP->iRSTPin, LOW);
    delay(20);
    digitalWrite(pBBEP->iRSTPin, HIGH);
    delay(20);
    bbepWaitBusy(pBBEP);

    // Command groups match the vendor T133A01 init sequence.
    bbepT133A01WriteCommandData(pBBEP, 0x74, r74DataBuf, sizeof(r74DataBuf), 0);
    delay(10);
    bbepT133A01WriteCommandData(pBBEP, 0xf0, rf0DataBuf, sizeof(rf0DataBuf), 1);
    delay(10);
    bbepT133A01WriteCommandData(pBBEP, 0x00, psrDataBuf, sizeof(psrDataBuf), 1);
    delay(10);
    bbepT133A01WriteCommandData(pBBEP, 0xa5, dcdcDataBuf, sizeof(dcdcDataBuf), 0);
    delay(10);
    bbepT133A01WriteCommandData(pBBEP, 0x50, cdiDataBuf, sizeof(cdiDataBuf), 1);
    delay(10);
    bbepT133A01WriteCommandData(pBBEP, 0x60, r60DataBuf, sizeof(r60DataBuf), 1);
    delay(10);
    bbepT133A01WriteCommandData(pBBEP, 0x86, r86DataBuf, sizeof(r86DataBuf), 1);
    delay(10);
    bbepT133A01WriteCommandData(pBBEP, 0xe3, pwsDataBuf, sizeof(pwsDataBuf), 1);
    delay(10);
    bbepT133A01WriteCommandData(pBBEP, 0x61, tresDataBuf, sizeof(tresDataBuf), 1);
    delay(10);
    bbepT133A01WriteCommandData(pBBEP, 0x01, pwrDataBuf, sizeof(pwrDataBuf), 0);
    delay(10);
    bbepT133A01WriteCommandData(pBBEP, 0xb6, rb6DataBuf, sizeof(rb6DataBuf), 0);
    delay(10);
    bbepT133A01WriteCommandData(pBBEP, 0x06, btstPDataBuf, sizeof(btstPDataBuf), 0);
    delay(10);
    bbepT133A01WriteCommandData(pBBEP, 0xb7, rb7DataBuf, sizeof(rb7DataBuf), 0);
    delay(10);
    bbepT133A01WriteCommandData(pBBEP, 0x05, btstNDataBuf, sizeof(btstNDataBuf), 0);
    delay(10);
    bbepT133A01WriteCommandData(pBBEP, 0xb0, rb0DataBuf, sizeof(rb0DataBuf), 0);
    delay(10);
    bbepT133A01WriteCommandData(pBBEP, 0xb1, rb1DataBuf, sizeof(rb1DataBuf), 0);
    delay(10);
    pBBEP->iCSPin = pBBEP->iCS1Pin;
    pBBEP->is_awake = 1;
} /* bbepT133A01InitIO() */
//
// Initialize the GPIO pins and SPI for use by bb_eink
//
void bbepInitIO(BBEPDISP *pBBEP, uint8_t u8DC, uint8_t u8RST, uint8_t u8BUSY, uint8_t u8CS, uint8_t u8MOSI, uint8_t u8SCK, uint32_t u32Speed)
{
    pBBEP->iDCPin = u8DC;
    pBBEP->iCSPin = u8CS;
    pBBEP->iCS1Pin = u8CS; // primary CS is always controller #1 (keeps dual-CS logic correct regardless of setCS2() order)
    pBBEP->iMOSIPin = u8MOSI;
    pBBEP->iCLKPin = u8SCK;
    pBBEP->iRSTPin = u8RST;
    pBBEP->iBUSYPin = u8BUSY;

    pinMode(pBBEP->iDCPin, OUTPUT);
    if (pBBEP->iRSTPin != 0xff) {
        pinMode(pBBEP->iRSTPin, OUTPUT);
        digitalWrite(pBBEP->iRSTPin, HIGH);
    }
    if (pBBEP->iBUSYPin != 0xff) {
        pinMode(pBBEP->iBUSYPin, INPUT);
    }
    pBBEP->iSpeed = u32Speed;
    pinMode(pBBEP->iCSPin, OUTPUT);
    digitalWrite(pBBEP->iCSPin, HIGH); // manually control the CS pin
    if (u32Speed == 0) { // bit bang mode
        pinMode(pBBEP->iMOSIPin, OUTPUT);
        pinMode(pBBEP->iCLKPin, OUTPUT);
    } else {
        if (u8MOSI != 0xff) { // Not shared SPI?
#ifdef ARDUINO_ARCH_ESP32
            SPI.begin(pBBEP->iCLKPin, -1, pBBEP->iMOSIPin, -1); //pBBEP->iCSPin);
#else
            SPI.begin(); // other architectures have fixed SPI pins
#endif
            SPI.beginTransaction(SPISettings(u32Speed, MSBFIRST, SPI_MODE0));
#ifdef ARDUINO_ARCH_ESP32
// For NRF52, you have to leave an 'open' transaction
            SPI.endTransaction(); // N.B. - if you call beginTransaction() again without a matching endTransaction(), it will hang on ESP32
#endif
        }
    }
// Before we can start sending pixels, many panels need to know the display resolution
    if (pBBEP->iFlags & BBEP_T133A01) {
        bbepT133A01InitIO(pBBEP);
    } else {
        bbepSendCMDSequence(pBBEP, pBBEP->pInitFull);
        if (pBBEP->iFlags & BBEP_7COLOR) { // need to send before you can send it data
            if (pBBEP->iFlags & BBEP_SPLIT_BUFFER) {
               // Send the same sequence to the second controller
               pBBEP->iCSPin = pBBEP->iCS2Pin;
               bbepSendCMDSequence(pBBEP, pBBEP->pInitFull);
               pBBEP->iCSPin = pBBEP->iCS1Pin;
            }
        }
    }
} /* bbepInitIO() */

void bbepWriteIT8951Cmd(BBEPDISP *pBBEP, uint16_t cmd)
{
    uint8_t ucTemp[4];
    ucTemp[0] = 0x60; // command introducer = 0x6000
    ucTemp[1] = 0;
    ucTemp[2] = (uint8_t)(cmd >> 8);
    ucTemp[3] = (uint8_t)cmd;
    digitalWrite(pBBEP->iCSPin, LOW);
#ifdef ARDUINO_ARCH_ESP32
    SPI.transferBytes(ucTemp, NULL, 4);
#else
    SPI.transfer(ucTemp, 4);
#endif
    digitalWrite(pBBEP->iCSPin, HIGH);
} /* bbepWriteIT8951Cmd() */

void bbepWriteIT8951Data(BBEPDISP *pBBEP, uint8_t *pData, int iLen)
{
    digitalWrite(pBBEP->iCSPin, LOW);
    SPI.transfer(0);
    SPI.transfer(0); // 0x0000 is the data introducer
#ifdef ARDUINO_ARCH_ESP32
    SPI.transferBytes(pData, NULL, iLen);
#else
    SPI.transfer(pData, iLen);
#endif
    digitalWrite(pBBEP->iCSPin, HIGH);
} /* bbepWriteIT8951Data() */

void bbepWriteIT8951CmdArgs(BBEPDISP *pBBEP, uint16_t cmd, uint16_t *pArgs, int iCount)
{
    bbepWriteIT8951Cmd(pBBEP, cmd);
    for (int i=0; i<iCount; i++) {
        pArgs[i] = __builtin_bswap16(pArgs[i]);
    }
    bbepWriteIT8951Data(pBBEP, (uint8_t *)pArgs, iCount * 2);
} /* bbepWriteIT8951CmdArgs() */
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
    digitalWrite(pBBEP->iDCPin, LOW);
    delay(1);
    digitalWrite(pBBEP->iCSPin, LOW);
    if (pBBEP->iSpeed == 0) { // bit bang
        SPI_Write(pBBEP, &cmd, 1);
    } else {
        SPI.transfer(cmd);
    }
    digitalWrite(pBBEP->iCSPin, HIGH);
    digitalWrite(pBBEP->iDCPin, HIGH); // leave data mode as the default
} /* bbepWriteCmd() */
//
// Write 1 or more bytes as DATA (D/C set high)
//
void bbepWriteData(BBEPDISP *pBBEP, uint8_t *pData, int iLen)
{
//if (iLen < 5) {
//  Serial.print("DATA: ");
//  for (int i=0; i<iLen; i++) {
//     Serial.printf("0x%02x, ", pData[i]);
//  }
//  Serial.println(" ");
//} else {
//  Serial.printf("DATA: len=%d\n", iLen);
//}
//    digitalWrite(pBBEP->iDCPin, HIGH);
#ifdef ARDUINO_ARCH_ESP32
    if (pBBEP->iFlags & BBEP_CS_EVERY_BYTE) {
        for (int i=0; i<iLen; i++) {
            digitalWrite(pBBEP->iCSPin, LOW);
            if (pBBEP->iSpeed == 0) { // bit bang
                SPI_Write(pBBEP, &pData[i], 1);
            } else {
                SPI.transfer(pData[i]);
            }
            digitalWrite(pBBEP->iCSPin, HIGH);
        }
    } else {
        digitalWrite(pBBEP->iCSPin, LOW);
        if (pBBEP->iSpeed == 0) { // bit bang
            SPI_Write(pBBEP, pData, iLen);
        } else {
            SPI.transferBytes(pData, NULL, iLen);
        }
        digitalWrite(pBBEP->iCSPin, HIGH);
    }
#else
    if (pBBEP->iFlags & BBEP_CS_EVERY_BYTE) {
        for (int i=0; i<iLen; i++) { // Arduino clobbers the data (duplex)
            digitalWrite(pBBEP->iCSPin, LOW);
            if (pBBEP->iSpeed == 0) { // bit bang
                SPI_Write(pBBEP, &pData[i], 1);
            } else {
                SPI.transfer(pData[i]);
            }
            digitalWrite(pBBEP->iCSPin, HIGH);
        }
    } else {
        digitalWrite(pBBEP->iCSPin, LOW);
        for (int i=0; i<iLen; i++) { // Arduino clobbers the data (duplex)
            if (pBBEP->iSpeed == 0) { // bit bang
                SPI_Write(pBBEP, &pData[i], 1);
            } else {
                SPI.transfer(pData[i]);
            }
        }
        digitalWrite(pBBEP->iCSPin, HIGH);
    }
#endif
} /* bbepWriteData() */

void bbepWriteCmdData(BBEPDISP *pBBEP, uint8_t cmd, const uint8_t *pData, int iLen)
{
    if (pBBEP->iSpeed != 0) {
        SPI.beginTransaction(SPISettings(pBBEP->iSpeed, MSBFIRST, SPI_MODE0));
    }
    digitalWrite(pBBEP->iCSPin, LOW);
    digitalWrite(pBBEP->iDCPin, LOW);
    if (pBBEP->iSpeed == 0) {
        SPI_Write(pBBEP, &cmd, 1);
    } else {
        SPI.transfer(cmd);
    }
    digitalWrite(pBBEP->iDCPin, HIGH);
    for (int i = 0; i < iLen; i++) {
        uint8_t data = pData[i];
        if (pBBEP->iSpeed == 0) {
            SPI_Write(pBBEP, &data, 1);
        } else {
            SPI.transfer(data);
        }
    }
    digitalWrite(pBBEP->iCSPin, HIGH);
    digitalWrite(pBBEP->iDCPin, HIGH);
    if (pBBEP->iSpeed != 0) {
        SPI.endTransaction();
    }
} /* bbepWriteCmdData() */

void bbepStartDataStream(BBEPDISP *pBBEP, uint8_t cmd)
{
    digitalWrite(pBBEP->iCSPin, LOW);
    if (pBBEP->iSpeed != 0) {
        SPI.beginTransaction(SPISettings(pBBEP->iSpeed, MSBFIRST, SPI_MODE0));
    }
    digitalWrite(pBBEP->iDCPin, LOW);
    delay(1);
    if (pBBEP->iSpeed == 0) {
        SPI_Write(pBBEP, &cmd, 1);
    } else {
        SPI.transfer(cmd);
    }
    digitalWrite(pBBEP->iDCPin, HIGH);
} /* bbepStartDataStream() */

void bbepWriteDataStreamByte(BBEPDISP *pBBEP, uint8_t data)
{
    if (pBBEP->iSpeed == 0) {
        SPI_Write(pBBEP, &data, 1);
    } else {
        SPI.transfer(data);
    }
} /* bbepWriteDataStreamByte() */

void bbepEndDataStream(BBEPDISP *pBBEP)
{
    if (pBBEP->iSpeed != 0) {
        SPI.endTransaction();
    }
    digitalWrite(pBBEP->iCSPin, HIGH);
    digitalWrite(pBBEP->iDCPin, HIGH);
} /* bbepEndDataStream() */

//
// Convenience function to write a command byte along with a data
// byte (it's single parameter)
//
void bbepCMD2(BBEPDISP *pBBEP, uint8_t cmd1, uint8_t cmd2)
{
    bbepWriteCmd(pBBEP, cmd1);
    bbepWriteData(pBBEP, &cmd2, 1);
} /* bbepCMD2() */

#endif // __ARDUINO_IO__
