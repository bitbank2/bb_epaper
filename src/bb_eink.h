//
// bb_eink
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
#ifndef __BB_EINK__
#define __BB_EINK__

#ifndef MEMORY_ONLY
#include <BitBang_I2C.h>
#endif

#ifdef _LINUX_
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
    BBEI_SUCCESS,
    BBEI_ERROR_BAD_PARAMETER,
    BBEI_ERROR_BAD_DATA,
    BBEI_ERROR_NOT_SUPPORTED,
    BBEI_ERROR_NO_MEMORY,
    BBEI_ERROR_COUNT
};

// Display refresh modes
enum {
    REFRESH_FULL=0,
    REFRESH_FAST,
    REFRESH_PARTIAL
};
// controller chip types
enum {
    BBEI_CHIP_NOT_DEFINED = 0,
    BBEI_CHIP_SSD16xx,
    BBEI_CHIP_UC81xx,
    BBEI_CHIP_COUNT
};

// 5 possible font sizes: 8x8, 16x32, 6x8, 12x16 (stretched from 6x8 with smoothing), 16x16 (stretched from 8x8)
enum {
   FONT_6x8 = 0,
   FONT_8x8,
   FONT_12x16,
   FONT_16x16,
   FONT_16x32,
   FONT_CUSTOM0,
   FONT_CUSTOM1,
   FONT_CUSTOM2,
   FONT_COUNT
};

typedef struct epd_panel {
    uint16_t width;
    uint16_t height;
    uint8_t *pInitFull;
    uint8_t *pInitFast;
    uint8_t *pInitPart;
    uint8_t flags;
    uint8_t chip_type;
} EPD_PANEL;

// Display types
enum {
    EPD_PANEL_UNDEFINED=0,
    EPD42_400x300, // WFT0420CZ15
    EPD42B_400x300, // DEPG0420BN / GDEY042T81
    EPD213B_104x212,
    EPD293_128x296,
    EPD294_128x296, // Waveshare newer 2.9" 1-bit 128x296
    EPD295_128x296, // harvested from Solum 2.9" BW ESLs
    EPD266_152x296, // GDEY0266T90
    EPD102_80x128, // GDEW0102T4
    EPD27B_176x264 // GDEY027T91
};
#ifdef FUTURE
    EPD42_4GRAY_400x300, // WFT0420CZ15
  EPD42Y_400x300, // DEPG0420YN
  EPD29_128x296,
  EPD29B_128x296,
  EPD29R_128x296,
  EPD29Y_128x296, // DEPG0290YN
  EPD42R_400x300,
  EPD42R2_400x300, // GDEQ042Z21
  EPD213R_104x212,
  EPD213R2_122x250, // DEPG0213RW
  EPD213R_104x212_d,
  EPD213_104x212,
  EPD213_122x250, // waveshare
  EPD213B_122x250, // GDEY0213B74
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
  EPD37_240x416, // GDEY037T03
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
#define BBEI_INVERTED 0x0001
#define BBEI_BITBANG  0x0002
#define BBEI_3COLOR   0x0004
#define BBEI_4COLOR   0x0008
#define BBEI_4GRAY    0x0010
#define BBEI_CS_EVERY_BYTE 0x0020

#define BBEI_WHITE 0
#define BBEI_BLACK 1
#define BBEI_YELLOW 2
#define BBEI_RED 3

// 4 gray levels
#define BBEI_GRAY0 0
#define BBEI_GRAY1 1
#define BBEI_GRAY2 2
#define BBEI_GRAY3 3

