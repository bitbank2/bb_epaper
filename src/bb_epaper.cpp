//
// bb_epaper
// Copyright (c) 2024 BitBank Software, Inc.
// Written by Larry Bank (bitbank@pobox.com)
// Project started 9/11/2024
//
// Use of this software is governed by the Business Source License
// included in the file ./LICENSE.
//
// As of the Change Date specified in that file, in accordance with
// the Business Source License, use of this software will be governed
// by the Apache License, Version 2.0, included in the file
// ./APL.txt.
//

#ifdef _LINUX_
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#define OUTPUT 0
#define INPUT  1
#define INPUT_PULLUP 2
#define HIGH 1
#define LOW 0
void delay(int);
#else // Arduino

#endif // _LINUX_

#include "bb_epaper.h"
#ifdef _LINUX_
#include "rpi_io.inl"
#else
#include "arduino_io.inl" // I/O (non-portable) code is in here
#endif
#include "bb_ep.inl" // All of the display interface code is in here
#include "bb_ep_gfx.inl" // drawing code
#include "bb_font.h" // bitbank compressed font info
#ifdef __cplusplus
//
// C++ Class implementation
//
//uint32_t BBEPAPER::getRefreshTime(void)
//{
//    return _bbep.iTimeout;
//}

BBEPAPER::BBEPAPER(int iPanel)
{
    memset(&_bbep, 0, sizeof(_bbep));
    _bbep.iFG = BBEP_BLACK;
    bbepSetPanelType(&_bbep, iPanel);
}

void BBEPAPER::setAddrWindow(int x, int y, int w, int h)
{
    bbepSetAddrWindow(&_bbep, x, y, w, h);
}

#ifdef ARDUINO
void BBEPAPER::initIO(int iDC, int iReset, int iBusy, int iCS, int iMOSI, int iSCLK, uint32_t u32Speed)
{
    _bbep.iCSPin = iCS;
    _bbep.iMOSIPin = iMOSI;
    _bbep.iCLKPin = iSCLK;
    _bbep.iDCPin = iDC;
    _bbep.iRSTPin = iReset;
    _bbep.iBUSYPin = iBusy;
    bbepInitIO(&_bbep, u32Speed);
    bbepWakeUp(&_bbep);
    bbepSendCMDSequence(&_bbep, _bbep.pInitFull);
} /* initIO() */
#else // Linux
void BBEPAPER::initIO(int iDC, int iReset, int iBusy, int iCS, int iSPIChannel, uint32_t u32Speed)
{
	_bbep.iCSPin = iCS;
	_bbep.iDCPin = iDC;
	_bbep.iBUSYPin = iBusy;
	_bbep.iRSTPin = iReset;
	_bbep.iMOSIPin = iSPIChannel;
	bbepInitIO(&_bbep, u32Speed);
	bbepWakeUp(&_bbep);
	bbepSendCMDSequence(&_bbep, _bbep.pInitFull);
} /* initIO() */
#endif
int BBEPAPER::writePlane(int iPlane)
{
    return bbepWritePlane(&_bbep, iPlane);
} /* writePlane() */

int BBEPAPER::refresh(int iMode, bool bWait)
{
    int rc;
    rc = bbepRefresh(&_bbep, iMode);
    if (rc == BBEP_SUCCESS && bWait) {
        bbepWaitBusy(&_bbep);
    }
    return rc;
} /* refresh() */

//int BBEPAPER::getFlags(void)
//{
//    return _bbep.iFlags;
//}

//void BBEPAPER::setFlags(int iFlags)
//{
//    _bbep.iFlags = iFlags;
//}

void BBEPAPER::setBuffer(uint8_t *pBuffer)
{
    _bbep.ucScreen = pBuffer;
}

