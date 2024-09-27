//
// bb_epaper I/O wrapper for the Raspberry Pi Pico
// Copyright (c) 2020 BitBank Software, Inc.
// Written by Larry Bank (bitbank@pobox.com)
// Project started 9/26/2024
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
#ifndef __PICO_IO__
#define __PICO_IO__
#define PROGMEM
#define memcpy_P memcpy
#define pgm_read_byte(a) *(uint8_t *)a
#define pgm_read_word(a) *(uint16_t *)a
#define false 0
#define true 1
#define LOW 0
#define HIGH 1
#ifndef I2C_SLAVE
#define I2C_SLAVE 0
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define HIGH 1
#define LOW 0
#endif
// forward references
void bbepWakeUp(BBEPDISP *pBBEP);
static void digitalWrite(int iPin, int iState) {
   gpio_put(iPin, iState);
}
static void pinMode(int iPin, int iMode)
{
  gpio_set_function(iPin,   GPIO_FUNC_SIO);
  if (iMode == INPUT || iMode == INPUT_PULLUP) {
      gpio_set_dir(iPin, GPIO_IN);
      if (iMode == INPUT_PULLUP) {
        gpio_pull_up(iPin);
      }
  } else if (iMode == OUTPUT) {
      gpio_set_dir(iPin, GPIO_OUT);
  }
} /* pinMode() */

static void delayMicroseconds(int iTime)
{
	sleep_us(iTime);
} /* delayMicroseconds() */
static int digitalRead(int iPin)
{
  return gpio_get(iPin);
} /* digitalRead() */

static void delay(int iTime)
{
  delayMicroseconds(iTime * 1000);
}

// Initialize SPI and epaper I/O
void bbepInitIO(BBEPDISP *pBBEP, uint8_t u8DC, uint8_t u8RST, uint8_t u8BUSY, uint8_t u8CS, uint8_t u8MOSI, uint8_t u8SCK, uint32_t u32Speed)
{
    pBBEP->iDCPin = u8DC;
    pBBEP->iCSPin = u8CS;
//    pBBEP->iMOSIPin = u8MOSI;
//    pBBEP->iCLKPin = u8SCK;
    pBBEP->iRSTPin = u8RST;
    pBBEP->iBUSYPin = u8BUSY;
    pinMode(pBBEP->iCSPin, OUTPUT);
    digitalWrite(pBBEP->iCSPin, HIGH);
    pinMode(pBBEP->iDCPin, OUTPUT);
    pinMode(pBBEP->iRSTPin, OUTPUT);
    digitalWrite(pBBEP->iRSTPin, LOW);
    delay(100);
    digitalWrite(pBBEP->iRSTPin, HIGH);
    delay(100);
    if (pBBEP->iBUSYPin != 0xff) {
        pinMode(pBBEP->iBUSYPin, INPUT);
    }

    spi_init(spi0, u32Speed);
//    gpio_set_function(iMISO, GPIO_FUNC_SPI);
//    gpio_set_function(u8CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
} /* bbepInitIO() */

void I2CInit(uint32_t iSpeed)
{
   // I2C Initialisation. Using it at 400Khz.
    i2c_init(i2c0, iSpeed);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
 }

static void bbepWriteData(BBEPDISP *pBBEP, uint8_t *pData, int iLen)
{
      digitalWrite(pBBEP->iCSPin, LOW);
      spi_write_blocking(spi0, pData, iLen);
      digitalWrite(pBBEP->iCSPin, HIGH);
} /* bbepWriteData() */
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
    digitalWrite(pBBEP->iCSPin, LOW);
    spi_write_blocking(spi0, &cmd1, 1);
    digitalWrite(pBBEP->iDCPin, HIGH);
    spi_write_blocking(spi0, &cmd2, 1); // second byte is data
    digitalWrite(pBBEP->iCSPin, HIGH);
    digitalWrite(pBBEP->iDCPin, HIGH); // leave data mode as the default
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
    digitalWrite(pBBEP->iCSPin, LOW);
    spi_write_blocking(spi0, &cmd, 1);
    digitalWrite(pBBEP->iCSPin, HIGH);
    digitalWrite(pBBEP->iDCPin, HIGH); // leave data mode as the default
} /* bbepWriteCmd() */

long millis(void)
{
  return time_us_32() / 1000;
}
long micros(void)
{
  return time_us_32();
}

#endif // __PICO_IO__
