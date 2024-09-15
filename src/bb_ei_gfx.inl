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
// bb_ei_gfx.inl - graphics functions
//
#ifndef __BB_EI_GFX__
#define __BB_EI_GFX__
#include <Group5.h>
#include "bb_font.h"
G5DECIMAGE g5dec;
// forward declarations
void InvertBytes(uint8_t *pData, uint8_t bLen);

const uint8_t ucFont[]PROGMEM = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x5f,0x5f,0x06,0x00,
  0x00,0x07,0x07,0x00,0x07,0x07,0x00,0x14,0x7f,0x7f,0x14,0x7f,0x7f,0x14,
  0x24,0x2e,0x2a,0x6b,0x6b,0x3a,0x12,0x46,0x66,0x30,0x18,0x0c,0x66,0x62,
  0x30,0x7a,0x4f,0x5d,0x37,0x7a,0x48,0x00,0x04,0x07,0x03,0x00,0x00,0x00,
  0x00,0x1c,0x3e,0x63,0x41,0x00,0x00,0x00,0x41,0x63,0x3e,0x1c,0x00,0x00,
  0x08,0x2a,0x3e,0x1c,0x3e,0x2a,0x08,0x00,0x08,0x08,0x3e,0x3e,0x08,0x08,
  0x00,0x00,0x80,0xe0,0x60,0x00,0x00,0x00,0x08,0x08,0x08,0x08,0x08,0x08,
  0x00,0x00,0x00,0x60,0x60,0x00,0x00,0x60,0x30,0x18,0x0c,0x06,0x03,0x01,
  0x3e,0x7f,0x59,0x4d,0x47,0x7f,0x3e,0x40,0x42,0x7f,0x7f,0x40,0x40,0x00,
  0x62,0x73,0x59,0x49,0x6f,0x66,0x00,0x22,0x63,0x49,0x49,0x7f,0x36,0x00,
  0x18,0x1c,0x16,0x53,0x7f,0x7f,0x50,0x27,0x67,0x45,0x45,0x7d,0x39,0x00,
  0x3c,0x7e,0x4b,0x49,0x79,0x30,0x00,0x03,0x03,0x71,0x79,0x0f,0x07,0x00,
  0x36,0x7f,0x49,0x49,0x7f,0x36,0x00,0x06,0x4f,0x49,0x69,0x3f,0x1e,0x00,
  0x00,0x00,0x00,0x66,0x66,0x00,0x00,0x00,0x00,0x80,0xe6,0x66,0x00,0x00,
  0x08,0x1c,0x36,0x63,0x41,0x00,0x00,0x00,0x14,0x14,0x14,0x14,0x14,0x14,
  0x00,0x41,0x63,0x36,0x1c,0x08,0x00,0x00,0x02,0x03,0x59,0x5d,0x07,0x02,
  0x3e,0x7f,0x41,0x5d,0x5d,0x5f,0x0e,0x7c,0x7e,0x13,0x13,0x7e,0x7c,0x00,
  0x41,0x7f,0x7f,0x49,0x49,0x7f,0x36,0x1c,0x3e,0x63,0x41,0x41,0x63,0x22,
  0x41,0x7f,0x7f,0x41,0x63,0x3e,0x1c,0x41,0x7f,0x7f,0x49,0x5d,0x41,0x63,
  0x41,0x7f,0x7f,0x49,0x1d,0x01,0x03,0x1c,0x3e,0x63,0x41,0x51,0x33,0x72,
  0x7f,0x7f,0x08,0x08,0x7f,0x7f,0x00,0x00,0x41,0x7f,0x7f,0x41,0x00,0x00,
  0x30,0x70,0x40,0x41,0x7f,0x3f,0x01,0x41,0x7f,0x7f,0x08,0x1c,0x77,0x63,
  0x41,0x7f,0x7f,0x41,0x40,0x60,0x70,0x7f,0x7f,0x0e,0x1c,0x0e,0x7f,0x7f,
  0x7f,0x7f,0x06,0x0c,0x18,0x7f,0x7f,0x1c,0x3e,0x63,0x41,0x63,0x3e,0x1c,
  0x41,0x7f,0x7f,0x49,0x09,0x0f,0x06,0x1e,0x3f,0x21,0x31,0x61,0x7f,0x5e,
  0x41,0x7f,0x7f,0x09,0x19,0x7f,0x66,0x26,0x6f,0x4d,0x49,0x59,0x73,0x32,
  0x03,0x41,0x7f,0x7f,0x41,0x03,0x00,0x7f,0x7f,0x40,0x40,0x7f,0x7f,0x00,
  0x1f,0x3f,0x60,0x60,0x3f,0x1f,0x00,0x3f,0x7f,0x60,0x30,0x60,0x7f,0x3f,
  0x63,0x77,0x1c,0x08,0x1c,0x77,0x63,0x07,0x4f,0x78,0x78,0x4f,0x07,0x00,
  0x47,0x63,0x71,0x59,0x4d,0x67,0x73,0x00,0x7f,0x7f,0x41,0x41,0x00,0x00,
  0x01,0x03,0x06,0x0c,0x18,0x30,0x60,0x00,0x41,0x41,0x7f,0x7f,0x00,0x00,
  0x08,0x0c,0x06,0x03,0x06,0x0c,0x08,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
  0x00,0x00,0x03,0x07,0x04,0x00,0x00,0x20,0x74,0x54,0x54,0x3c,0x78,0x40,
  0x41,0x7f,0x3f,0x48,0x48,0x78,0x30,0x38,0x7c,0x44,0x44,0x6c,0x28,0x00,
  0x30,0x78,0x48,0x49,0x3f,0x7f,0x40,0x38,0x7c,0x54,0x54,0x5c,0x18,0x00,
  0x48,0x7e,0x7f,0x49,0x03,0x06,0x00,0x98,0xbc,0xa4,0xa4,0xf8,0x7c,0x04,
  0x41,0x7f,0x7f,0x08,0x04,0x7c,0x78,0x00,0x44,0x7d,0x7d,0x40,0x00,0x00,
  0x60,0xe0,0x80,0x84,0xfd,0x7d,0x00,0x41,0x7f,0x7f,0x10,0x38,0x6c,0x44,
  0x00,0x41,0x7f,0x7f,0x40,0x00,0x00,0x7c,0x7c,0x18,0x78,0x1c,0x7c,0x78,
  0x7c,0x78,0x04,0x04,0x7c,0x78,0x00,0x38,0x7c,0x44,0x44,0x7c,0x38,0x00,
  0x84,0xfc,0xf8,0xa4,0x24,0x3c,0x18,0x18,0x3c,0x24,0xa4,0xf8,0xfc,0x84,
  0x44,0x7c,0x78,0x4c,0x04,0x0c,0x18,0x48,0x5c,0x54,0x74,0x64,0x24,0x00,
  0x04,0x04,0x3e,0x7f,0x44,0x24,0x00,0x3c,0x7c,0x40,0x40,0x3c,0x7c,0x40,
  0x1c,0x3c,0x60,0x60,0x3c,0x1c,0x00,0x3c,0x7c,0x60,0x30,0x60,0x7c,0x3c,
  0x44,0x6c,0x38,0x10,0x38,0x6c,0x44,0x9c,0xbc,0xa0,0xa0,0xfc,0x7c,0x00,
  0x4c,0x64,0x74,0x5c,0x4c,0x64,0x00,0x08,0x08,0x3e,0x77,0x41,0x41,0x00,
  0x00,0x00,0x00,0x77,0x77,0x00,0x00,0x41,0x41,0x77,0x3e,0x08,0x08,0x00,
  0x02,0x03,0x01,0x03,0x02,0x03,0x01,0x70,0x78,0x4c,0x46,0x4c,0x78,0x70};

  // 5x7 font (in 6x8 cell)