int BBEPAPER::allocBuffer(void)
{
    int iSize = ((_bbep.native_width+7)>>3) * _bbep.native_height;
    if (_bbep.iFlags & (BBEP_3COLOR | BBEP_4COLOR) || _bbep.chip_type == BBEP_CHIP_UC81xx)
        iSize *= 2; // 2 bit planes
    _bbep.ucScreen = (uint8_t *)malloc(iSize);
    if (_bbep.ucScreen != NULL) {
        return BBEP_SUCCESS;
    }
    return BBEP_ERROR_NO_MEMORY; // failed
} /* allocBuffer() */

void * BBEPAPER::getBuffer(void)
{
    return (void *)_bbep.ucScreen;
} /* getBuffer() */

void BBEPAPER::freeBuffer(void)
{
    if (_bbep.ucScreen) {
        free(_bbep.ucScreen);
        _bbep.ucScreen = NULL;
    }
} /* freeBuffer() */

uint32_t BBEPAPER::capabilities(void)
{
  return _bbep.iFlags;
}

void BBEPAPER::setRotation(int iRotation)
{
    bbepSetRotation(&_bbep, iRotation);
} /* setRotation() */

int BBEPAPER::getRotation(void)
{
  return _bbep.iOrientation;
}

void BBEPAPER::fillScreen(int iColor, int iPlane)
{
  bbepFill(&_bbep, iColor, iPlane);
} /* fillScreen() */

void BBEPAPER::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    bbepRectangle(&_bbep, x, y, x+w-1, y+h-1, color, 0);
} /* drawRect() */

void BBEPAPER::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    bbepRectangle(&_bbep, x, y, x+w-1, y+h-1, color, 1);
} /* fillRect() */

void BBEPAPER::setTextWrap(bool bWrap)
{
  _bbep.wrap = (int)bWrap;
}

void BBEPAPER::setTextColor(int iFG, int iBG)
{
    if (iFG > BBEP_RED) iFG = BBEP_BLACK;
    if (iBG > BBEP_RED) iBG = BBEP_BLACK;
    if ((_bbep.iFlags & (BBEP_3COLOR | BBEP_4COLOR)) == 0) {
        if (iFG == BBEP_RED) iFG = BBEP_BLACK; // can't set red color
        if (iBG == BBEP_RED) iBG = BBEP_BLACK;
    }
    _bbep.iFG = iFG;
    _bbep.iBG = (iBG == -1) ? iFG : iBG;
} /* setTextColor() */

void BBEPAPER::setCursor(int x, int y)
{
    if (x >= 0)
       _bbep.iCursorX = x;
    if (y >= 0)
    _bbep.iCursorY = y;
} /* setCursor() */

int BBEPAPER::loadBMP(const uint8_t *pBMP, int x, int y, int iFG, int iBG)
{
    return bbepLoadBMP(&_bbep, pBMP, x, y, iFG, iBG);
} /* loadBMP() */

int BBEPAPER::loadBMP3(const uint8_t *pBMP, int x, int y)
{
    return bbepLoadBMP3(&_bbep, pBMP, x, y);
} /* loadBMP3() */

void BBEPAPER::setFont(int iFont)
{
    _bbep.iFont = iFont;
    _bbep.pFont = NULL;
} /* setFont() */

void BBEPAPER::setFont(const void *pFont)
{
    _bbep.iFont = -1;
    _bbep.pFont = (void *)pFont;
} /* setFont() */

//void BBEPAPER::setFreeFont(const GFXfont *pFont)
//{
//    _bbep.pFreeFont = (GFXfont *)pFont;
//} /* setFreeFont() */

void BBEPAPER::drawLine(int x1, int y1, int x2, int y2, int iColor)
{
    bbepDrawLine(&_bbep, x1, y1, x2, y2, iColor);
} /* drawLine() */

#ifdef _LINUX_
void BBEPAPER::print(const string &str)
{
   print(str.c_str());
} /* print() */

void BBEPAPER::println(const string &str)
{
char ucTemp[4];

   print(str);
   ucTemp[0] = '\n';
   ucTemp[1] = '\r';
   ucTemp[2] = 0;
   print((const char *)ucTemp);
} /* print() */

