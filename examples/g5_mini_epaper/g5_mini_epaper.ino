//
// Getting_started with the Group5 image compression library
// Written by Larry Bank, September 20, 2024
//
// This sketch will demonstrate how to decode a 128x64 1-bit image
// and display it on an OLED display
//
#include <Group5.h>
#include <bb_epaper.h>
#include "bart_80x128.h" // a pre-compressed image made with the Linux CLI tool in this repo

BBEPAPER bbep;
G5DECODER g5dec;
#define IMAGE_WIDTH 80
#define IMAGE_HEIGHT 128
// Pinout for the LilyGo mini-epaper S3
#define PIN_BUSY 10
#define PIN_RST 11
#define PIN_DC 12
#define PIN_CS 13
#define PIN_SCK 14
#define PIN_MOSI 15

void setup()
{
uint8_t u8Line[16]; // current decoded line
int rc, y;

  bbep.setPanelType(EPD102_80x128);
  bbep.initIO(PIN_DC, PIN_RST, PIN_BUSY, PIN_CS, PIN_MOSI, PIN_SCK, 4000000);
  bbep.setAddrWindow(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT); // set memory window to whole display
  bbep.startWrite(PLANE_0); // write the data to plane 0
  rc = g5dec.init(IMAGE_WIDTH, IMAGE_HEIGHT, (uint8_t *)bart_80x128, sizeof(bart_80x128));
  if (rc == G5_SUCCESS) {
    for (y=0; y<IMAGE_HEIGHT && rc == G5_SUCCESS; y++) { // last line returns G5_DECODE_COMPLETE
        rc = g5dec.decodeLine(u8Line);
        bbep.writeData(u8Line, (IMAGE_WIDTH+7)/8); // write this line of data
    } // for y
    bbep.refresh(REFRESH_FULL);
  } // if decode init succeeded
  bbep.sleep(1);
} /* setup() */

void loop()
{
} /* loop() */