const uint8_t ucSmallFont[] PROGMEM = {
0x00,0x00,0x00,0x00,0x00,
0x00,0x06,0x5f,0x06,0x00,
0x07,0x03,0x00,0x07,0x03,
0x24,0x7e,0x24,0x7e,0x24,
0x24,0x2b,0x6a,0x12,0x00,
0x63,0x13,0x08,0x64,0x63,
0x36,0x49,0x56,0x20,0x50,
0x00,0x07,0x03,0x00,0x00,
0x00,0x3e,0x41,0x00,0x00,
0x00,0x41,0x3e,0x00,0x00,
0x08,0x3e,0x1c,0x3e,0x08,
0x08,0x08,0x3e,0x08,0x08,
0x00,0xe0,0x60,0x00,0x00,
0x08,0x08,0x08,0x08,0x08,
0x00,0x60,0x60,0x00,0x00,
0x20,0x10,0x08,0x04,0x02,
0x3e,0x51,0x49,0x45,0x3e,
0x00,0x42,0x7f,0x40,0x00,
0x62,0x51,0x49,0x49,0x46,
0x22,0x49,0x49,0x49,0x36,
0x18,0x14,0x12,0x7f,0x10,
0x2f,0x49,0x49,0x49,0x31,
0x3c,0x4a,0x49,0x49,0x30,
0x01,0x71,0x09,0x05,0x03,
0x36,0x49,0x49,0x49,0x36,
0x06,0x49,0x49,0x29,0x1e,
0x00,0x6c,0x6c,0x00,0x00,
0x00,0xec,0x6c,0x00,0x00,
0x08,0x14,0x22,0x41,0x00,
0x24,0x24,0x24,0x24,0x24,
0x00,0x41,0x22,0x14,0x08,
0x02,0x01,0x59,0x09,0x06,
0x3e,0x41,0x5d,0x55,0x1e,
0x7e,0x11,0x11,0x11,0x7e,
0x7f,0x49,0x49,0x49,0x36,
0x3e,0x41,0x41,0x41,0x22,
0x7f,0x41,0x41,0x41,0x3e,
0x7f,0x49,0x49,0x49,0x41,
0x7f,0x09,0x09,0x09,0x01,
0x3e,0x41,0x49,0x49,0x7a,
0x7f,0x08,0x08,0x08,0x7f,
0x00,0x41,0x7f,0x41,0x00,
0x30,0x40,0x40,0x40,0x3f,
0x7f,0x08,0x14,0x22,0x41,
0x7f,0x40,0x40,0x40,0x40,
0x7f,0x02,0x04,0x02,0x7f,
0x7f,0x02,0x04,0x08,0x7f,
0x3e,0x41,0x41,0x41,0x3e,
0x7f,0x09,0x09,0x09,0x06,
0x3e,0x41,0x51,0x21,0x5e,
0x7f,0x09,0x09,0x19,0x66,
0x26,0x49,0x49,0x49,0x32,
0x01,0x01,0x7f,0x01,0x01,
0x3f,0x40,0x40,0x40,0x3f,
0x1f,0x20,0x40,0x20,0x1f,
0x3f,0x40,0x3c,0x40,0x3f,
0x63,0x14,0x08,0x14,0x63,
0x07,0x08,0x70,0x08,0x07,
0x71,0x49,0x45,0x43,0x00,
0x00,0x7f,0x41,0x41,0x00,
0x02,0x04,0x08,0x10,0x20,
0x00,0x41,0x41,0x7f,0x00,
0x04,0x02,0x01,0x02,0x04,
0x80,0x80,0x80,0x80,0x80,
0x00,0x03,0x07,0x00,0x00,
0x20,0x54,0x54,0x54,0x78,
0x7f,0x44,0x44,0x44,0x38,
0x38,0x44,0x44,0x44,0x28,
0x38,0x44,0x44,0x44,0x7f,
0x38,0x54,0x54,0x54,0x08,
0x08,0x7e,0x09,0x09,0x00,
0x18,0xa4,0xa4,0xa4,0x7c,
0x7f,0x04,0x04,0x78,0x00,
0x00,0x00,0x7d,0x40,0x00,
0x40,0x80,0x84,0x7d,0x00,
0x7f,0x10,0x28,0x44,0x00,
0x00,0x00,0x7f,0x40,0x00,
0x7c,0x04,0x18,0x04,0x78,
0x7c,0x04,0x04,0x78,0x00,
0x38,0x44,0x44,0x44,0x38,
0xfc,0x44,0x44,0x44,0x38,
0x38,0x44,0x44,0x44,0xfc,
0x44,0x78,0x44,0x04,0x08,
0x08,0x54,0x54,0x54,0x20,
0x04,0x3e,0x44,0x24,0x00,
0x3c,0x40,0x20,0x7c,0x00,
0x1c,0x20,0x40,0x20,0x1c,
0x3c,0x60,0x30,0x60,0x3c,
0x6c,0x10,0x10,0x6c,0x00,
0x9c,0xa0,0x60,0x3c,0x00,
0x64,0x54,0x54,0x4c,0x00,
0x08,0x3e,0x41,0x41,0x00,
0x00,0x00,0x77,0x00,0x00,
0x00,0x41,0x41,0x3e,0x08,
0x02,0x01,0x02,0x01,0x00,
0x3c,0x26,0x23,0x26,0x3c};

//
// Draw a sprite of any size in any position
// If it goes beyond the left/right or top/bottom edges
// it's trimmed to show the valid parts
// This function requires a back buffer to be defined
// The priority color (0 or 1) determines which color is painted
// when a 1 is encountered in the source image.
//
void bbeiDrawSprite(BBEIDISP *pBBEI, uint8_t *pSprite, int cx, int cy, int iPitch, int x, int y, uint8_t iPriority)
{
    int tx, ty, dx, dy, iStartX;
    uint8_t *s, *d, uc, pix, ucSrcMask, ucDstMask;
    int iLocalPitch;
    iLocalPitch = pBBEI->width;

    if (pBBEI == NULL) return;
    if (x+cx < 0 || y+cy < 0 || x >= pBBEI->width || y >= pBBEI->height || pBBEI->ucScreen == NULL)
        return; // no backbuffer or out of bounds
    dy = y; // destination y
    if (y < 0) // skip the invisible parts
    {
        cy += y;
        y = -y;
        pSprite += (y * iPitch);
        dy = 0;
    }
    if (y + cy > pBBEI->height)
        cy = pBBEI->height - y;
    iStartX = 0;
    dx = x;
    if (x < 0)
    {
        cx += x;
        x = -x;
        iStartX = x;
        dx = 0;
    }
    if (x + cx > pBBEI->width)
        cx = pBBEI->width - x;
    for (ty=0; ty<cy; ty++)
    {
        s = &pSprite[(iStartX >> 3)];
        d = &pBBEI->ucScreen[(dy>>3) * iLocalPitch + dx];
        ucSrcMask = 0x80 >> (iStartX & 7);
        pix = *s++;
        ucDstMask = 1 << (dy & 7);
        if (iPriority) // priority color is 1
        {
          for (tx=0; tx<cx; tx++)
          {
            uc = d[0];
            if (pix & ucSrcMask) // set pixel in source, set it in dest
              d[0] = (uc | ucDstMask);
            d++; // next pixel column
            ucSrcMask >>= 1;
            if (ucSrcMask == 0) // read next byte
            {
                ucSrcMask = 0x80;
                pix = *s++;
            }
          } // for tx
        } // priorty color 1
        else
        {
          for (tx=0; tx<cx; tx++)
          {
            uc = d[0];
            if (pix & ucSrcMask) // clr pixel in source, clr it in dest
              d[0] = (uc & ~ucDstMask);
            d++; // next pixel column
            ucSrcMask >>= 1;
            if (ucSrcMask == 0) // read next byte
            {
                ucSrcMask = 0x80;
                pix = *s++;
            }
          } // for tx
        } // priority color 0
        dy++;
        pSprite += iPitch;
    } // for ty
} /* bbeiDrawSprite() */

