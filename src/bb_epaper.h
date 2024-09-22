//
// bb_epaper
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
#ifndef __BB_EPAPER__
#define __BB_EPAPER__

#ifdef ARDUINO
#include <Arduino.h>
#else // _LINUX_
// for Print support
#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2
#define PROGMEM
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#endif // _LINUX_

// error messages
enum {
    BBEP_SUCCESS,
    BBEP_ERROR_BAD_PARAMETER,
    BBEP_ERROR_BAD_DATA,
    BBEP_ERROR_NOT_SUPPORTED,
    BBEP_ERROR_NO_MEMORY,
    BBEP_ERROR_COUNT
};

#define LIGHT_SLEEP 0
#define DEEP_SLEEP 1

// Display refresh modes
#define REFRESH_FULL 0
#define REFRESH_FAST 1
#define REFRESH_PARTIAL 2

// controller chip types
enum {
    BBEP_CHIP_NOT_DEFINED = 0,
    BBEP_CHIP_SSD16xx,
    BBEP_CHIP_UC81xx,
    BBEP_CHIP_COUNT
};

// enum for writing local framebuffer to EPD memory plane(s)
enum {
    PLANE_0=0,
    PLANE_1,
    PLANE_BOTH,
    PLANE_DUPLICATE // duplicate 0 to both 0 and 1
};
// 5 possible font sizes: 8x8, 16x32, 6x8, 12x16 (stretched from 6x8 with smoothing), 16x16 (stretched from 8x8)
enum {
   FONT_6x8 = 0,
   FONT_8x8,
   FONT_12x16,
   FONT_COUNT
};
// Centering coordinates to pass to the character drawing functions
#define CENTER_X 9998
#define CENTER_Y 9999

typedef struct epd_panel {
    uint16_t width;
    uint16_t height;
    const uint8_t *pInitFull;
    const uint8_t *pInitFast;
    const uint8_t *pInitPart;
    uint8_t flags;
    uint8_t chip_type;
} EPD_PANEL;

// Display types
enum {
    EPD_PANEL_UNDEFINED=0,
    EPD42_400x300, // WFT0420CZ15
    EPD42B_400x300, // DEPG0420BN / GDEY042T81
    EPD213_122x250, // waveshare
    EPD213B_122x250, // GDEY0213B74
    EPD293_128x296,
    EPD294_128x296, // Waveshare newer 2.9" 1-bit 128x296
    EPD295_128x296, // harvested from Solum 2.9" BW ESLs
    EPD266_152x296, // GDEY0266T90
    EPD102_80x128, // GDEW0102T4
    EPD27B_176x264, // GDEY027T91
    EPD29R_128x296,
    EPD122_192x176, // GDEM0122T61
    EPD154R_152x152,
    EPD42R_400x300,
    EPD42R2_400x300, // GDEQ042Z21
    EPD37_240x416, // GDEY037T03
    EPD_PANEL_COUNT
};
#ifdef FUTURE
    EPD42_4GRAY_400x300, // WFT0420CZ15
  EPD42Y_400x300, // DEPG0420YN
  EPD29_128x296,
  EPD29B_128x296,
  EPD29Y_128x296, // DEPG0290YN
  EPD213R_104x212,
  EPD213R2_122x250, // DEPG0213RW
  EPD213R_104x212_d,
  EPD213_104x212,
  EPD154_152x152, // GDEW0154M10
  EPD154R_152x152,
  EPD154Y_152x152, // DEPG0154YN
  EPD154_200x200, // waveshare
  EPD154B_200x200, // DEPG01540BN
  EPD27_176x264, // waveshare
  EPD266B_152x296, // DEPG0266BN
  EPD266Y_152x296, // DEPG0266YN
  EPD31R_168x296, // DEPG0310RW
  EPD37Y_240x416, // DEPG0370YN
  EPD579_792x272, // GDEY0579T93
  EPD583R_600x448,
  EPD75_800x480, // GDEY075T7
  EPD74R_640x384,
  EPD583_648x480, // DEPG0583BN
  EPD29_BWYR_128x296, // GDEY029F51
  EPD29_BWYR_168x384, // GDEY029F51H
  EPD266_BWYR_184x360, // GDEY0266F51
  EPD30_BWYR_168x400, // Waveshare 3" B/W/Y/R
  EPD164_BWYR_168x168, // Waveshare 1.64" B/W/Y/R
  EPD236_BWYR_168x296, // Waveshare 2.36" B/W/Y/R
  EPD47_540x960, // not working yet
  EPD35R_184x384, // Hanshow Nebular 3.5" BWR
  EPD_PANEL_COUNT
};
#endif // FUTURE
// flag bits
#define BBEP_INVERTED 0x0001
#define BBEP_BITBANG  0x0002
#define BBEP_3COLOR   0x0004
#define BBEP_4COLOR   0x0008
#define BBEP_4GRAY    0x0010
#define BBEP_CS_EVERY_BYTE 0x0020

