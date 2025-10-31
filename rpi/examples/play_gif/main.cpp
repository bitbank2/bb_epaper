//
// C++ example for bb_epaper library
// written by Larry Bank (bitbank@pobox.com)
// Project started 9/23/2024
// Copyright (c) 2025 BitBank Software, Inc.
//
#include <unistd.h>
#include <bb_epaper.h>
#include <AnimatedGIF.h>
#include "horse_214x122.h"
BBEPAPER bbep(EP29Z_128x296);
AnimatedGIF gif;

#define INVERT

// BCM GPIO numbers used by Pimoroni e-paper "HATs"
//#define PIN_DC 22
//#define PIN_RST 27
//#define PIN_BUSY 17
//#define PIN_CS 8
//#define SPI_BUS 0
//#define PIN_PWR 0xff
// BCM GPIO numbers for the Waveshare e-paper HAT
#define PIN_DC 25
#define PIN_CS 8
#define PIN_RST 17
#define PIN_BUSY 24
#define PIN_PWR 18
#define SPI_BUS 0
static uint8_t *pBuffer;
static int iPitch;
//
// This doesn't have to be super efficient
//
void DrawPixel(int x, int y, uint8_t ucColor)
{
uint8_t ucMask;
int index;

  if (x >= bbep.width() || y >= bbep.height())
     return;
  ucMask = 0x80 >> (x & 7);
  index = (x >> 3) + (y * iPitch);
#ifdef INVERT
  if (ucColor)
     pBuffer[index] &= ~ucMask;
  else
     pBuffer[index] |= ucMask;
#else
  if (ucColor)
     pBuffer[index] |= ucMask;
  else
     pBuffer[index] &= ~ucMask;
#endif
}
// Draw a line of image directly on the LCD

void GIFDraw(GIFDRAW *pDraw)
{
    uint8_t *s;
    int x, y, iWidth;
    static uint8_t ucPalette[256]; // thresholded palette

    if (pDraw->y == 0) // first line, convert palette to 0/1
    {
      for (x = 0; x < 256; x++)
      {
        uint16_t usColor = pDraw->pPalette[x];
        int gray = (usColor & 0xf800) >> 8; // red
        gray += ((usColor & 0x7e0) >> 2); // plus green*2
        gray += ((usColor & 0x1f) << 3); // plus blue
        ucPalette[x] = (gray >> 9); // 0->511 = 0, 512->1023 = 1
      }
    }
    y = pDraw->iY + pDraw->y; // current line
    iWidth = pDraw->iWidth;
    if (iWidth > bbep.width())
       iWidth = bbep.width();

    s = pDraw->pPixels;
    if (pDraw->ucDisposalMethod == 2) // restore to background color
    {
      for (x=0; x<iWidth; x++)
      {
        if (s[x] == pDraw->ucTransparent)
           s[x] = pDraw->ucBackground;
      }
      pDraw->ucHasTransparency = 0;
    }
    // Apply the new pixels to the main image
    if (pDraw->ucHasTransparency) // if transparency used
    {
      uint8_t c, ucTransparent = pDraw->ucTransparent;
      int x;
      for(x=0; x < iWidth; x++)
      {
        c = *s++;
        if (c != ucTransparent)
             DrawPixel(pDraw->iX + x, y, ucPalette[c]);
      }
    }
    else
    {
      s = pDraw->pPixels;
      // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
      for (x=0; x<pDraw->iWidth; x++)
        DrawPixel(pDraw->iX + x, y, ucPalette[*s++]);
    }
} /* GIFDraw() */

int main(int argc, const char * argv[]) {

  if (PIN_PWR != 0xff) {
     pinMode(PIN_PWR, OUTPUT);
     digitalWrite(PIN_PWR, 1);
  }
  bbep.initIO(PIN_DC, PIN_RST, PIN_BUSY, PIN_CS, SPI_BUS, 0, 8000000);
  bbep.allocBuffer(); // draw into RAM first
  if (bbep.width() < bbep.height()) {
      bbep.setRotation(90);
  }
  pBuffer = (uint8_t *)bbep.getBuffer(); // point to our copy of the EPD framebuffer
  iPitch = (bbep.width() + 7) / 8;

  bbep.fillScreen(BBEP_WHITE);
  bbep.setFont(FONT_12x16);
  bbep.setTextColor(BBEP_BLACK);
  bbep.drawString("bb_epaper GIF demo", 0, 0);
  bbep.writePlane(PLANE_DUPLICATE); // push the pixels from our RAM buffer to the e-epaper
  bbep.refresh(REFRESH_FULL); // do a full refresh (only type possible on B/W/R)
  usleep(3000000);
  bbep.fillScreen(BBEP_WHITE);
  bbep.sleep(DEEP_SLEEP); // turn off the epaper power circuit
  if (gif.open((uint8_t *)horse_214x122, sizeof(horse_214x122), GIFDraw))
  {
    printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    for (int i=0; i<4; i++) { // play the loop 4 times
        while (gif.playFrame(false, NULL)) {
            bbep.writePlane(); // write it to the EPD memory
            bbep.refresh(REFRESH_PARTIAL);
        } // while
        gif.reset(); // start from frame 0 again
    } // for i
    gif.close();
  }
  bbep.sleep(DEEP_SLEEP);
  if (PIN_PWR != 0xff) {
     digitalWrite(PIN_PWR, 0);
  }
  return 0;
} /* main() */