// Set (or clear) an individual pixel
// The local copy of the frame buffer is used to avoid
// reading data from the display controller
int bbeiSetPixel(BBEIDISP *pBBEI, int x, int y, unsigned char ucColor)
{
int i;
unsigned char uc, ucOld;
int iPitch, iSize;

    iPitch = pBBEI->width;
    iSize = iPitch * ((pBBEI->height+7)/8);

    i = ((y >> 3) * iPitch) + x;
    if (i < 0 || i > iSize-1) { // off the screen
        return BBEI_ERROR_BAD_PARAMETER;
    }
    // Special case for 4-color e-ink
    if (pBBEI->iFlags & BBEI_4COLOR) {
        uint8_t ucMask = (1 << (y & 7));
        ucColor ^= 1; // invert low bit
        if (ucColor & 1)
           pBBEI->ucScreen[i] |= ucMask;
        else
           pBBEI->ucScreen[i] &= ~ucMask;
        if (ucColor & 2)
            pBBEI->ucScreen[iSize + i] |= ucMask;
        else
            pBBEI->ucScreen[iSize + i] &= ~ucMask;
        return BBEI_SUCCESS;
    }
    // special case for 3-color e-ink
    if (pBBEI->iFlags & BBEI_3COLOR) {
        if (ucColor >= BBEI_YELLOW) { // yellow/red has priority
            pBBEI->ucScreen[iSize + i] |= (1 << (y & 7));
        } else {
            pBBEI->ucScreen[iSize + i] &= ~(1 << (y & 7)); // clear red plane bit
            if (ucColor == BBEI_WHITE) {
                pBBEI->ucScreen[i] &= ~(1 << (y & 7));
            } else { // must be black
                pBBEI->ucScreen[i] |= (1 << (y & 7));
            }
        }
        return BBEI_SUCCESS;
    }
  bbeiSetPosition(pBBEI, x, y, 8, 1);

  if (pBBEI->ucScreen)
    uc = ucOld = pBBEI->ucScreen[i];
  else
     uc = ucOld = 0;

  uc &= ~(0x1 << (y & 7));
  if (ucColor)
  {
    uc |= (0x1 << (y & 7));
  }
  if (uc != ucOld) // pixel changed
  {
//    obdSetPosition(x, y>>3);
    if (pBBEI->ucScreen)
    {
      bbeiWriteData(pBBEI, &uc, 1);
      pBBEI->ucScreen[i] = uc;
    }
  }
  return BBEI_SUCCESS;
} /* obdSetPixel() */

//
// Invert font data
//
void InvertBytes(uint8_t *pData, uint8_t bLen)
{
uint8_t i;
   for (i=0; i<bLen; i++)
   {
      *pData = ~(*pData);
      pData++;
   }
} /* InvertBytes() */

