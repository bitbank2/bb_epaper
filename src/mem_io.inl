//
// bb_eink I/O stub functions for memory-only use
// Copyright (c) 2025 BitBank Software, Inc.
// Written by Larry Bank (bitbank@pobox.com)
// Project started 11/17/2025
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

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h> 
#include <string.h>
#define pgm_read_byte(a) (*(uint8_t *)a)
#define pgm_read_word(a) (*(uint16_t *)a)
#define pgm_read_dword(a) (*(uint32_t *)a)
#define memcpy_P memcpy

// forward references
void bbepWakeUp(BBEPDISP *pBBEP);
void bbepSendCMDSequence(BBEPDISP *pBBEP, const uint8_t *pSeq);

void SPI_transfer(BBEPDISP *pBBEP, uint8_t *pBuf, int iLen)
{
    (void)pBBEP;
    (void)pBuf;
    (void)iLen;
} /* SPI_transfer() */

int digitalRead(int iPin)
{
    (void)iPin;
    return 0;
} /* digitalRead() */

void digitalWrite(int iPin, int iState)
{
    (void)iPin;
    (void)iState;
} /* digitalWrite() */

void pinMode(int iPin, int iMode)
{
   (void)iPin;
   (void)iMode;
} /* pinMode() */

void delay(int iMS)
{
  (void)iMS;
} /* delay() */

long millis(void)
{
   return 0;
} /* millis() */

//
// Initialize the GPIO pins and SPI for use by bb_eink
//
void bbepInitIO(BBEPDISP *pBBEP, int iDC, int iReset, int iBusy, int iCS, int iMOSI, int iSCK, uint32_t u32Speed)
{
    pBBEP->iSpeed = u32Speed;
    (void)iDC; (void)iReset; (void)iBusy; (void)iCS; (void)iMOSI; (void)iSCK;
} /* bbepInitIO() */
//
// Convenience function to write a command byte along with a data
// byte (it's single parameter)
//
void bbepCMD2(BBEPDISP *pBBEP, uint8_t cmd1, uint8_t cmd2)
{
    pBBEP->is_awake = 1;
    (void)cmd1;
    (void)cmd2;
} /* bbepCMD2() */
//
// Set the second CS pin for dual-controller displays
//
void bbepSetCS2(BBEPDISP *pBBEP, uint8_t cs)
{
    pBBEP->iCS1Pin = pBBEP->iCSPin;
    pBBEP->iCS2Pin = cs;
} /* bbepSetCS2() */

//
// Write a single byte as a COMMAND (D/C set low)
//
void bbepWriteCmd(BBEPDISP *pBBEP, uint8_t cmd)
{
    (void)cmd;
    pBBEP->is_awake = 1;
} /* bbepWriteCmd() */
//
// Write 1 or more bytes as DATA (D/C set high)
//
void bbepWriteData(BBEPDISP *pBBEP, uint8_t *pData, int iLen)
{
    (void)pBBEP;
    (void)pData;
    (void)iLen;
} /* bbepWriteData() */

#endif // __BB_EP_IO__
