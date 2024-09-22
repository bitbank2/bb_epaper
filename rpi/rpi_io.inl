//
// bb_eink I/O wrapper functions for Raspberry Pi
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

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h> 
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>
#include <gpiod.h>
#include <math.h>
struct gpiod_chip *chip = NULL;
struct gpiod_line *lines[64];
static int spi_fd; // SPI handle

void SPI_transfer(BBEIDISP *pBBEI, uint8_t *pBuf, int iLen)
{
struct spi_ioc_transfer spi;
   memset(&spi, 0, sizeof(spi));
   spi.tx_buf = (unsigned long)pBuf;
   spi.len = iLen;
   spi.speed_hz = pBBEI->iSpeed;
   spi.bits_per_word = 8;
   ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi);
} /* SPI_transfer() */

int digitalRead(int iPin)
{
  return gpiod_line_get_value(lines[iPin]);
} /* digitalRead() */

void digitalWrite(int iPin, int iState)
{
   gpiod_line_set_value(lines[iPin], iState);
} /* digitalWrite() */

void pinMode(int iPin, int iMode)
{
   if (chip == NULL) {
       chip = gpiod_chip_open_by_name("gpiochip0");
   }
   lines[iPin] = gpiod_chip_get_line(chip, iPin);
   if (iMode == OUTPUT) {
       gpiod_line_request_output(lines[iPin], CONSUMER, 0);
   } else if (iMode == INPUT_PULLUP) {
       gpiod_line_request_input_flags(lines[iPin], CONSUMER, GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP);
   } else { // plain input
       gpiod_line_request_input(lines[iPin], CONSUMER);
   }
} /* pinMode() */

static void delay(int iMS)
{
  usleep(iMS * 1000);
} /* delay() */

static void delayMicroseconds(int iMS)
{
  usleep(iMS);
} /* delayMicroseconds() */

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
    pinMode(pBBEI->iCSPin, OUTPUT);
    digitalWrite(pBBEI->iCSPin, HIGH); // we have to manually control the CS pin
    spi_fd = open("/dev/spidev0.1", O_RDWR); // DEBUG - open SPI channel 0
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
    digitalWrite(pBBEI->iCSPin, LOW);
    SPI_transfer(pBBEI, &cmd1, 1);
    digitalWrite(pBBEI->iDCPin, HIGH);
    SPI_transfer(pBBEI, &cmd2, 1); // second byte is data
    digitalWrite(pBBEI->iCSPin, HIGH);
    digitalWrite(pBBEI->iDCPin, HIGH); // leave data mode as the default
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
    digitalWrite(pBBEI->iCSPin, LOW);
    SPI_transfer(pBBEI, &cmd, 1);
    digitalWrite(pBBEI->iCSPin, HIGH);
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
    digitalWrite(pBBEI->iCSPin, LOW);
    SPI_transfer(pBBEI, pData, iLen);
    digitalWrite(pBBEI->iCSPin, HIGH);
} /* bbeiWriteData() */

#endif // __BB_EI_IO__