void BBEPAPER::print(const char *pString)
{
uint8_t *s = (uint8_t *)pString;

   while (*s != 0) {
      write(*s++);
   }
} /* print() */

void BBEPAPER::println(const char *pString)
{
char ucTemp[4];

    print(pString);
    ucTemp[0] = '\n';
    ucTemp[1] = '\r';
    ucTemp[2] = 0;
    print((const char *)ucTemp);
} /* println() */
void BBEPAPER::print(int value, int format)
{
char c, ucTemp[32];
char *d = &ucTemp[31];

   if (value) {
   d[0] = 0;
   switch(format) {
      case DEC:
         while (value) {
	     d--;
             *d = '0' + (value % 10);
             value /= 10;
	 }
         break;
      case HEX:
	 while (value) {
            d--;
            c = value & 0xf;
	    if (c < 10)
		    *d = '0' + c;
	    else
		    *d = 'A' + (c-10);
	    value >>= 4;
	 }
         break;
      case OCT:
	 while (value) {
            d--;
            *d = '0' + (value & 7);
	    value >>= 3;
	 }
         break;
      case BIN:
         while (value) {
            d--;
            *d = '0' + (value & 1);
	    value >>= 1;
	 }
         break;
      default:
         break;
      }
   } else { // if zero value
     d--;
     *d = '0';
   }
      print((const char *)d);
} /* print() */

void BBEPAPER::println(int value, int format)
{
char ucTemp[4];

	print(value, format);
	ucTemp[0] = '\n';
	ucTemp[1] = '\r';
	ucTemp[2] = 0;
	print((const char *)ucTemp);
} /* println() */

#endif // _LINUX_
//
// write (Arduino Print friend class)
//
#ifndef __AVR__
size_t BBEPAPER::write(uint8_t c) {
char szTemp[2]; // used to draw 1 character at a time to the C methods
int w, h;

  szTemp[0] = c; szTemp[1] = 0;
   if (_bbep.pFont == NULL) { // use built-in fonts
      if (_bbep.iFont == FONT_8x8 || _bbep.iFont == FONT_6x8) {
        h = 8;
        w = (_bbep.iFont == FONT_8x8) ? 8 : 6;
      } else if (_bbep.iFont == FONT_12x16) {
        h = 16;
        w = 12;
      }

    if (c == '\n') {              // Newline?
      _bbep.iCursorX = 0;          // Reset x to zero,
      _bbep.iCursorY += h; // advance y one line
    } else if (c != '\r') {       // Ignore carriage returns
        if (_bbep.wrap && ((_bbep.iCursorX + w) > _bbep.width)) { // Off right?
            _bbep.iCursorX = 0;               // Reset x to zero,
            _bbep.iCursorY += h; // advance y one line
        }
        bbepWriteString(&_bbep, -1, -1, szTemp, _bbep.iFont, _bbep.iFG);
    }
  } else { // Custom font
      BB_FONT *pBBF = (BB_FONT *)_bbep.pFont;
    if (c == '\n') {
      _bbep.iCursorX = 0;
      _bbep.iCursorY += pBBF->height;
    } else if (c != '\r') {
      if (c >= pBBF->first && c <= pBBF->last) {
          BB_GLYPH *pBBG = &pBBF->glyphs[c-pBBF->first];
        w = pBBG->width;
        h = pBBG->height;
        if ((w > 0) && (h > 0)) { // Is there an associated bitmap?
          int16_t xo = pBBG->xOffset;
          w += xo; // xadvance
          h = pBBF->height;
          if (_bbep.wrap && ((_bbep.iCursorX + w) > _bbep.width)) {
            _bbep.iCursorX = 0;
            _bbep.iCursorY += h;
          }
            bbepWriteStringCustom(&_bbep, (BB_FONT *)_bbep.pFont, -1, -1, szTemp, _bbep.iFG, _bbep.iPlane);
        }
      }
    }
  }
  return 1;
} /* write() */
#endif // !__AVR__

