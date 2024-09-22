//
// Compressed font example
// written by Larry Bank
//
// This sketch demonstrates how to work with BBF (BitBank Font) files
// and render the characters to a color LCD display
// This should definitely be considered an advanced topic. Some of this functionality will be baked into
// my display libraries eventually, but the paid vs free divide makes it necessary to demonstrate how to
// work with the compressed font files created by the Group5 fontconvert tool
//
#include <Group5.h>
#include "bb_font.h"
#include <bb_spi_lcd.h>
#include "Roboto_Black_40.h" // BBF (BitBank Font file included as hex data to compile with the sketch)

BB_SPI_LCD lcd; // class instance of bb_spi_lcd display library
G5DECODER g5dec; // class instance of Group5 image decoder
uint16_t u16Temp[256]; // holds one line of a character being displayed

//
// Draw a string of characters in the given font at the given position and color
// The y coordinate is relative to the baseline of the character, not the top
// This means that drawing at coordinate 0,0 on the display will usually get clipped 100%
// For this example, a 40pt font is about 64 pixels tall, so the baseline should be top+64
//
void DrawString(const char *szMsg, const BB_FONT *pFont, int x, int y, uint16_t u16FG, uint16_t u16BG)
{
int i, rc, iBitOff, dx, dy, cx, cy, tx, ty;
char c;
BB_GLYPH *pGlyph;
uint8_t *s, u8, u8Mask, *pG5, u8Temp[32]; // 32 bytes can hold 256 1-bit pixels (of each character)
uint16_t *d;

   u16FG = __builtin_bswap16(u16FG); // swap for big endian (the SPI LCD data format)
   u16BG = __builtin_bswap16(u16BG);
   i = 0;
   cx = (pFont->last - pFont->first + 1); // Glyph count
   pG5 = (uint8_t *)&pFont->glyphs[cx]; // compressed data starts after the last glyph
   while (szMsg[i] != 0 && x < lcd.width()) {
      c = szMsg[i++];
      if (c < pFont->first || c > pFont->last) // undefined character
         continue; // skip it
      c -= pFont->first; // subtract first char of font defined to start from index 0
      pGlyph = (BB_GLYPH *)&pFont->glyphs[c];
      // set up the destination window (rectangle) on the display
      dx = x + pGlyph->xOffset; // offset from character UL to start drawing
      dy = y + pGlyph->yOffset;
      cx = pGlyph->width;
      cy = pGlyph->height;
      if (dy + cy > lcd.height())
         cy = lcd.height() - dy; // clip bottom edge
      if (dx + cx > lcd.width())
         cx = lcd.width() - dx; // clip right edge
      lcd.setAddrWindow(dx, dy, cx, cy); // tell the LCD the memory window in advance of writing the pixels
      s = pG5 + pGlyph->bitmapOffset; // start of bitmap data
      ty = (pGlyph[1].bitmapOffset - pGlyph[0].bitmapOffset); // compressed size
      if (ty < 0 || ty > 4096) ty = 4096; // DEBUG
      rc = g5dec.init(pGlyph->width, cy, s, ty);
      if (rc != G5_SUCCESS) return;
      for (ty=0; ty<cy && rc == G5_SUCCESS; ty++) { // loop over the lines (all equal width)
         rc = g5dec.decodeLine(u8Temp); // decode the 1-bit image one line at a time
         s = u8Temp;
         d = u16Temp; // point to start of output buffer
         u8Mask = 0x80; // MSB first
         u8 = *s++; // preload first byte
         for (tx=0; tx<cx; tx++) { // loop over the pixels on this line and convert from 1 to 16-bits
            *d++ = (u8 & u8Mask) ? u16FG : u16BG; // 1/0 foreground or background color is written
            u8Mask >>= 1;
            if (u8Mask == 0) { // next source byte
               u8 = *s++;
               u8Mask = 0x80;
            }
         } // for x
         lcd.pushPixels(u16Temp, cx); // write to the memory window one line at a time
      } // for ty
      x += pGlyph->xAdvance;
   } // while characters
} /* DrawString() */

void setup()
{
  lcd.begin(DISPLAY_CYD_2USB); // easy initialization of a named display type (ESP32 w/ 2.8" 240x320 ST7789)
  lcd.fillScreen(TFT_BLACK);
  DrawString("Hello", (BB_FONT *)Roboto_Black_40, 0, 70, TFT_GREEN, TFT_BLACK);
  DrawString("World!", (BB_FONT *)Roboto_Black_40, 0, 140, TFT_BLUE, TFT_BLACK);
} /* setup() */

void loop()
{
} /* loop() */