//
// Load a 1-bpp Windows bitmap
// Pass the pointer to the beginning of the BMP file
// If the FG == BG color, it will
// draw the 1's bits as the FG color and leave
// the background (0 pixels) unchanged - aka transparent.
//
int bbeiLoadBMP(BBEIDISP *pBBEI, const uint8_t *pBMP, int dx, int dy, int iFG, int iBG)
{
int16_t i16, cx, cy;
int iOffBits; // offset to bitmap data
int x, y, iPitch;
uint8_t b=0, *s, *d=NULL;
uint8_t ucFill, dst_mask=0, src_mask;
uint8_t bFlipped = 0;

    if (pBBEI->ucScreen == NULL) {
        // no BG specified or no back buffer, BG color is opposite of FG
        if (iFG == -1) iFG = BBEI_WHITE;
        if (iBG == -1) iBG = BBEI_BLACK;
    }
    // Don't use pgm_read_word because it can cause an unaligned
    // access on RP2040 for odd addresses
  i16 = pgm_read_byte(pBMP);
  i16 += (pgm_read_byte(&pBMP[1]) << 8);
  if (i16 != 0x4d42) // must start with 'BM'
     return BBEI_ERROR_BAD_DATA; // not a BMP file
  cx = pgm_read_byte(pBMP + 18);
    cx += (pgm_read_byte(pBMP+19)<<8);
  if (cx + dx > pBBEI->width) // must fit on the display
     return BBEI_ERROR_BAD_PARAMETER;
  cy = pgm_read_byte(pBMP + 22);
    cy += (pgm_read_byte(pBMP+23)<<8);
  if (cy < 0) cy = -cy;
  else bFlipped = 1;
  if (cy + dy > pBBEI->height) // must fit on the display
     return BBEI_ERROR_BAD_PARAMETER;
  i16 = pgm_read_byte(pBMP + 28);
    i16 += (pgm_read_byte(pBMP+29)<<8);
  if (i16 != 1) // must be 1 bit per pixel
     return BBEI_ERROR_BAD_DATA;
  iOffBits = pgm_read_byte(pBMP + 10);
    iOffBits += (pgm_read_byte(pBMP+11));
  iPitch = (((cx+7)>>3) + 3) & 0xfffc; // must be DWORD aligned
  if (bFlipped)
  {
    iOffBits += ((cy-1) * iPitch); // start from bottom
    iPitch = -iPitch;
  }
    ucFill = (iBG == BBEI_WHITE && pBBEI->type < EPD42_400x300) ? iBG : 0xff;
    if (!pBBEI->ucScreen || iFG >= BBEI_YELLOW) { // this will override the B/W plane, so invert things
        ucFill = 0x00;
        x = iFG;
        iFG = iBG;
        iBG = x; // swap colors
    }
  for (y=0; y<cy; y++)
  {
     if (!pBBEI->ucScreen)
     {
         dst_mask = (1 << (y & 7));
        d = u8Cache;
        if ((y & 7) == 0)
           memset(u8Cache, ucFill, sizeof(u8Cache));
     }
     s = (uint8_t *)&pBMP[iOffBits + (y*iPitch)];
     src_mask = 0;
      if (!pBBEI->ucScreen) // direct to display
      {
         for (x=0; x<cx; x++)
         {
            if (src_mask == 0) // need to load the next byte
            {
               b = pgm_read_byte(s++);
               src_mask = 0x80; // MSB on left
            }
            if (b & src_mask)
            {
               if (iFG == BBEI_WHITE)
                  d[0] &= ~dst_mask;
               else
                  d[0] |= dst_mask;
            } else {
                if (iBG == BBEI_BLACK)
                   d[0] |= dst_mask;
                else
                   d[0] &= ~dst_mask;
            }
            d++;
            src_mask >>= 1;
         } // for x
      } else { // use the setPixel function for more features
          for (x=0; x<cx; x++)
          {
             if (src_mask == 0) // need to load the next byte
             {
                b = pgm_read_byte(s++);
                src_mask = 0x80; // MSB on left
             }
             if (b & src_mask)
             {
                 if (iFG >= 0)
                     bbeiSetPixel(pBBEI, dx+x, dy+y, (uint8_t)iFG);
             } else {
                 if (iBG >= 0)
                     bbeiSetPixel(pBBEI, dx+x, dy+y, (uint8_t)iBG);
             }
             src_mask >>= 1;
          } // for x
      }
     if (pBBEI->ucScreen == NULL && ((y & 7) == 7 || y == cy-1)) // dump to LCD
     {
       bbeiSetPosition(pBBEI, dx, y+dy, cx, 1);
       bbeiWriteData(pBBEI, u8Cache, cx);
     }
  } // for y
  return BBEI_SUCCESS;
} /* bbeiLoadBMP() */
//
// Load a 4-bpp Windows bitmap for a 3-color bitmap
// Pass the pointer to the beginning of the BMP file
// First pass version assumes a full screen bitmap
//
int bbeiLoadBMP3(BBEIDISP *pBBEI, const uint8_t *pBMP, int dx, int dy)
{
int16_t i16, cx, cy, bpp;
int x, y, iOffBits; // offset to bitmap data
int iPitch, iDestPitch;
int iRedOff, iColors, iPalOff;
uint8_t uc, b, *s, *d;
uint8_t dst_mask;
uint8_t bFlipped = 0;
uint8_t ucColorMap[16];
    
    if (!(pBBEI->iFlags & BBEI_3COLOR) || pBBEI->ucScreen == 0)
        return BBEI_ERROR_NOT_SUPPORTED; // if not 3-color EPD or no back buffer, bye-byte
    iDestPitch = pBBEI->width;
    iRedOff = ((pBBEI->height+7)>>3) * iDestPitch;
    // Need to avoid pgm_read_word because it can cause an
    // unaligned address exception on the RP2040 for odd addresses
    i16 = pgm_read_byte(pBMP);
    i16 += (pgm_read_byte(pBMP+1)<<8);
    if (i16 != 0x4d42) // must start with 'BM'
        return BBEI_ERROR_BAD_DATA; // not a BMP file
    cx = pgm_read_byte(&pBMP[18]);
    cx += (pgm_read_byte(&pBMP[19]) << 8);
    if (cx + dx > pBBEI->width) // must fit on the display
        return BBEI_ERROR_BAD_PARAMETER;
    cy = pgm_read_byte(&pBMP[22]);
    cy += (pgm_read_byte(&pBMP[23])<<8);
    if (cy < 0)
        cy = -cy;
    else
        bFlipped = 1;
    if (cy + dy > pBBEI->height) // must fit on the display
        return BBEI_ERROR_BAD_PARAMETER;
    if (pgm_read_byte(&pBMP[30]) != 0) // compression must be NONE
        return BBEI_ERROR_BAD_DATA;
    bpp = pgm_read_byte(&pBMP[28]);
    if (bpp != 4) // must be 4 bits per pixel
        return BBEI_ERROR_BAD_DATA;
    iOffBits = pgm_read_byte(&pBMP[10]);
    iOffBits += (pgm_read_byte(&pBMP[11])<<8);
    iColors = pgm_read_byte(&pBMP[46]); // colors used BMP field
    if (iColors == 0 || iColors > (1<<bpp))
        iColors = (1 << bpp); // full palette
    iPalOff = iOffBits - (4 * iColors); // start of color palette
    iPitch = (((cx+1)>>1) + 3) & 0xfffc; // must be DWORD aligned
    if (bFlipped)
    {
        iOffBits += ((cy-1) * iPitch); // start from bottom
        iPitch = -iPitch;
    }
// Map the colors to white/black/red with a simple quantization. Convert colors to G3R3B2 and find the closest value (red in the middle)
    // white = 0xff, red = 0x1c, black = 0x00
    for (x=0; x<iColors; x++) {
        uint8_t r, g, b, uc;
        b = pgm_read_byte(&pBMP[iPalOff+(x*4)]);
        g = pgm_read_byte(&pBMP[iPalOff+1+(x*4)]);
        r = pgm_read_byte(&pBMP[iPalOff+2+(x*4)]);
        uc = (b >> 6) | ((r >> 5) << 2) | ((g >> 5) << 5);
        if (uc >= 0x1c) { // check for red/white
            ucColorMap[x] = ((0xff - uc) < (uc - 0x1c)) ? BBEI_WHITE : BBEI_RED;
        } else {
            ucColorMap[x] = ((0x1c - uc) < uc) ? BBEI_RED : BBEI_BLACK;
        }
     }
  for (y=0; y<cy; y++)
  {
     dst_mask = 1 << ((y+dy) & 7);
      d = &pBBEI->ucScreen[(((y+dy)>>3)*iDestPitch)+dx];
     s = (uint8_t *)&pBMP[iOffBits+(y*iPitch)];
     for (x=0; x<cx; x+=2) // work with pixel pairs
     {
         b = pgm_read_byte(s++);
         d[x] &= ~dst_mask; // clear b/w & red planes to start as white
         d[x+1] &= ~dst_mask;
         d[x+iRedOff] &= ~dst_mask;
         d[x+1+iRedOff] &= ~dst_mask;
         uc = ucColorMap[b >> 4]; // left pixel
         if (uc == BBEI_BLACK)
             d[x] |= dst_mask;
         else if (uc == BBEI_RED)
             d[x+iRedOff] |= dst_mask; // we made it white already
         uc = ucColorMap[b & 0xf]; // right pixel
         if (uc == BBEI_BLACK)
             d[x+1] |= dst_mask;
         else if (uc == BBEI_RED)
             d[x+1+iRedOff] |= dst_mask;
     } // for x
  } // for y
  return BBEI_SUCCESS;
} /* bbeiLoadBMP3() */
//
// Set the current cursor position
// The column represents the pixel column (0-127)
// The row represents the text row (0-7)
//
void bbeiSetCursor(BBEIDISP *pBBEI, int x, int y)
{
  pBBEI->iCursorX = x;
  pBBEI->iCursorY = y;
} /* bbeiSetCursor() */
//
// Turn text wrap on or off for the oldWriteString() function
//
void bbeiSetTextWrap(BBEIDISP *pBBEI, int bWrap)
{
  pBBEI->wrap = bWrap;
} /* bbeiSetTextWrap() */
//
// Draw a string of BB_FONT characters directly into the EPD framebuffer
//
void bbeiWriteStringCustom(BBEIDISP *pBBEI, BB_FONT *pFont, int x, int y, char *szMsg, int iColor, uint8_t iPlane)
{
int rc, i, h, w, end_y, dx, dy, ty, iSrcPitch, iPitch;
unsigned int c;
uint8_t *s, bInvert = 0;
BB_GLYPH *pGlyph;
uint8_t *pBits, u8CMD1, u8CMD2, u8CMD, u8EndMask;

    if (x == -1)
        x = pBBEI->iCursorX;
    if (y == -1)
        y = pBBEI->iCursorY;
    if (x == CENTER_X) { // center the string on the e-paper
        dx = i = 0;
        while (szMsg[i]) {
          c = szMsg[i++];
          if (c < pFont->first || c > pFont->last) // undefined character
             continue; // skip it
          c -= pFont->first; // first char of font defined
          pGlyph = &pFont->glyphs[c]; // glyph info for this character
          dx += pGlyph->xAdvance;
        }
        x = (pBBEI->native_width - dx)/2;
        if (x < 0) x = 0;
    }
    // Point to the start of the compressed data
    pBits = (uint8_t *)pFont;
    pBits += sizeof(BB_FONT);
    pBits += (pFont->last - pFont->first + 1) * sizeof(BB_GLYPH);
   i = 0;
   while (szMsg[i] && x < pBBEI->native_width && y < pBBEI->native_height) {
      c = szMsg[i++];
      if (c < pFont->first || c > pFont->last) // undefined character
         continue; // skip it
      c -= pFont->first; // first char of font defined
      pGlyph = &pFont->glyphs[c]; // glyph info for this character
       if (pGlyph->width > 1) { // skip this if drawing a space
           dx = x + pGlyph->xOffset; // offset from character UL to start drawing
           dy = y + pGlyph->yOffset;
           s = pBits + pGlyph->bitmapOffset; // start of compressed bitmap data
           if (pFont->rotation == 0 || pFont->rotation == 180) {
               h = pGlyph->height;
               w = pGlyph->width;
           } else { // rotated
               w = pGlyph->height;
               h = pGlyph->width;
           }
           if ((dy + h) > pBBEI->native_height) { // trim it
               h = pBBEI->native_height - dy;
           }
           u8EndMask = 0xff;
           if (w & 7) { // width ends on a partial byte
               u8EndMask <<= (8-(w & 7));
           }
           end_y = dy + h;
           ty = (pGlyph[1].bitmapOffset - (intptr_t)(s - pBits)); // compressed size
           if (ty < 0 || ty > 4096) ty = 4096; // DEBUG
           rc = g5_decode_init(&g5dec, w, h, s, ty);
           if (rc != G5_SUCCESS) return; // corrupt data?
           // set the memory window for this character
           iSrcPitch = (w+7)/8;
           w += (dx & 7); // add parital byte
           bbeiSetPosition(pBBEI, dx, dy, w, h);
           iPitch = (w+7)/8;
           // start writing into the correct plane
           if (pBBEI->chip_type == BBEI_CHIP_UC81xx) {
               u8CMD1 = UC8151_DTM2;
               u8CMD2 = UC8151_DTM1;
           } else {
               u8CMD1 = SSD1608_WRITE_RAM;
               u8CMD2 = SSD1608_WRITE_ALTRAM;
           }
           u8CMD = u8CMD1;
           if (iColor == BBEI_BLACK) bInvert = 1;
           else if (iColor == BBEI_RED && (pBBEI->iFlags & BBEI_3COLOR)) {
               u8CMD = u8CMD2; // second plane is red (inverted)
           }
        bbeiWriteCmd(pBBEI, u8CMD); // memory write command
      for (ty=dy; ty<end_y && ty < pBBEI->native_height; ty++) {
          g5_decode_line(&g5dec, u8Cache);
          u8Cache[iSrcPitch-1] &= u8EndMask; // clean pixels beyond character width
          u8Cache[iSrcPitch] = 0;
          if (dx & 7) { // need to shift it over by 1-7 bits
            uint8_t *s = u8Cache, uc1, uc0 = 0; // last shifted byte
            uint8_t n = dx & 7; // shift amount
            for (int j=0; j<w+7; j+= 8) {
                uc1 = *s;
                uc0 |= (uc1 >> n);
                *s++ = uc0;
                uc0 = uc1 << (8-n);
            }
            *s++ = uc0; // store final byte
            *s++ = 0; // and a zero for good measure
          }
          if (bInvert) InvertBytes(u8Cache, iPitch);
          bbeiWriteData(pBBEI, u8Cache, iPitch); // write each row into the EPD framebuffer
      } // for y
      } // if not drawing a space
      if (pFont->rotation == 0 || pFont->rotation == 180) {
        x += pGlyph->xAdvance; // width of this character
      } else {
        y += pGlyph->xAdvance;
      }
   } // while drawing characters
    pBBEI->iCursorX = x;
    pBBEI->iCursorY = y;
} /* EPDWriteStringCustom() */
//
// Draw a string of normal (8x8), small (6x8) or large (16x32) characters
// At the given col+row
//
int bbeiWriteString(BBEIDISP *pBBEI, int x, int y, char *szMsg, int iSize, int iColor)
{
int i, iFontOff, iLen;
    uint8_t c, *s, ucCMD, ucCMD1, ucCMD2;
int iOldFG; // old fg color to make sure red works
uint8_t u8Temp[128];
    
  if (pBBEI == NULL) return BBEI_ERROR_BAD_PARAMETER;
    if (pBBEI->chip_type == BBEI_CHIP_UC81xx) {
        ucCMD1 = UC8151_DTM2;
        ucCMD2 = UC8151_DTM1;
    } else {
        ucCMD1 = SSD1608_WRITE_RAM;
        ucCMD2 = SSD1608_WRITE_ALTRAM;
    }
    ucCMD = ucCMD1;
    if ((pBBEI->iFlags & BBEI_3COLOR) && iColor == BBEI_RED || pBBEI->iPlane == 1) {
        ucCMD = ucCMD2; // write to second plane for red
    }
    if (x == -1 || y == -1) // use the cursor position
    {
        x = pBBEI->iCursorX; y = pBBEI->iCursorY;
    } else {
        pBBEI->iCursorX = x; pBBEI->iCursorY = y;
    }
    if (pBBEI->iCursorX >= pBBEI->width || pBBEI->iCursorY >= pBBEI->height)
       return BBEI_ERROR_BAD_PARAMETER; // can't draw off the display

    iOldFG = pBBEI->iFG; // save old fg color
    if (iColor >= BBEI_YELLOW) {
       pBBEI->iFG = iColor;
    }
    if (iSize == FONT_8x8) // 8x8 font
    {
       i = 0;
        if (!pBBEI->ucScreen) { // bufferless mode, rotate the coordinate system to fit the situation
 //           bbeiSetPosition(pBBEI, √, pBBEI->iCursorX, 8, pBBEI->native_height-pBBEI->iCursorX);
//            bbeiWriteCmd(pBBEI, ucCMD1); // write to "new" plane
        }
       while (pBBEI->iCursorX < pBBEI->width && szMsg[i] != 0 && pBBEI->iCursorY < pBBEI->height)
       {
         c = (unsigned char)szMsg[i];
         iFontOff = (int)(c-32) * 7;
         // we can't directly use the pointer to FLASH memory, so copy to a local buffer
         u8Temp[0] = 0; // first column is blank
         memcpy_P(&u8Temp[1], &ucFont[iFontOff], 7); // only needed on AVR
         if (iColor == BBEI_BLACK) InvertBytes(u8Temp, 8);
         iLen = 8;
           if (pBBEI->iCursorX + iLen > pBBEI->width) { // clip right edge
               iLen = pBBEI->width - pBBEI->iCursorX;
           }
           if (!pBBEI->ucScreen) { // bufferless mode, rotate the coordinate system to fit the situation
               bbeiSetPosition(pBBEI, pBBEI->native_width-8-pBBEI->iCursorY, pBBEI->iCursorX, 8, pBBEI->native_height-pBBEI->iCursorX);
               bbeiWriteCmd(pBBEI, ucCMD); // write to "new" plane
               bbeiWriteData(pBBEI, u8Temp, iLen);
           } else { // draw in memory
               // DEBUG
           }
         pBBEI->iCursorX += iLen;
        if (pBBEI->iCursorX >= pBBEI->width-7 && pBBEI->wrap) { // word wrap enabled?
           pBBEI->iCursorX = 0; // start at the beginning of the next line
           pBBEI->iCursorY+=8;
         }
         i++;
       } // while
       pBBEI->iFG = iOldFG; // restore color
       return BBEI_SUCCESS;
    } else if (iSize == FONT_12x16) { // 6x8 stretched to 12x16
      i = 0;
        while (pBBEI->iCursorX < pBBEI->width && pBBEI->iCursorY < pBBEI->height && szMsg[i] != 0) {
// stretch the 'normal' font instead of using the big font
              int tx, ty;
              c = szMsg[i] - 32;
              unsigned char uc1, uc2, ucMask, *pDest;
              s = (unsigned char *)&ucSmallFont[(int)c*5];
              u8Temp[0] = 0; // first column is blank
              memcpy_P(&u8Temp[1], s, 6);
              if (iColor == BBEI_BLACK)
                  InvertBytes(u8Temp, 6);
              // Stretch the font to double width + double height
              memset(&u8Temp[6], 0, 24); // write 24 new bytes
              for (tx=0; tx<6; tx++)
              {
                  ucMask = 3;
                  pDest = &u8Temp[6+tx*2];
                  uc1 = uc2 = 0;
                  c = u8Temp[tx];
                  for (ty=0; ty<4; ty++)
                  {
                      if (c & (1 << ty)) // a bit is set
                          uc1 |= ucMask;
                      if (c & (1 << (ty + 4)))
                          uc2 |= ucMask;
                      ucMask <<= 2;
                  }
                  pDest[0] = uc1;
                  pDest[1] = uc1; // double width
                  pDest[12] = uc2;
                  pDest[13] = uc2;
              }
              // smooth the diagonal lines
              for (tx=0; tx<5; tx++)
              {
                  uint8_t c0, c1, ucMask2;
                  c0 = u8Temp[tx];
                  c1 = u8Temp[tx+1];
                  pDest = &u8Temp[6+tx*2];
                  ucMask = 1;
                  ucMask2 = 2;
                  for (ty=0; ty<7; ty++)
                  {
                      if (((c0 & ucMask) && !(c1 & ucMask) && !(c0 & ucMask2) && (c1 & ucMask2)) || (!(c0 & ucMask) && (c1 & ucMask) && (c0 & ucMask2) && !(c1 & ucMask2)))
                      {
                          if (ty < 3) // top half
                          {
                              if (iColor == BBEI_BLACK) {
                                pDest[1] &= ~(1 << ((ty * 2)+1));
                                pDest[2] &= ~(1 << ((ty * 2)+1));
                                pDest[1] &= ~(1 << ((ty+1) * 2));
                                pDest[2] &= ~(1 << ((ty+1) * 2));
                              } else {
                                pDest[1] |= (1 << ((ty * 2)+1));
                                pDest[2] |= (1 << ((ty * 2)+1));
                                pDest[1] |= (1 << ((ty+1) * 2));
                                pDest[2] |= (1 << ((ty+1) * 2));
                              }
                          }
                          else if (ty == 3) // on the border
                          {
                              if (iColor == BBEI_BLACK) {
                                pDest[1] &= ~0x80; pDest[2] &= ~0x80;
                                pDest[13] &= ~1; pDest[14] &= ~1;
                              } else {
                                pDest[1] |= 0x80; pDest[2] |= 0x80;
                                pDest[13] |= 1; pDest[14] |= 1;
                              }
                          }
                          else // bottom half
                          {
                              if (iColor == BBEI_BLACK) {
                                pDest[13] &= ~(1 << (2*(ty-4)+1));
                                pDest[14] &= ~(1 << (2*(ty-4)+1));
                                pDest[13] &= ~(1 << ((ty-3) * 2));
                                pDest[14] &= ~(1 << ((ty-3) * 2));
                              } else {
                                pDest[13] |= (1 << (2*(ty-4)+1));
                                pDest[14] |= (1 << (2*(ty-4)+1));
                                pDest[13] |= (1 << ((ty-3) * 2));
                                pDest[14] |= (1 << ((ty-3) * 2));
                              }
                          }
                      }
                      else if (!(c0 & ucMask) && (c1 & ucMask) && (c0 & ucMask2) && !(c1 & ucMask2))
                      {
                          if (ty < 4) // top half
                          {
                              if (iColor == BBEI_BLACK) {
                                pDest[1] &= ~(1 << ((ty * 2)+1));
                                pDest[2] &= ~(1 << ((ty+1) * 2));
                              } else {
                                pDest[1] |= (1 << ((ty * 2)+1));
                                pDest[2] |= (1 << ((ty+1) * 2));
                              }
                          }
                          else
                          {
                              if (iColor == BBEI_BLACK) {
                                pDest[13] &= ~(1 << (2*(ty-4)+1));
                                pDest[14] &= ~(1 << ((ty-3) * 2));
                              } else {
                                pDest[13] |= (1 << (2*(ty-4)+1));
                                pDest[14] |= (1 << ((ty-3) * 2));
                              }
                          }
                      }
                      ucMask <<= 1; ucMask2 <<= 1;
                  }
              }
              iLen = 12;
              if (pBBEI->iCursorX + iLen > pBBEI->width) // clip right edge
                  iLen = pBBEI->width - pBBEI->iCursorX;
            if (!pBBEI->ucScreen) { // bufferless mode
                bbeiSetPosition(pBBEI, pBBEI->native_width-8-pBBEI->iCursorY, pBBEI->iCursorX, 8, iLen);
                bbeiWriteCmd(pBBEI, ucCMD); // write to "new" plane
                bbeiWriteData(pBBEI, &u8Temp[6], iLen);
                bbeiSetPosition(pBBEI, pBBEI->native_width-16-pBBEI->iCursorY, pBBEI->iCursorX, 8, iLen);
                bbeiWriteCmd(pBBEI, ucCMD); // write to "new" plane
                bbeiWriteData(pBBEI, &u8Temp[18], iLen);
            } else { // write to RAM
                // DEBUG
            }
              pBBEI->iCursorX += iLen;
              if (pBBEI->iCursorX >= pBBEI->width-11 && pBBEI->wrap) // word wrap enabled?
              {
                pBBEI->iCursorX = 0; // start at the beginning of the next line
                pBBEI->iCursorY += 16;
              }
          i++;
      } // while
       pBBEI->iFG = iOldFG; // restore color
       return BBEI_SUCCESS;
    } // 12x16
    else if (iSize == FONT_6x8)
    {
       i = 0;
       while (pBBEI->iCursorX < pBBEI->width && pBBEI->iCursorY < pBBEI->height && szMsg[i] != 0)
       {
               c = szMsg[i] - 32;
               // we can't directly use the pointer to FLASH memory, so copy to a local buffer
               u8Temp[0] = 0; // first column is blank
               memcpy_P(&u8Temp[1], &ucSmallFont[(int)c*5], 5);
               if (iColor == BBEI_BLACK) InvertBytes(u8Temp, 6);
               iLen = 6;
               if (pBBEI->iCursorX + iLen > pBBEI->width) // clip right edge
                   iLen = pBBEI->width - pBBEI->iCursorX;
           if (!pBBEI->ucScreen) { // bufferless mode
               bbeiSetPosition(pBBEI, pBBEI->iCursorX, pBBEI->iCursorY, 8, iLen);
               bbeiWriteCmd(pBBEI, ucCMD); // write to "new" plane
               bbeiWriteData(pBBEI, u8Temp, iLen);
           } else { // write to RAM
               // DEBUG
           }
               pBBEI->iCursorX += iLen;
               if (pBBEI->iCursorX >= pBBEI->width-5 && pBBEI->wrap) // word wrap enabled?
               {
                 pBBEI->iCursorX = 0; // start at the beginning of the next line
                 pBBEI->iCursorY +=8;
               }
         i++;
       }
      pBBEI->iFG = iOldFG; // restore color
      return BBEI_SUCCESS;
    } // 6x8
  pBBEI->iFG = iOldFG; // restore color
  return BBEI_ERROR_BAD_PARAMETER; // invalid size
} /* bbeiWriteString() */
//
// Get the width of text in a custom font
//
void bbeiGetStringBox(BB_FONT *pFont, const char *szMsg, int *width, int *top, int *bottom)
{
int cx = 0;
unsigned int c, i = 0;
BB_GLYPH *pBBG;
int miny, maxy;

   if (width == NULL || top == NULL || bottom == NULL || pFont == NULL || szMsg == NULL) return; // bad pointers
   miny = 1000; maxy = 0;
   while (szMsg[i]) {
      c = szMsg[i++];
      if (c < pFont->first || c > pFont->last) // undefined character
         continue; // skip it
      c -= pFont->first; // first char of font defined
      pBBG = &pFont->glyphs[c];
      cx += pBBG->xAdvance;
      if (pBBG->yOffset < miny) miny = pBBG->yOffset;
      if (pBBG->height+pBBG->yOffset > maxy) maxy = pBBG->height+pBBG->yOffset;
   }
   *width = cx;
   *top = miny;
   *bottom = maxy;
} /* bbeiGetStringBox() */

