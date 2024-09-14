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
//

#ifdef _LINUX_
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
// convert wire library constants into ArmbianIO values
#define OUTPUT GPIO_OUT
#define INPUT GPIO_IN
#define INPUT_PULLUP GPIO_IN_PULLUP
#define HIGH 1
#define LOW 0
void delay(int);
#else // Arduino

#endif // _LINUX_

#include "bb_eink.h"
#include "bb_ei_io.inl" // I/O (non-portable) code is in here
#include "bb_ei.inl" // All of the display interface code is in here
#include "bb_ei_gfx.inl" // drawing code
#include "Group5.h" // Group5 data compression library
#include "bb_font.h" // bitbank compressed font info
#ifdef __cplusplus
//
// C++ Class implementation
//
//uint32_t BBEINK::getRefreshTime(void)
//{
//    return _bbei.iTimeout;
//}

void BBEINK::setPosition(int x, int y, int w, int h)
{
    bbeiSetPosition(&_bbei, x, y, w, h);
}
int BBEINK::setPanelType(int iPanel)
{
    return bbeiSetPanelType(&_bbei, iPanel);
}
void BBEINK::initIO(int iDC, int iReset, int iBusy, int iCS, int iMOSI, int iSCLK, uint32_t u32Speed)
{
    _bbei.iCSPin = iCS;
    _bbei.iMOSIPin = iMOSI;
    _bbei.iCLKPin = iSCLK;
    _bbei.iDCPin = iDC;
    _bbei.iRSTPin = iReset;
    _bbei.iBUSYPin = iBusy;
    bbeiInitIO(&_bbei, u32Speed);
    bbeiWakeUp(&_bbei);
    bbeiSendCMDSequence(&_bbei, _bbei.pInitFull);
} /* initIO() */

int BBEINK::writePlane(int iPlane)
{
    return bbeiWritePlane(&_bbei, iPlane);
} /* writePlane() */

int BBEINK::refresh(int iMode, bool bWait)
{
    int rc;
    rc = bbeiRefresh(&_bbei, iMode);
    if (rc == BBEI_SUCCESS && bWait) {
        bbeiWaitBusy(&_bbei);
    }
    return rc;
} /* refresh() */

//int BBEINK::getFlags(void)
//{
//    return _bbei.iFlags;
//}

//void BBEINK::setFlags(int iFlags)
//{
//    _bbei.iFlags = iFlags;
//}

void BBEINK::setBuffer(uint8_t *pBuffer)
{
    _bbei.ucScreen = pBuffer;
}

int BBEINK::allocBuffer(void)
{
    int iSize = ((_bbei.native_width+7)>>3) * _bbei.native_height;
    if (_bbei.iFlags & (BBEI_3COLOR | BBEI_4COLOR) || _bbei.chip_type == BBEI_CHIP_UC81xx)
        iSize *= 2; // 2 bit planes
    _bbei.ucScreen = (uint8_t *)malloc(iSize);
    if (_bbei.ucScreen != NULL) {
        return BBEI_SUCCESS;
    }
    return BBEI_ERROR_NO_MEMORY; // failed
} /* allocBuffer() */

void * BBEINK::getBuffer(void)
{
    return (void *)_bbei.ucScreen;
} /* getBuffer() */

void BBEINK::freeBuffer(void)
{
    if (_bbei.ucScreen) {
        free(_bbei.ucScreen);
        _bbei.ucScreen = NULL;
    }
} /* freeBuffer() */

uint32_t BBEINK::capabilities(void)
{
  return _bbei.iFlags;
}

void BBEINK::setRotation(int iRotation)
{
//    obdSetRotation(&_bbei, iRotation);
} /* setRotation() */

int BBEINK::getRotation(void)
{
  return _bbei.iOrientation;
}

void BBEINK::fillScreen(int iColor, int iPlane)
{
  bbeiFill(&_bbei, iColor, iPlane);
} /* fillScreen() */

void BBEINK::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    bbeiRectangle(&_bbei, x, y, x+w-1, y+h-1, color, 0);
} /* drawRect() */

void BBEINK::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    bbeiRectangle(&_bbei, x, y, x+w-1, y+h-1, color, 1);
} /* fillRect() */

void BBEINK::setTextWrap(bool bWrap)
{
  _bbei.wrap = (int)bWrap;
}

void BBEINK::setTextColor(int iFG, int iBG)
{
    if (iFG > BBEI_RED) iFG = BBEI_BLACK;
    if (iBG > BBEI_RED) iBG = BBEI_BLACK;
    if ((_bbei.iFlags & (BBEI_3COLOR | BBEI_4COLOR)) == 0) {
        if (iFG == BBEI_RED) iFG = BBEI_BLACK; // can't set red color
        if (iBG == BBEI_RED) iBG = BBEI_BLACK;
    }
    _bbei.iFG = iFG;
    _bbei.iBG = (iBG == -1) ? iFG : iBG;
} /* setTextColor() */

