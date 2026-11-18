//
// Seeed Studio reTerminal 'Sticky' panel test
//
// The Sticky comes with one of two controllers for its 3.97" 800x480 panel:
// SSD1677 (original boards) or SSD2677 (newer boards). begin(EPD_SEEED_STICKY)
// reads the controller ID and picks the right panel definition.
// This sketch does a full refresh, 10 partial updates of a counter and a
// 4-gray screen, and prints the time of each on the serial port (115200).
//
// Select the target device as "ESP32S3 Dev Module" with OPI PSRAM
//
#include <bb_epaper.h>

BBEPAPER bbep;

const char *ControllerName(void)
{
  int iPanel = bbep.getPanelType();
  if (iPanel == EP397C_800x480 || iPanel == EP397C_800x480_4GRAY) return "SSD2677";
  return "SSD1677";
} /* ControllerName() */
//
// Write the memory plane(s) and refresh the panel, return the total time in ms
//
int Update(int iMode, int iPlane)
{
  long l = millis();
  // Panels which aren't sent an init sequence on each refresh (SSD2677)
  // need to be told which waveform to use before the data is written
  if (bbep.capabilities() & BBEP_DONT_INIT) {
    bbep.sendInit(iMode);
  }
  bbep.writePlane(iPlane);
  bbep.refresh(iMode);
  return (int)(millis() - l);
} /* Update() */

void DrawCounter(int i)
{
  bbep.fillRect(40, 200, 400, 80, BBEP_WHITE);
  bbep.setFont(FONT_16x16);
  bbep.setTextColor(BBEP_BLACK, BBEP_WHITE);
  bbep.setCursor(60, 230);
  bbep.printf("Partial update %d", i);
} /* DrawCounter() */

void setup()
{
  int i, iTime;

  // Keep the board powered when running from the battery
  pinMode(45, OUTPUT); // power hold (DATA)
  pinMode(46, OUTPUT); // power lock (CLK)
  digitalWrite(45, 1);
  digitalWrite(46, 0);
  digitalWrite(46, 1);

  Serial.begin(115200);
  delay(1000);
  Serial.println("bbep: Seeed Sticky test");

  if (bbep.begin(EPD_SEEED_STICKY) != BBEP_SUCCESS) {
    Serial.println("bbep: begin failed");
    return;
  }
  Serial.printf("bbep: controller %s (panel type %d)\n", ControllerName(), bbep.getPanelType());
  // 2 planes: plane 0 = new image, plane 1 = old image (for partial updates)
  if (bbep.allocBuffer(true) != BBEP_SUCCESS) {
    Serial.println("bbep: no memory for the buffer");
    return;
  }
  // Full refresh
  bbep.fillScreen(BBEP_WHITE);
  bbep.drawRect(20, 20, bbep.width() - 40, bbep.height() - 40, BBEP_BLACK);
  bbep.drawRect(24, 24, bbep.width() - 48, bbep.height() - 48, BBEP_BLACK);
  bbep.setFont(FONT_16x16);
  bbep.setTextColor(BBEP_BLACK, BBEP_WHITE);
  bbep.setCursor(60, 60);
  bbep.print("bb_epaper on the Seeed Sticky");
  bbep.setFont(FONT_12x16);
  bbep.setCursor(60, 100);
  bbep.printf("Controller: %s", ControllerName());
  DrawCounter(0);
  iTime = Update(REFRESH_FULL, PLANE_DUPLICATE);
  Serial.printf("bbep: full %d ms\n", iTime);

  // 10 partial updates
  for (i=1; i<=10; i++) {
    bbep.backupPlane(); // current image becomes the old image
    DrawCounter(i);
    iTime = Update(REFRESH_PARTIAL, PLANE_BOTH);
    Serial.printf("bbep: partial %d ms\n", iTime);
  }
  delay(2000);

  // 4-gray screen
  bbep.setPanelType((bbep.getPanelType() == EP397C_800x480) ? EP397C_800x480_4GRAY : EP397_800x480_4GRAY);
  bbep.fillScreen(BBEP_WHITE);
  for (i=0; i<4; i++) {
    bbep.fillRect(40 + i*180, 120, 160, 240, BBEP_GRAY0 + i);
  }
  bbep.setFont(FONT_12x16);
  bbep.setTextColor(BBEP_GRAY0, BBEP_GRAY3);
  bbep.setCursor(40, 400);
  bbep.print("4-gray: GRAY0 GRAY1 GRAY2 GRAY3");
  iTime = Update(REFRESH_FULL, PLANE_BOTH);
  Serial.printf("bbep: 4gray %d ms\n", iTime);

  bbep.sleep(DEEP_SLEEP);
  Serial.println("bbep: done");
} /* setup() */

void loop()
{
} /* loop() */