void bbeiAllocBuffer(BBEIDISP *pBBEI)
{
  pBBEI->ucScreen = (uint8_t *)malloc(pBBEI->width * ((pBBEI->height+7)>>3));
  pBBEI->iScreenOffset = 0;
} /* obdAllocBuffer() */

void bbeiDrawLine(BBEIDISP *pBBEI, int x1, int y1, int x2, int y2, uint8_t ucColor)
{
  int temp;
  int dx = x2 - x1;
  int dy = y2 - y1;
  int error;
  uint8_t *p, *pStart, ucFill = 0, mask, bOld, bNew;
  int xinc, yinc;
  int y, x;
  int iPitch = pBBEI->width;
  int iRedOffset = 0;

  if (pBBEI == NULL) return;

  if (x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0 || x1 >= pBBEI->width || x2 >= pBBEI->width || y1 >= pBBEI->height || y2 >= pBBEI->height)
     return;
  if (ucColor >= BBEI_YELLOW && pBBEI->iFlags & (BBEI_3COLOR | BBEI_4COLOR)) {
        // use the second half of the image buffer
        iRedOffset = pBBEI->width * ((pBBEI->height+7)/8);
    }
    if (!pBBEI->ucScreen) { // no back buffer, draw in local buffer
        if (pBBEI->type >= EPD42_400x300) {
            // no back buffer on EPD means we may need to invert the color
            if (ucColor >= BBEI_YELLOW)
                ucColor = BBEI_BLACK;
            else
                ucColor = 1-ucColor; // swap black/white
        }
        if (ucColor == BBEI_BLACK) // fill with opposite color
            ucFill = 0;
        else
            ucFill = 0xff;
        memset(u8Cache, ucFill, sizeof(u8Cache));
    }
  if(abs(dx) > abs(dy)) {
    // X major case
    if(x2 < x1) {
      dx = -dx;
      temp = x1;
      x1 = x2;
      x2 = temp;
      temp = y1;
      y1 = y2;
      y2 = temp;
    }

    y = y1;
    dy = (y2 - y1);
    error = dx >> 1;
    yinc = 1;
    if (dy < 0)
    {
      dy = -dy;
      yinc = -1;
    }
      if (pBBEI->ucScreen) {
          p = pStart = &pBBEI->ucScreen[iRedOffset + x1 + ((y >> 3) * iPitch)]; // point to current spot in back buffer
      } else { // no back buffer, draw directly to the display RAM
          p = pStart = u8Cache;
      }
    mask = 1 << (y & 7); // current bit offset
    for(x=x1; x1 <= x2; x1++) {
      if (ucColor == BBEI_BLACK)
        *p++ |= mask; // set pixel and increment x pointer
      else
        *p++ &= ~mask;
      error -= dy;
      if (error < 0)
      {
        error += dx;
        if (yinc > 0)
           mask <<= 1;
        else
           mask >>= 1;
        if (mask == 0) // we've moved outside the current row, write the data we changed
        {
            bbeiSetPosition(pBBEI, x, y, 8*(int)(p-pStart), 1);
            bbeiWriteData(pBBEI, pStart,  (int)(p-pStart)); // write the row we changed
           x = x1+1; // we've already written the byte at x1
           y1 = y+yinc;
            if (pBBEI->ucScreen) {
                p += (yinc > 0) ? iPitch : -iPitch;
                pStart = p;
            } else { // no buffer
                memset(pStart, ucFill, (int)(p-pStart));
                p = pStart;
            }
           mask = 1 << (y1 & 7);
        }
        y += yinc;
      }
    } // for x1
   if (p != pStart) // some data needs to be written
   {
     bbeiSetPosition(pBBEI, x, y, 8*(int)(p-pStart), 1);
     bbeiWriteData(pBBEI, pStart, (int)(p-pStart));
   }
  } else {
    // Y major case
    if(y1 > y2) {
      dy = -dy;
      temp = x1;
      x1 = x2;
      x2 = temp;
      temp = y1;
      y1 = y2;
      y2 = temp;
    }
      if (pBBEI->ucScreen) {
          p = &pBBEI->ucScreen[iRedOffset + x1 + ((y1 >> 3) * iPitch)]; // point to current spot in back buffer
          bOld = bNew = p[0]; // current pixels
      } else {
          p = u8Cache;
          bOld = bNew = ucFill;
      }
    mask = 1 << (y1 & 7); // current bit offset
    dx = (x2 - x1);
    error = dy >> 1;
    xinc = 1;
    if (dx < 0)
    {
      dx = -dx;
      xinc = -1;
    }
    for(x = x1; y1 <= y2; y1++) {
      if (ucColor == BBEI_BLACK)
        bNew |= mask; // set the pixel
      else
        bNew &= ~mask;
      error -= dx;
      mask <<= 1; // y1++
      if (mask == 0) // we're done with this byte, write it if necessary
      {
        if (bOld != bNew)
        {
            p[0] = bNew; // save to RAM
            bbeiSetPosition(pBBEI, x, y1, 8, 1);
            bbeiWriteData(pBBEI, &bNew, 1);
        } // data changed
        if (pBBEI->ucScreen) {
            p += iPitch; // next line
            bOld = bNew = p[0];
        } else {
            bOld = bNew = ucFill;
        }
        mask = 1; // start at LSB again
      }
      if (error < 0)
      {
        error += dy;
        if (bOld != bNew) // write the last byte we modified if it changed
        {
            p[0] = bNew; // save to RAM
            bbeiSetPosition(pBBEI, x, y1, 8, 1);
            bbeiWriteData(pBBEI, &bNew, 1);
        }
          x += xinc;
          if (pBBEI->ucScreen) {
              p += xinc;
              bOld = bNew = p[0];
          } else {
              bOld = bNew = ucFill;
          }
      }
    } // for y
    if (bOld != bNew) // write the last byte we modified if it changed
    {
      p[0] = bNew; // save to RAM
        bbeiSetPosition(pBBEI, x, y2, 8, 1);
        bbeiWriteData(pBBEI, &bNew, 1);
    }
  } // y major case
} /* bbeiDrawLine() */

