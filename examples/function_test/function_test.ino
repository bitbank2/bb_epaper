//
// Library function test
// Test functions and boundary conditions
//
#include <bb_epaper.h>
BBEPAPER epd(EP295_128x296);
#include "Roboto_Thin_48.h"
// Our C6 epaper PCB
#define CS_PIN 2
#define DC_PIN 3
#define RESET_PIN 5
#define BUSY_PIN 4
#define CLK_PIN 1
#define MOSI_PIN 0
#define POWER_PIN -1
#define BUTTON1 18
#define BUTTON2 19
#define SDA_PIN 7
#define SCL_PIN 6

// Custom font clipping test
void test1()
{
  Serial.print("Custom font upper edge clipping test: ");
  epd.fillScreen(BBEP_WHITE);
  epd.setTextColor(BBEP_BLACK, BBEP_WHITE);
  epd.setCursor(0, 10); // text needs to be clipped above the top of the display area
  epd.setFont(Roboto_Thin_48);
  epd.print("Test");
  epd.writePlane();
  epd.refresh(REFRESH_FULL, true);
  Serial.println(" PASS"); // if it didn't crash, it passed
  Serial.print("Custom font right edge clipping test: ");
  epd.setCursor(epd.width()-30, 10); // text needs to be clipped above the top of the display area
  epd.print("Test");
  epd.writePlane();
  epd.refresh(REFRESH_FULL, true);
  Serial.println(" PASS"); // if it didn't crash, it passed
  Serial.print("Custom font lower right corner clipping test: ");
  epd.setCursor(epd.width()-30, epd.height()+10);
  epd.print("Test");
  epd.writePlane();
  epd.refresh(REFRESH_FULL, true);
  Serial.println(" PASS"); // if it didn't crash, it passed
}
// Built-in font clipping test
void test2()
{
  Serial.print("Buit-in font upper edge clipping test: ");
  epd.fillScreen(BBEP_WHITE);
  epd.setTextColor(BBEP_BLACK, BBEP_WHITE);
  epd.setCursor(0, -10); // text needs to be clipped above the top of the display area
  epd.setFont(FONT_12x16);
  epd.print("Test");
  epd.writePlane();
  epd.refresh(REFRESH_FULL, true);
  Serial.println(" PASS"); // if it didn't crash, it passed
  Serial.print("Buit-in font right edge clipping test: ");
  epd.setCursor(epd.width()-30, 0); // text needs to be clipped above the top of the display area
  epd.print("Test");
  epd.writePlane();
  epd.refresh(REFRESH_FULL, true);
  Serial.println(" PASS"); // if it didn't crash, it passed
  Serial.print("Buit-in font lower right corner clipping test: ");
  epd.setCursor(epd.width()-30, epd.height()-8);
  epd.print("Test");
  epd.writePlane();
  epd.refresh(REFRESH_FULL, true);
  Serial.println(" PASS"); // if it didn't crash, it passed
}

void test3()
{

}
void setup()
{
  Serial.begin(115200);
  delay(3000); // wait for CDC-Serial
  Serial.println("bb_epaper function tests");
  epd.initIO(DC_PIN, RESET_PIN, BUSY_PIN, CS_PIN, MOSI_PIN, CLK_PIN);
  epd.allocBuffer();
  test1();
  test2();
  test3();
} /* setup() */

void loop()
{
} /* loop() */