#define BBEP_WHITE 0
#define BBEP_BLACK 1
#define BBEP_YELLOW 2
#define BBEP_RED 3

// 4 gray levels
#define BBEP_GRAY0 0
#define BBEP_GRAY1 1
#define BBEP_GRAY2 2
#define BBEP_GRAY3 3

// (UC81xx commands)
enum {
    UC8151_PSR      = 0x00,
    UC8151_PWR      = 0x01,
    UC8151_POFF     = 0x02,
    UC8151_PFS      = 0x03,
    UC8151_PON      = 0x04,
    UC8151_PMES     = 0x05,
    UC8151_BTST     = 0x06,
    UC8151_DSLP     = 0x07,
    UC8151_DTM1     = 0x10,
    UC8151_DSP      = 0x11,
    UC8151_DRF      = 0x12,
    UC8151_DTM2     = 0x13,
    UC8151_LUT_VCOM = 0x20,
    UC8151_LUT_WW   = 0x21,
    UC8151_LUT_BW   = 0x22,
    UC8151_LUT_WB   = 0x23,
    UC8151_LUT_BB   = 0x24,
    UC8151_LUT_VCOM2 = 0x25,
    UC8151_PLL      = 0x30,
    UC8151_TSC      = 0x40,
    UC8151_TSE      = 0x41,
    UC8151_TSR      = 0x43,
    UC8151_TSW      = 0x42,
    UC8151_CDI      = 0x50,
    UC8151_LPD      = 0x51,
    UC8151_TCON     = 0x60,
    UC8151_TRES     = 0x61,
    UC8151_REV      = 0x70,
    UC8151_FLG      = 0x71,
    UC8151_AMV      = 0x80,
    UC8151_VV       = 0x81,
    UC8151_VDCS     = 0x82,
    UC8151_PTL      = 0x90,
    UC8151_PTIN     = 0x91,
    UC8151_PTOU     = 0x92,
    UC8151_PGM      = 0xa0,
    UC8151_APG      = 0xa1,
    UC8151_ROTP     = 0xa2,
    UC8151_CCSET    = 0xe0,
    UC8151_PWS      = 0xe3,
    UC8151_TSSET    = 0xe5
  };

enum {
    SSD1608_DRIVER_CONTROL = 0x01,
    SSD1608_GATE_VOLTAGE = 0x03,
    SSD1608_SOURCE_VOLTAGE = 0x04,
    SSD1608_DISPLAY_CONTROL = 0x07,
    SSD1608_NON_OVERLAP = 0x0B,
    SSD1608_BOOSTER_SOFT_START = 0x0C,
    SSD1608_GATE_SCAN_START = 0x0F,
    SSD1608_DEEP_SLEEP = 0x10,
    SSD1608_DATA_MODE = 0x11,
    SSD1608_SW_RESET = 0x12,
    SSD1608_TEMP_WRITE = 0x1A,
    SSD1608_TEMP_READ = 0x1B,
    SSD1608_TEMP_CONTROL = 0x1C,
    SSD1608_TEMP_LOAD = 0x1D,
    SSD1608_MASTER_ACTIVATE = 0x20,
    SSD1608_DISP_CTRL1 = 0x21,
    SSD1608_DISP_CTRL2 = 0x22,
    SSD1608_WRITE_RAM = 0x24,
    SSD1608_WRITE_ALTRAM = 0x26,
    SSD1608_READ_RAM = 0x25,
    SSD1608_VCOM_SENSE = 0x28,
    SSD1608_VCOM_DURATION = 0x29,
    SSD1608_WRITE_VCOM = 0x2C,
    SSD1608_READ_OTP = 0x2D,
    SSD1608_WRITE_LUT = 0x32,
    SSD1608_WRITE_DUMMY = 0x3A,
    SSD1608_WRITE_GATELINE = 0x3B,
    SSD1608_WRITE_BORDER = 0x3C,
    SSD1608_SET_RAMXPOS = 0x44,
    SSD1608_SET_RAMYPOS = 0x45,
    SSD1608_SET_RAMXCOUNT = 0x4E,
    SSD1608_SET_RAMYCOUNT = 0x4F,
    SSD1608_NOP = 0xFF,
};

#define BUSY_WAIT 0xff