void BBEPAPER::drawPixel(int16_t x, int16_t y, uint8_t color)
{
    bbepSetPixel(&_bbep, x, y, color);
}
int16_t BBEPAPER::getCursorX(void)
{
  return _bbep.iCursorX;
}
int16_t BBEPAPER::getCursorY(void)
{
  return _bbep.iCursorY;
}
void BBEPAPER::getTextBounds(const char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h)
{
    if (_bbep.pFont) {
        int width, top, bottom;
        bbepGetStringBox((BB_FONT *)_bbep.pFont, string, &width, &top, &bottom);
        *x1 = x;
        *w = width;
        *y1 = y + top;
        *h = (bottom - top + 1);
    }
}
#ifdef ARDUINO
void BBEPAPER::getTextBounds(const String &str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h)
{
    getTextBounds(str.c_str(), x, y, x1, y1, w, h);
}
#endif
int BBEPAPER::dataTime(void)
{
    return _bbep.iDataTime;
}
int BBEPAPER::opTime(void)
{
    return _bbep.iOpTime;
}
int16_t BBEPAPER::width(void)
{
   return _bbep.width;
}
int16_t BBEPAPER::height(void)
{
   return _bbep.height;
}
void BBEPAPER::drawCircle(int32_t x, int32_t y, int32_t r, uint32_t color)
{
  bbepEllipse(&_bbep, x, y, r, r, color, 0);
}
void BBEPAPER::fillCircle(int32_t x, int32_t y, int32_t r, uint32_t color)
{
    bbepEllipse(&_bbep, x, y, r, r, color, 1);
}
void BBEPAPER::drawEllipse(int16_t x, int16_t y, int32_t rx, int32_t ry, uint16_t color)
{
    bbepEllipse(&_bbep, x, y, rx, ry, color, 0);
}
void BBEPAPER::fillEllipse(int16_t x, int16_t y, int32_t rx, int32_t ry, uint16_t color)
{
    bbepEllipse(&_bbep, x, y, rx, ry, color, 1);
}

void BBEPAPER::sleep(int bDeep)
{
    bbepSleep(&_bbep, bDeep);
}
void BBEPAPER::wait(bool bQuick)
{
    bbepWaitBusy(&_bbep);
}
void BBEPAPER::drawString(const char *pText, int x, int y)
{
    if (_bbep.pFont) {
        bbepWriteStringCustom(&_bbep, (BB_FONT *)_bbep.pFont, x, y, (char *)pText, _bbep.iFG, 0); // iPlane);
    } else if (_bbep.iFont >= FONT_6x8 && _bbep.iFont < FONT_COUNT) {
        bbepWriteString(&_bbep, x, y, (char *)pText, _bbep.iFont, _bbep.iFG);
    }
} /* drawString() */

void BBEPAPER::setPlane(int iPlane)
{
    _bbep.iPlane = iPlane;
}
int BBEPAPER::getPlane(void)
{
    return _bbep.iPlane;
}

int BBEPAPER::getChip(void)
{
    return _bbep.chip_type;
}
void BBEPAPER::drawSprite(const uint8_t *pSprite, int cx, int cy, int iPitch, int x, int y, uint8_t iColor)
{
    bbepDrawSprite(&_bbep, pSprite, cx, cy, iPitch, x, y, iColor);
}
void BBEPAPER::startWrite(int iPlane)
{
    bbepStartWrite(&_bbep, iPlane);
} /* startWrite() */

void BBEPAPER::writeData(uint8_t *pData, int iLen)
{
   bbepWriteData(&_bbep, pData, iLen);
} /* writeData() */

void BBEPAPER::writeCmd(uint8_t u8Cmd)
{
    bbepWriteCmd(&_bbep, u8Cmd);
} /* writeCmd() */

#endif // __cplusplus