//
// For drawing ellipses, a circle is drawn and the x and y pixels are scaled by a 16-bit integer fraction
// This function draws a single pixel and scales its position based on the x/y fraction of the ellipse
//
static void DrawScaledPixel(BBEIDISP *pBBEI, int iCX, int iCY, int x, int y, int32_t iXFrac, int32_t iYFrac, uint8_t ucColor)
{
    uint8_t *d, ucMask;
    int iPitch;
    int iRedOffset = 0;

    iPitch = pBBEI->width;
    if (ucColor >= BBEI_YELLOW && pBBEI->iFlags & (BBEI_3COLOR | BBEI_4COLOR)) {
          // use the second half of the image buffer
          iRedOffset = pBBEI->width * ((pBBEI->height+7)/8);
      }
    if (iXFrac != 0x10000) x = ((x * iXFrac) >> 16);
    if (iYFrac != 0x10000) y = ((y * iYFrac) >> 16);
    x += iCX; y += iCY;
    if (x < 0 || x >= pBBEI->width || y < 0 || y >= pBBEI->height)
        return; // off the screen
    d = &pBBEI->ucScreen[iRedOffset + ((y >> 3)*iPitch) + x];
    ucMask = 1 << (y & 7);
    if (ucColor)
        *d |= ucMask;
    else
        *d &= ~ucMask;
} /* DrawScaledPixel() */
//
// For drawing filled ellipses
//
static void DrawScaledLine(BBEIDISP *pBBEI, int iCX, int iCY, int x, int y, int32_t iXFrac, int32_t iYFrac, uint8_t ucColor)
{
    int iLen, x2;
    uint8_t *d, ucMask;
    int iPitch;
    int iRedOffset = 0;

    if (ucColor >= BBEI_YELLOW && pBBEI->iFlags & (BBEI_3COLOR | BBEI_4COLOR)) {
          // use the second half of the image buffer
          iRedOffset = pBBEI->width * ((pBBEI->height+7)/8);
      }
    iPitch = pBBEI->width;
    if (iXFrac != 0x10000) x = ((x * iXFrac) >> 16);
    if (iYFrac != 0x10000) y = ((y * iYFrac) >> 16);
    iLen = x*2;
    x = iCX - x; y += iCY;
    x2 = x + iLen;
    if (y < 0 || y >= pBBEI->height)
        return; // completely off the screen
    if (x < 0) x = 0;
    if (x2 >= pBBEI->width) x2 = pBBEI->width-1;
    iLen = x2 - x + 1; // new length
    d = &pBBEI->ucScreen[iRedOffset + ((y >> 3)*iPitch) + x];
    ucMask = 1 << (y & 7);
    if (ucColor) // white
    {
        for (; iLen > 0; iLen--)
            *d++ |= ucMask;
    }
    else // black
    {
        for (; iLen > 0; iLen--)
            *d++ &= ~ucMask;
    }
} /* DrawScaledLine() */
//
// Draw the 8 pixels around the Bresenham circle
// (scaled to make an ellipse)
//
static void BresenhamCircle(BBEIDISP *pBBEI, int iCX, int iCY, int x, int y, int32_t iXFrac, int32_t iYFrac, uint8_t ucColor, uint8_t bFill)
{
    if (bFill) // draw a filled ellipse
    {
        // for a filled ellipse, draw 4 lines instead of 8 pixels
        DrawScaledLine(pBBEI, iCX, iCY, x, y, iXFrac, iYFrac, ucColor);
        DrawScaledLine(pBBEI, iCX, iCY, x, -y, iXFrac, iYFrac, ucColor);
        DrawScaledLine(pBBEI, iCX, iCY, y, x, iXFrac, iYFrac, ucColor);
        DrawScaledLine(pBBEI, iCX, iCY, y, -x, iXFrac, iYFrac, ucColor);
    }
    else // draw 8 pixels around the edges
    {
        DrawScaledPixel(pBBEI, iCX, iCY, x, y, iXFrac, iYFrac, ucColor);
        DrawScaledPixel(pBBEI, iCX, iCY, -x, y, iXFrac, iYFrac, ucColor);
        DrawScaledPixel(pBBEI, iCX, iCY, x, -y, iXFrac, iYFrac, ucColor);
        DrawScaledPixel(pBBEI, iCX, iCY, -x, -y, iXFrac, iYFrac, ucColor);
        DrawScaledPixel(pBBEI, iCX, iCY, y, x, iXFrac, iYFrac, ucColor);
        DrawScaledPixel(pBBEI, iCX, iCY, -y, x, iXFrac, iYFrac, ucColor);
        DrawScaledPixel(pBBEI, iCX, iCY, y, -x, iXFrac, iYFrac, ucColor);
        DrawScaledPixel(pBBEI, iCX, iCY, -y, -x, iXFrac, iYFrac, ucColor);
    }
} /* BresenhamCircle() */