typedef struct bbepstruct
{
uint8_t wrap, type, chip_type;
uint8_t *ucScreen;
int iCursorX, iCursorY;
int width, height, native_width, native_height;
int iScreenOffset, iOrientation;
int iFG, iBG; //current color
int iFont, iFlags;
void *pFont;
int iDataTime, iOpTime; // time in milliseconds for data transmission and operation
uint32_t iSpeed;
uint32_t iTimeout; // for e-paper panels
uint8_t iDCPin, iMOSIPin, iCLKPin, iCSPin, iRSTPin, iBUSYPin;
uint8_t x_offset, y_offset; // memory offsets
uint8_t is_awake, iPlane;
const uint8_t *pInitFull; // full update init sequence
const uint8_t *pInitFast; // fast update init sequence
const uint8_t *pInitPart; // partial update init sequence
} BBEPDISP;

#ifdef __cplusplus
#ifdef _LINUX_
#include <string>
using namespace std;
class BBEPAPER
#else // Arduino
#ifndef __AVR__
class BBEPAPER : public Print
#else
class BBEPAPER
#endif // !__AVR__
#endif // _LINUX_
{
  public:
    BBEPAPER(int iPanel);
    void setAddrWindow(int x, int y, int w, int h);
    void initIO(int iDC, int iReset, int iBusy, int iCS = SS, int iMOSI = MOSI, int iSCLK = SCK, uint32_t u32Speed = 8000000);
    int writePlane(int iPlane = PLANE_DUPLICATE);
    void startWrite(int iPlane);
    void writeData(uint8_t *pData, int iLen);
    void writeCmd(uint8_t u8Cmd);
    int refresh(int iMode, bool bWait = true);
    void setBuffer(uint8_t *pBuffer);
    int allocBuffer(void);
    void * getBuffer(void);
    void freeBuffer(void);
    uint32_t capabilities();
    void setRotation(int iAngle);
    int getRotation(void);
    void fillScreen(int iColor, int iPlane = PLANE_DUPLICATE);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void setTextWrap(bool bWrap);
    void setTextColor(int iFG, int iBG = -1);
    void setCursor(int x, int y);
    int loadBMP(const uint8_t *pBMP, int x, int y, int iFG, int iBG);
    int loadBMP3(const uint8_t *pBMP, int x, int y);
    void setFont(int iFont);
    void setFont(const void *pFont);
    void drawLine(int x1, int y1, int x2, int y2, int iColor);
    void drawPixel(int16_t x, int16_t y, uint8_t color);
    int16_t getCursorX(void);
    int16_t getCursorY(void);
    void getTextBounds(const char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
    void getTextBounds(const String &str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
    int dataTime();
    int opTime();
    int16_t height(void);
    int16_t width(void);
    void drawCircle(int32_t x, int32_t y, int32_t r, uint32_t color);
    void fillCircle(int32_t x, int32_t y, int32_t r, uint32_t color);
    void drawEllipse(int16_t x, int16_t y, int32_t rx, int32_t ry, uint16_t color);
    void fillEllipse(int16_t x, int16_t y, int32_t rx, int32_t ry, uint16_t color);
    void sleep(int bDeep);
    void wait(bool bQuick = false);
    void drawString(const char *pText, int x, int y);
    void setPlane(int iPlane);
    int getPlane(void);
    int getChip(void);
    void drawSprite(const uint8_t *pSprite, int cx, int cy, int iPitch, int x, int y, uint8_t iColor);
#ifdef FUTURE
//    int drawGFX(int x, int y, int cx, int cy, uint8_t *pPlane0, uint8_t *pPlane1);
//    void setFreeFont(const BB_FONT *pFont);
    void pushBytes(uint8_t *pByte, int iCount);
    void writeCommand(uint8_t ucCMD);
    void writeRaw(uint8_t *pData, int iLen);
    void pushImage(int x, int y, int w, int h, uint16_t *pixels);
    void drawString(String text, int x, int y);
#endif // FUTURE
    
#ifdef _LINUX_
    void print(const char *pString);
    void println(const char *pString);
    void print(int, int);
    void println(int, int);
    void print(const string &);
    void println(const string &);
    size_t write(uint8_t ucChar);
    void delayMicroseconds(int iTime);
#else
#ifndef __AVR__
    using Print::write;
    virtual size_t write(uint8_t);
#endif // __AVR__
#endif // _LINUX_

  private:
    BBEPDISP _bbep;
}; // class BBEPAPER
#endif // __cplusplus

#if !defined(BITBANK_LCD_MODES)
#define BITBANK_LCD_MODES
typedef enum
{
 MODE_DATA = 0,
 MODE_COMMAND
} DC_MODE;
#endif

#endif // __BB_EPAPER__