void BBEINK::setCursor(int x, int y)
{
    if (x >= 0)
       _bbei.iCursorX = x;
    if (y >= 0)
    _bbei.iCursorY = y;
} /* setCursor() */

int BBEINK::loadBMP(const uint8_t *pBMP, int x, int y, int iFG, int iBG)
{
    return bbeiLoadBMP(&_bbei, pBMP, x, y, iFG, iBG);
} /* loadBMP() */

int BBEINK::loadBMP3(const uint8_t *pBMP, int x, int y)
{
    return bbeiLoadBMP3(&_bbei, pBMP, x, y);
} /* loadBMP3() */

void BBEINK::setFont(int iFont)
{
    _bbei.iFont = iFont;
    _bbei.pFont = NULL;
} /* setFont() */

void BBEINK::setFont(const void *pFont)
{
    _bbei.iFont = -1;
    _bbei.pFont = (void *)pFont;
} /* setFont() */

//void BBEINK::setFreeFont(const GFXfont *pFont)
//{
//    _bbei.pFreeFont = (GFXfont *)pFont;
//} /* setFreeFont() */

void BBEINK::drawLine(int x1, int y1, int x2, int y2, int iColor)
{
    bbeiDrawLine(&_bbei, x1, y1, x2, y2, iColor);
} /* drawLine() */

#ifdef _LINUX_
void BBEINK::print(const string &str)
{
   print(str.c_str());
} /* print() */

void BBEINK::println(const string &str)
{
char ucTemp[4];

   print(str);
   ucTemp[0] = '\n';
   ucTemp[1] = '\r';
   ucTemp[2] = 0;
   print((const char *)ucTemp);
} /* print() */

void BBEINK::print(const char *pString)
{
uint8_t *s = (uint8_t *)pString;

   while (*s != 0) {
      write(*s++);
   }
} /* print() */

void BBEINK::println(const char *pString)
{
char ucTemp[4];

    print(pString);
    ucTemp[0] = '\n';
    ucTemp[1] = '\r';
    ucTemp[2] = 0;
    print((const char *)ucTemp);
} /* println() */
void BBEINK::print(int value, int format)
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

void BBEINK::println(int value, int format)
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
size_t BBEINK::write(uint8_t c) {
#ifdef FUTURE
char szTemp[2]; // used to draw 1 character at a time to the C methods
int w, h;

  szTemp[0] = c; szTemp[1] = 0;
   if (_bbei.pFreeFont == NULL) { // use built-in fonts
       if (_bbei.iFont == -1) { // scaled 5x7 font
           h = (int)(_bbei.u32FontScaleY >> 8) * 8;
           w = (int)(_bbei.u32FontScaleX >> 8) * 6;
       } else if (_bbei.iFont == FONT_8x8 || _bbei.iFont == FONT_6x8) {
        h = 8;
        w = (_bbei.iFont == FONT_8x8) ? 8 : 6;
      } else if (_bbei.iFont == FONT_12x16 || _bbei.iFont == FONT_16x16) {
        h = 16;
        w = (_bbei.iFont == FONT_12x16) ? 12 : 16;
      } else { w = 16; h = 32; }

    if (c == '\n') {              // Newline?
      _bbei.iCursorX = 0;          // Reset x to zero,
      _bbei.iCursorY += h; // advance y one line
        // should we scroll the screen up 1 line?
        if (_bbei.iCursorY >= _bbei.height && _bbei.ucScreen && _bbei.bScroll) {
            obdScroll1Line(&_bbei, h/8);
            if (_bbei.render) {
                obdDumpBuffer(&_bbei, NULL, false, false, false);
            }
            _bbei.iCursorY -= h;
        }
    } else if (c != '\r') {       // Ignore carriage returns
      if (_bbei.wrap && ((_bbei.iCursorX + w) > _bbei.width)) { // Off right?
        _bbei.iCursorX = 0;               // Reset x to zero,
        _bbei.iCursorY += h; // advance y one line
          // should we scroll the screen up 1 line?
          if (_bbei.iCursorY >= _bbei.height && _bbei.ucScreen && _bbei.bScroll) {
              obdScroll1Line(&_bbei, h/8);
              if (_bbei.render) {
                  obdDumpBuffer(&_bbei, NULL, false, false, false);
              }
              _bbei.iCursorY -= h;
          }
      }
        if (_bbei.iFont == -1) { // scaled drawing
            obdScaledString(&_bbei, _bbei.iCursorX, _bbei.iCursorY, szTemp, FONT_6x8, _bbei.iFG, _bbei.u32FontScaleX, _bbei.u32FontScaleY, 0);
            _bbei.iCursorX += w;
        } else {
            obdWriteString(&_bbei, 0, -1, -1, szTemp, _bbei.iFont, _bbei.iFG, _bbei.render);
        }
    }
  } else { // Custom font
    if (c == '\n') {
      _bbei.iCursorX = 0;
      _bbei.iCursorY += (uint8_t)pgm_read_byte(&_bbei.pFreeFont->yAdvance);
    } else if (c != '\r') {
      uint8_t first = pgm_read_word((const uint8_t*)&_bbei.pFreeFont->first);
      if ((c >= first) && (c <= (uint8_t)pgm_read_word((const uint8_t*)&_bbei.pFreeFont->last))) {
        GFXglyph *glyph = pgm_read_glyph_ptr(_bbei.pFreeFont, c - first);
        w = pgm_read_byte(&glyph->width);
        h = pgm_read_byte(&glyph->height);
        if ((w > 0) && (h > 0)) { // Is there an associated bitmap?
          int16_t xo = (int8_t)pgm_read_word((const uint8_t*)&glyph->xOffset);
          w += xo; // xadvance
          h = (uint8_t)pgm_read_byte(&_bbei.pFreeFont->yAdvance);
          if (_bbei.wrap && ((_bbei.iCursorX + w) > _bbei.width)) {
            _bbei.iCursorX = 0;
            _bbei.iCursorY += h;
          }
            bbeiWriteStringCustom(&_bbei, _bbei.pFreeFont, -1, -1, szTemp, _bbei.iFG);
        }
      }
    }
  }
#endif // FUTURE
  return 1;
} /* write() */
#endif // !__AVR__