// (UC81xx commands)
enum reg_uc81xx {
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

enum reg_ssd16xx {
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

typedef struct bbeistruct
{
#ifndef MEMORY_ONLY
BBI2C bbi2c;
#endif
uint8_t wrap, flip, invert, type, render, can_flip, chip_type;
uint8_t *ucScreen;
int iCursorX, iCursorY;
int width, height, native_width, native_height;
int iScreenOffset, iOrientation;
int iFG, iBG; //current color
int iFont, iFlags;
int iDataTime, iOpTime; // time in milliseconds for data transmission and operation
uint32_t u32FontScaleX, u32FontScaleY;
uint32_t iSpeed;
uint32_t iTimeout; // for e-ink panels
uint8_t iDCPin, iMOSIPin, iCLKPin, iCSPin, iRSTPin, iBUSYPin;
uint8_t x_offset, y_offset; // memory offsets
uint8_t bBitBang;
const uint8_t *pInitFull; // full update init sequence
const uint8_t *pInitFast; // fast update init sequence
const uint8_t *pInitPart; // partial update init sequence
} BBEIDISP;

#ifdef __cplusplus
#ifdef _LINUX_
#include <string>
using namespace std;
class BBEINK
#else // Arduino
#ifndef __AVR__
class BBEINK : public Print
#else
class BBEINK
#endif // !__AVR__
#endif // _LINUX_
{
  public:
    BBEINK() { memset(&_bbei, 0, sizeof(_bbei)); _bbei.iFG = BBEI_BLACK; _bbei.render = 1; _bbei.type = EPD_UNDEFINED;}
    void initIO(int iCS, int iMOSI, int iSCLK, int iDC, int iReset=-1, int iBusy=-1, uint32_t u32Speed = 8000000);
    int setPanelType(int iPanel);
    uint32_t capabilities();
    int16_t height(void);
    int16_t width(void);
    void setPosition(int x, int y, int w, int h);
    void sleep(int bDeep);
    void wait(bool bQuick = false);
    int dataTime();
    int opTime();
    int allocBuffer(void);
    void * getBuffer(void);
    void setBuffer(uint8_t *pBuffer);
    void freeBuffer(void);
    void fillScreen(int iColor);
    void setRotation(int iAngle);
    int getRotation(void);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    int16_t getCursorX(void);
    int16_t getCursorY(void);
    void setCursor(int x, int y);
    void setPlane(int iPlane);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawCircle(int32_t x, int32_t y, int32_t r, uint32_t color);
    void fillCircle(int32_t x, int32_t y, int32_t r, uint32_t color);
    void drawEllipse(int16_t x, int16_t y, int32_t rx, int32_t ry, uint16_t color);
    void fillEllipse(int16_t x, int16_t y, int32_t rx, int32_t ry, uint16_t color);
    int drawGFX(uint8_t *pSrc, int iSrcCol, int iSrcRow, int iDestCol, int iDestRow, int iWidth, int iHeight, int iSrcPitch);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawSprite(uint8_t *pSprite, int cx, int cy, int iPitch, int x, int y, uint8_t iPriority);
    void drawTile(const uint8_t *pTile, int x, int y, int iRotation, int bInvert, int bRender);
    void setTextColor(int iFG, int iBG = -1);
    int refresh(int iMode);
    int drawEPDGFX(int x, int y, int cx, int cy, uint8_t *pPlane0, uint8_t *pPlane1);
    int loadBMP(const uint8_t *pBMP, int x, int y, int iFG, int iBG);
    int loadBMP3(const uint8_t *pBMP, int x, int y);
    void getTextBounds(const char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
    void getTextBounds(const String &str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
    void setTextWrap(bool bWrap);
    void setFont(int iFont);
    void setFreeFont(const BB_FONT *pFont);
    void pushBytes(uint8_t *pByte, int iCount);
    void writeCommand(uint8_t ucCMD);
    void writeRaw(uint8_t *pData, int iLen);
    void pushImage(int x, int y, int w, int h, uint16_t *pixels);
    void drawString(const char *pText, int x, int y);
    void drawString(String text, int x, int y);
    void drawLine(int x1, int y1, int x2, int y2, int iColor);
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
    using Print::write;
    virtual size_t write(uint8_t);
#endif // _LINUX_

  private:
    BBEIDISP _bbei;
}; // class BBEINK
#endif // __cplusplus

// Make the Linux library interface C instead of C++
#if defined(_LINUX_) && defined(__cplusplus)
extern "C" {
#endif

#if !defined(BITBANK_LCD_MODES)
#define BITBANK_LCD_MODES
typedef enum
{
 MODE_DATA = 0,
 MODE_COMMAND
} DC_MODE;
#endif

void EPDFill(OBDISP *pOBD, uint8_t ucCMD, uint8_t ucPattern);
void EPDSetPosition(OBDISP *pOBD, int x, int y, int cx, int cy);

//
// Create a virtual display of any size
// The memory buffer must be provided at the time of creation
//
void obdCreateVirtualDisplay(OBDISP *pOBD, int width, int height, uint8_t *buffer);
//
// Draw the contents of a memory buffer onto a display
// The sub-window will be clipped if it specifies too large an area
// for the destination display. The source OBDISP structure must have
// a valid back buffer defined
// The top and bottom destination edges will be drawn on byte boundaries (8 rows)
// The source top/bot edges can be on pixel boundaries
//
void obdDumpWindow(OBDISP *pOBDSrc, OBDISP *pOBDDest, int srcx, int srcy, int destx, int desty, int width, int height);
//
// Write one of more lines to a Sharp memory LCD
//
void obdWriteLCDLines(OBDISP *pOBD, int iStart, int iCount);
//
// Initializes the display controller into "page mode" on I2C
// If SDAPin and SCLPin are not -1, then bit bang I2C on those pins
// Otherwise use the Wire library.
// If you don't need to use a separate reset pin, set it to -1
//
int obdI2CInit(OBDISP *pOBD, int iType, int iAddr, int bFlip, int bInvert, int bWire, int iSDAPin, int iSCLPin, int iResetPin, int32_t iSpeed);
//
// Initialize an SPI version of the display
//
void obdSPIInit(OBDISP *pOBD, int iType, int iDC, int iCS, int iReset, int iMOSI, int iCLK, int iLED, int bFlip, int bInvert, int iBitBang, int32_t iSpeed);
//
// Set the drawing direction in 90 degree increments
//
void obdSetRotation(OBDISP *pOBD, int iRotation);
//
// Set the memory configuration to display the pixels at 0 or 180 degrees (flipped)
// pass true (1) to flip 180, false (0) to set to 0
void obdSetFlip(OBDISP *pOBD, int iOnOff);
//
// Provide or revoke a back buffer for your OLED graphics
// This allows you to manage the RAM used by ss_oled on tiny
// embedded platforms like the ATmega series
// Pass NULL to revoke the buffer. Make sure you provide a buffer
// large enough for your display (e.g. 128x64 needs 1K - 1024 bytes)
//
void obdSetBackBuffer(OBDISP *pOBD, uint8_t *pBuffer);
void obdAllocBuffer(OBDISP *pOBD);
//
// Sets the brightness (0=off, 255=brightest)
//
void obdSetContrast(OBDISP *pOBD, unsigned char ucContrast);
//
// Load a 1-bpp Windows bitmap
// Pass the pointer to the beginning of the BMP file
// First pass version assumes a full screen bitmap
//
int obdLoadBMP(OBDISP *pOBD, const uint8_t *pBMP, int x, int y, int iFG, int iBG);
//
// load a 4-bpp Windows bitmap
// into memory for 3-color (BLACK/WHITE/RED)
// e-paper displays
// It does a 'best match' of the colors to
// B/W/R
//
int obdLoadBMP3(OBDISP *pOBD, const uint8_t *pBMP, int dx, int dy);
//
// Power up/down the display
// useful for low power situations
//
void obdPower(OBDISP *pOBD, int bOn);
//
// Set the current cursor position
// The column represents the pixel column (0-127)
// The row represents the text row (0-7)
//
void obdSetCursor(OBDISP *pOBD, int x, int y);

//
// Turn text wrap on or off for the obdWriteString() function
//
void obdSetTextWrap(OBDISP *pOBD, int bWrap);
//
// Advance to the next line
//
void obdNextLine(OBDISP *pOBD);
//
// Draw a string of normal (8x8), small (6x8) or large (16x32) characters
// At the given col+row with the given scroll offset. The scroll offset allows you to
// horizontally scroll text which does not fit on the width of the display. The offset
// represents the pixels to skip when drawing the text. An offset of 0 starts at the beginning
// of the text.
// The system remembers where the last text was written (the cursor position)
// To continue writing from the last position, set the x,y values to -1
// The text can optionally wrap around to the next line by calling oledSetTextWrap(true);
// otherwise text which would go off the right edge will not be drawn and the cursor will
// be left "off screen" until set to a new position explicitly
//
//  Returns 0 for success, -1 for invalid parameter
//
int obdWriteString(OBDISP *pOBD, int iScrollX, int x, int y, char *szMsg, int iSize, int iColor, int bRender);
//
// Draw a string with a fractional scale in both dimensions
// the scale is a 16-bit integer with and 8-bit fraction and 8-bit mantissa
// To draw at 1x scale, set the scale factor to 256. To draw at 2x, use 512
// The output must be drawn into a memory buffer, not directly to the display
// The string can be drawn in one of 4 rotations (ROT_0, ROT_90, ROT_180, ROT_270)
//
int obdScaledString(OBDISP *pOBD, int x, int y, char *szMsg, int iSize, int iColor, int iXScale, int iYScale, int iRotation);
//
// Draw a string in a proportional font you supply
// Requires a back buffer
//
int obdWriteStringCustom(OBDISP *pOBD, GFXfont *pFont, int x, int y, char *szMsg, uint8_t ucColor);
//
// Get the width of text in a custom font
//
void obdGetStringBox(GFXfont *pFont, char *szMsg, int *width, int *top, int *bottom);
//
// Fill the frame buffer with a byte pattern
// e.g. all off (0x00) or all on (0xff)
//
void obdFill(OBDISP *pOBD, unsigned char ucData, int bRender);
//
// Set (or clear) an individual pixel
// The local copy of the frame buffer is used to avoid
// reading data from the display controller
// (which isn't possible in most configurations)
// This function needs the USE_BACKBUFFER macro to be defined
// otherwise, new pixels will erase old pixels within the same byte
//
int obdSetPixel(OBDISP *pOBD, int x, int y, unsigned char ucColor, int bRender);
//
// Dump a portion of the screen
// optional buffer pointer if no back buffer
//
int obdDumpPartial(OBDISP *pOBD, int startx, int starty, int width, int height, uint8_t *pBuffer);
//
// Dump an entire custom buffer to the display
// useful for custom animation effects
//
int obdDumpBuffer(OBDISP *pOBD, uint8_t *pBuffer, int bRefresh, int bWait, int bFast);
//
// Render a window of pixels from a provided buffer or the library's internal buffer
// to the display. The row values refer to byte rows, not pixel rows due to the memory
// layout of OLEDs. Pass a src pointer of NULL to use the internal backing buffer
// returns 0 for success, -1 for invalid parameter
//
int obdDrawGFX(OBDISP *pOBD, uint8_t *pSrc, int iSrcCol, int iSrcRow, int iDestCol, int iDestRow, int iWidth, int iHeight, int iSrcPitch);
//
// Set the current custom font pointers for playing back
// bytewise commands
//
void obdSetCustomFont(OBDISP *pOBD, GFXfont *pFont, uint8_t ucFont);
//
// Execute a set of bytewise command bytes
// and execute the drawing instructions on the current display/buffer
// Optionally render on backbuffer or physical display
//
void obdExecCommands(uint8_t *pData, int iLen, OBDISP *pOBD, int bRender);
//
// Return the number of bytes accumulated as commands
//
int obdGetCommandLen(OBDISP *pOBD);
//
// Draw a line between 2 points
//
void obdDrawLine(OBDISP *pOBD, int x1, int y1, int x2, int y2, uint8_t ucColor, int bRender);
//
// Play a frame of animation data
// The animation data is assumed to be encoded for a full frame of the display
// Given the pointer to the start of the compressed data,
// it returns the pointer to the start of the next frame
// Frame rate control is up to the calling program to manage
// When it finishes the last frame, it will start again from the beginning
//
uint8_t * obdPlayAnimFrame(OBDISP *pOBD, uint8_t *pAnimation, uint8_t *pCurrent, int iLen);
void obdWriteCommand(OBDISP *pOBD, unsigned char c);
void obdSetPosition(OBDISP *pOBD, int x, int y, int bRender);
void obdWriteDataBlock(OBDISP *pOBD, unsigned char *ucBuf, int iLen, int bRender);
void RawWriteData(OBDISP *pOBD, unsigned char *ucBuf, int iLen);
//
// Scroll the internal buffer by 1 scanline (up/down)
// width is in pixels, lines is group of 8 rows
// Returns 0 for success, -1 for invalid parameter
//
int obdScrollBuffer(OBDISP *pOBD, int iStartCol, int iEndCol, int iStartRow, int iEndRow, int bUp);
//
// Draw a sprite of any size in any position
// If it goes beyond the left/right or top/bottom edges
// it's trimmed to show the valid parts
// This function requires a back buffer to be defined
// The priority color (0 or 1) determines which color is painted 
// when a 1 is encountered in the source image.
// e.g. when 0, the input bitmap acts like a mask to clear
// the destination where bits are set.
//
void obdDrawSprite(OBDISP *pOBD, uint8_t *pSprite, int cx, int cy, int iPitch, int x, int y, uint8_t iPriority);

//
// Draw 1 or 2 planes of raw image into a specific spot
// in e-paper memory
//
int obdDrawEPDGFX(OBDISP *pOBD, int x, int y, int cx, int cy, uint8_t *pPlane0, uint8_t *pPlane1);

//
// Draw a 16x16 tile in any of 4 rotated positions
// Assumes input image is laid out like "normal" graphics with
// the MSB on the left and 2 bytes per line
// On AVR, the source image is assumed to be in FLASH memory
// The function can draw the tile on byte boundaries, so the x value
// can be from 0 to 112 and y can be from 0 to 6
//
void obdDrawTile(OBDISP *pOBD, const uint8_t *pTile, int x, int y, int iRotation, int bInvert, int bRender);
//
// Draw an outline or filled ellipse
//
void obdEllipse(OBDISP *pOBD, int iCenterX, int iCenterY, int32_t iRadiusX, int32_t iRadiusY, uint8_t ucColor, uint8_t bFilled);
//
// Draw an outline or filled rectangle
//
void obdRectangle(OBDISP *pOBD, int x1, int y1, int x2, int y2, uint8_t ucColor, uint8_t bFilled);

//
// Turn the LCD backlight on or off
//
void obdBacklight(OBDISP *pODB, int bOn);

//
// Menu functions
//
// Initialize the simple menu structure
//
int obdMenuInit(OBDISP *pOBD, SIMPLEMENU *sm, char **pText, int iFontSize, int bCenter, int btnUp, int btnDn, int btnEnter, int iPressedState, int bIsRotary);
//
// Erase the display and show the given menu
//
void obdMenuShow(SIMPLEMENU *sm, int iItem);
//
// Set a callback function to return custom info/status
// for each menu item
//
void obdMenuSetCallback(SIMPLEMENU *sm, SIMPLECALLBACK pfnCallBack);
//
// Display the text of a single menu item
// optionally erases what's under it to prevent left-over text when the length changes
//
void obdMenuShowItem(OBDISP *pOBD, int x, int y, char *szText, int bInvert, int bErase, int iFontSize, int bRender);
//
// Change the menu index incrementally
// redraws the minimum amount of screen to show the new info
// (this prevents flicker/flash and saves battery life)
// returns the new menu index
//
int obdMenuDelta(SIMPLEMENU *sm, int iDelta);
//
// With the given setup, check for button presses
// and act accordingly
// returns -1 for normal interactions and the menu item index if the user presses the ENTER button
//
int obdMenuRun(SIMPLEMENU *sm);

#if defined(_LINUX_) && defined(__cplusplus)
}
#endif // _LINUX_

#endif // __ONEBITDISPLAY__

