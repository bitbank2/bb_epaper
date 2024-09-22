//
// Getting_started with the Group5 image compression library
// Written by Larry Bank, September 20, 2024
//
// This sketch will demonstrate how to decode a 128x64 1-bit image
// and display it on an OLED display
//
#include <Group5.h>
#include <OneBitDisplay.h>
#include "bart.h" // a pre-compressed image made with the Linux CLI tool in this repo

ONE_BIT_DISPLAY oled;
G5DECODER g5dec;
uint8_t u8Buffer[1024]; // big enough to hold the OLED contents
#define IMAGE_WIDTH 128
#define IMAGE_HEIGHT 64
//
// Image files are normally organized as horizontal bytes with the MSB on the left
// The SSD1306 OLED is vertical bytes with the LSB as the top pixel of each set of 8 rows
// So we need to rotate the pixels to the display's memory layout
//
void DrawLine(int y, uint8_t *u8Line)
{
uint8_t u8, u8SrcMask, u8DestMask, *s, *d;
int x;
  u8DestMask = 1 << (y & 7);
  d = &u8Buffer[(y>>3) * IMAGE_WIDTH];
  s = u8Line;
  u8SrcMask = 0x80; // image encoding is MSB first (left pixel of each byte)
  u8 = *s++;
  for (x=0; x<IMAGE_WIDTH; x++) {
    if (u8 & u8SrcMask)
      d[x] |= u8DestMask;
    else
      d[x] &= ~u8DestMask;
    u8SrcMask >>= 1;
    if (u8SrcMask == 0) { // next byte
      u8SrcMask = 0x80;
      u8 = *s++;
    }
  } // for x
} /* DrawLine() */

void setup()
{
  oled.I2Cbegin(OLED_128x64); // use default I2C interface for target board
  oled.fillScreen(OBD_WHITE); // really black on OLEDs
  oled.setFont(FONT_12x16);
  oled.println("Group5 Demo");
  oled.setBuffer(u8Buffer);
  delay(2000);
} /* setup() */

void loop()
{
int rc, y;
long lTime;
uint8_t u8Line[16]; // current decoded line

  rc = g5dec.init(IMAGE_WIDTH, IMAGE_HEIGHT, (uint8_t *)bart, sizeof(bart));
  if (rc == G5_SUCCESS) {
    lTime = millis();
    for (y=0; y<IMAGE_HEIGHT && rc == G5_SUCCESS; y++) {
        rc = g5dec.decodeLine(u8Line);
        DrawLine(y, u8Line); // draw this into the correct bit direction for the OLED
    }
    lTime = millis() - lTime;
    oled.display();
    delay(2000);
    oled.fillScreen(OBD_WHITE);
    oled.println("Decoded in");
    oled.print((int)lTime, DEC);
    oled.println(" msec");
    delay(3000);
  }
} /* loop() */