void BBEINK::drawPixel(int16_t x, int16_t y, uint8_t color)
{
    bbeiSetPixel(&_bbei, x, y, color);
}
int16_t BBEINK::getCursorX(void)
{
  return _bbei.iCursorX;
}
int16_t BBEINK::getCursorY(void)
{
  return _bbei.iCursorY;
}
void BBEINK::getTextBounds(const char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h)
{
//    if (_bbei.pFreeFont) {
//        int width, top, bottom;
  //      obdGetStringBox(_bbei.pFreeFont, (char *)string, &width, &top, &bottom);
//        *x1 = x;
//        *w = width;
//        *y1 = y + top;
//        *h = (bottom - top + 1);
//    }
}
void BBEINK::getTextBounds(const String &str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h)
{
    getTextBounds(str.c_str(), x, y, x1, y1, w, h);
}
int BBEINK::dataTime(void)
{
    return _bbei.iDataTime;
}
int BBEINK::opTime(void)
{
    return _bbei.iOpTime;
}
int16_t BBEINK::width(void)
{
   return _bbei.width;
}
int16_t BBEINK::height(void)
{
   return _bbei.height;
}
void BBEINK::drawCircle(int32_t x, int32_t y, int32_t r, uint32_t color)
{
  bbeiEllipse(&_bbei, x, y, r, r, color, 0);
}
void BBEINK::fillCircle(int32_t x, int32_t y, int32_t r, uint32_t color)
{
    bbeiEllipse(&_bbei, x, y, r, r, color, 1);
}
void BBEINK::drawEllipse(int16_t x, int16_t y, int32_t rx, int32_t ry, uint16_t color)
{
    bbeiEllipse(&_bbei, x, y, rx, ry, color, 0);
}
void BBEINK::fillEllipse(int16_t x, int16_t y, int32_t rx, int32_t ry, uint16_t color)
{
    bbeiEllipse(&_bbei, x, y, rx, ry, color, 1);
}

void BBEINK::sleep(int bDeep)
{
    bbeiSleep(&_bbei, bDeep);
}
void BBEINK::wait(bool bQuick)
{
    bbeiWaitBusy(&_bbei);
}
void BBEINK::drawString(const char *pText, int x, int y)
{
    if (_bbei.pFont) {
        bbeiWriteStringCustom(&_bbei, (BB_FONT *)_bbei.pFont, x, y, (char *)pText, _bbei.iFG, 0); // iPlane);
    } else if (_bbei.iFont >= FONT_6x8 && _bbei.iFont < FONT_COUNT) {
        bbeiWriteString(&_bbei, x, y, (char *)pText, _bbei.iFont, _bbei.iFG);
    }
} /* drawString() */

#endif // __cplusplus