//
// Draw an outline or filled ellipse
//
void bbeiEllipse(BBEIDISP *pBBEI, int iCenterX, int iCenterY, int32_t iRadiusX, int32_t iRadiusY, uint8_t ucColor, uint8_t bFilled)
{
    int32_t iXFrac, iYFrac;
    int iRadius, iDelta, x, y;
    
    if (pBBEI == NULL || pBBEI->ucScreen == NULL)
        return; // must have back buffer defined

    if (iRadiusX <= 0 || iRadiusY <= 0) return; // invalid radii
    
    if (iRadiusX > iRadiusY) // use X as the primary radius
    {
        iRadius = iRadiusX;
        iXFrac = 65536;
        iYFrac = (iRadiusY * 65536) / iRadiusX;
    }
    else
    {
        iRadius = iRadiusY;
        iXFrac = (iRadiusX * 65536) / iRadiusY;
        iYFrac = 65536;
    }
    iDelta = 3 - (2 * iRadius);
    x = 0; y = iRadius;
    while (x <= y)
    {
        BresenhamCircle(pBBEI, iCenterX, iCenterY, x, y, iXFrac, iYFrac, ucColor, bFilled);
        x++;
        if (iDelta < 0)
        {
            iDelta += (4*x) + 6;
        }
        else
        {
            iDelta += 4 * (x-y) + 10;
            y--;
        }
    }
} /* bbeiEllipse() */
//
// Draw an outline or filled rectangle
//
void bbeiRectangle(BBEIDISP *pBBEI, int x1, int y1, int x2, int y2, uint8_t ucColor, uint8_t bFilled)
{
    uint8_t *d, ucMask, ucMask2;
    int tmp, iOff;
    int iPitch;
    int iRedOffset = 0;

    if (ucColor >= BBEI_YELLOW) {
        if (pBBEI->iFlags & (BBEI_3COLOR | BBEI_4COLOR)) {
          // use the second half of the image buffer
          iRedOffset = pBBEI->width * ((pBBEI->height+7)/8);
        } else { // force red to black if not present
            ucColor = BBEI_BLACK;
        }
    }

    if (pBBEI == NULL || pBBEI->ucScreen == NULL)
        return; // only works with a back buffer

    if (x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0 ||
       x1 >= pBBEI->width || y1 >= pBBEI->height || x2 >= pBBEI->width || y2 >= pBBEI->height) return; // invalid coordinates
    iPitch = pBBEI->width;
    // Make sure that X1/Y1 is above and to the left of X2/Y2
    // swap coordinates as needed to make this true
    if (x2 < x1)
    {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y2 < y1)
    {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }
    if (bFilled)
    {
        int x, y, iMiddle;
        iMiddle = (y2 >> 3) - (y1 >> 3);
        ucMask = 0xff << (y1 & 7);
        if (iMiddle == 0) // top and bottom lines are in the same row
            ucMask &= (0xff >> (7-(y2 & 7)));
        d = &pBBEI->ucScreen[iRedOffset + (y1 >> 3)*iPitch + x1];
        // Draw top
        for (x = x1; x <= x2; x++)
        {
            if (ucColor)
                *d |= ucMask;
            else
                *d &= ~ucMask;
            d++;
        }
        if (iMiddle > 1) // need to draw middle part
        {
            ucMask = (ucColor) ? 0xff : 0x00;
            for (y=1; y<iMiddle; y++)
            {
                d = &pBBEI->ucScreen[iRedOffset + (y1 >> 3)*iPitch + x1 + (y*iPitch)];
                for (x = x1; x <= x2; x++)
                    *d++ = ucMask;
            }
        }
        if (iMiddle >= 1) // need to draw bottom part
        {
            ucMask = 0xff >> (7-(y2 & 7));
            d = &pBBEI->ucScreen[iRedOffset + (y2 >> 3)*iPitch + x1];
            for (x = x1; x <= x2; x++)
            {
                if (ucColor)
                    *d++ |= ucMask;
                else
                    *d++ &= ~ucMask;
            }
        }
    }
    else // outline
    {
      // see if top and bottom lines are within the same byte rows
        d = &pBBEI->ucScreen[iRedOffset + (y1 >> 3)*iPitch + x1];
        if ((y1 >> 3) == (y2 >> 3))
        {
            ucMask2 = 0xff << (y1 & 7);  // L/R end masks
            ucMask = 1 << (y1 & 7);
            ucMask |= 1 << (y2 & 7);
            ucMask2 &= (0xff >> (7-(y2  & 7)));
            if (ucColor)
            {
                *d++ |= ucMask2; // start
                x1++;
                for (; x1 < x2; x1++)
                    *d++ |= ucMask;
                if (x1 <= x2)
                    *d++ |= ucMask2; // right edge
            }
            else
            {
                *d++ &= ~ucMask2;
                x1++;
                for (; x1 < x2; x1++)
                    *d++ &= ~ucMask;
                if (x1 <= x2)
                    *d++ &= ~ucMask2; // right edge
            }
        }
        else
        {
            int y;
            // L/R sides
            iOff = (x2 - x1);
            ucMask = 1 << (y1 & 7);
            for (y=y1; y <= y2; y++)
            {
                if (ucColor) {
                    *d |= ucMask;
                    d[iOff] |= ucMask;
                } else {
                    *d &= ~ucMask;
                    d[iOff] &= ~ucMask;
                }
                ucMask <<= 1;
                if  (ucMask == 0) {
                    ucMask = 1;
                    d += iPitch;
                }
            }
            // T/B sides
            ucMask = 1 << (y1 & 7);
            ucMask2 = 1 << (y2 & 7);
            x1++;
            d = &pBBEI->ucScreen[iRedOffset + (y1 >> 3)*iPitch + x1];
            iOff = (y2 >> 3) - (y1 >> 3);
            iOff *= iPitch;
            for (; x1 < x2; x1++)
            {
                if (ucColor) {
                    *d |= ucMask;
                    d[iOff] |= ucMask2;
                } else {
                    *d &= ~ucMask;
                    d[iOff] &= ~ucMask2;
                }
                d++;
            }
        }
    } // outline
} /* bbeiRectangle() */

#endif // __BB_EI_GFX__

