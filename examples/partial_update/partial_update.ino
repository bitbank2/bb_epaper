//
// Partial Update example
//
// Written by Larry Bank
// Oct 2, 2024
// Copyright (c) BitBank Software, Inc.
//
// What does 'partial update' mean???
// Before answering that question, it's best to define what a 'full update' means:
// In the world of e-paper, a full update means that the pigment granules are moved from their
// current state/color to a new state/color in a multi-pass sequence that causes them to all go from
// a white state to a black state and back again, with a very visible 'flicker' of the entire display.
// This full update can require multiple passes from black to white to ensure that all granules are
// fully in the desired state. The granules are moved with a strong electric field between the front
// and back of the display. Now that you know what a full update is, a partial update is where the
// pixels are changed from their old state to a new state in a single pass without flapping back and
// forth. This is much more pleasing to the eye and uses less power. The down side to this form of
// update is that the granules don't all move fully to their desired positions and, over time, will
// leave areas with 'ghosts' of the old image. From what I've obvserved, it is usually best to do
// at most 10 partial updates between full updates.
//
// The 24-pin SPI epaper controllers generally come in two types: SSD16xx or UC81xx. They both work
// nearly identically. They each contain 2 1-bit image planes (old image data, new image data). To
// perform a partial update, you need to tell the controller to use partial mode and treat plane 1
// as the new pixels and plane 0 as the old pixels. It will then see which pixels are different and
// update only those pixels. The SSD16xx controllers can be told just the new pixels for each update
// and it copies that data to the "old" plane for you. On the UC81xx controllers, I've had to write
// both old and new data each update.
//
#include <bb_epaper.h>
BBEPAPER bbep(EPD295_128x296);
//BBEPAPER bbep(EPD122_192x176);
// My Arduino Nano 33 BLE e-paper adapter
#define DC_PIN 16
#define BUSY_PIN 15
#define RESET_PIN 14
#define CS_PIN 10

void DrawScene(int i, int iPlane)
{
  char szTemp[16];
  bbep.setPlane(iPlane);
  bbep.fillScreen(BBEP_WHITE, iPlane);
  bbep.setFont(FONT_16x16);
  bbep.setTextColor(BBEP_BLACK);
  bbep.setCursor(0,0);
  bbep.println("bb_epaper");
  bbep.println("partial");
  bbep.println("Update demo");
  sprintf(szTemp, "\n%04d", i);
  bbep.println(szTemp);
}  /* DrawScene() */

void setup()
{
  int i = 0;
  bbep.initIO(DC_PIN, RESET_PIN, BUSY_PIN, CS_PIN);
  bbep.setRotation(90); // use panel in landscape orientation
  // work in bufferless mode (no memory on MCU needed)
  DrawScene(i, PLANE_0); // draw "old" data (full refresh only needs "old" data plane)
  bbep.refresh(REFRESH_FULL); // do a full refresh to start
  bbep.wait();
  bbep.sleep(LIGHT_SLEEP);
  // count from 0 to 9 using partial updates
  while (i<10) {
    DrawScene(i, 0); // draw scene to old plane 0 (not always needed, but doesn't hurt to do)
    i++;
    DrawScene(i, 1); // new scene drawn to plane 1
    bbep.refresh(REFRESH_PARTIAL);
    bbep.wait();
//    bbep.sleep(LIGHT_SLEEP);
  } // for i
  delay(5000); // wait 5 seconds, then erase to white
  bbep.setPlane(0);
  bbep.fillScreen(BBEP_WHITE);
  bbep.refresh(REFRESH_FULL);
  bbep.wait();
  bbep.sleep(DEEP_SLEEP);
} /* setup() */

void loop()
{

} /* loop() */