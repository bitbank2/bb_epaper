//
// bb_epaper diagnostic test for the Seeed reTerminal E1004 (T133A01).
// Prints timing + BUSY-line state so a blank-screen failure can be diagnosed
// without seeing the panel. Loops slowly so repeated testing is gentler.
//
#include <Wire.h>
#include <bb_epaper.h>
#include "Fonts/Roboto_Black_24.h"

#define EPD_SCK   7
#define EPD_MOSI  9
#define EPD_CS    10
#define EPD_CS2   2
#define EPD_DC    11
#define EPD_BUSY  13
#define EPD_RST   38
#define EPD_EN    12

#define LOG Serial1
#define PIN_LOG_RX 44
#define PIN_LOG_TX 43
#define REFRESH_INTERVAL_MS 300000UL

BBEPAPER bbep;

static void drawColorTile(int x, int y, int w, int h, uint16_t color, const char *label)
{
    bbep.fillRect(x, y, w, h, color);
    bbep.drawRect(x, y, w, h, BBEP_BLACK);

    const int labelW = 180;
    const int labelH = 44;
    const int labelX = x + 24;
    const int labelY = y + 24;
    bbep.fillRect(labelX, labelY, labelW, labelH, BBEP_WHITE);
    bbep.drawRect(labelX, labelY, labelW, labelH, BBEP_BLACK);
    bbep.setTextColor(BBEP_BLACK, BBEP_WHITE);
    bbep.setFont(FONT_16x16);
    bbep.setCursor(labelX + 14, labelY + 14);
    bbep.println(label);
}

static void drawPattern()
{
    bbep.fillScreen(BBEP_WHITE);
    bbep.setFont(Roboto_Black_24);
    bbep.setTextColor(BBEP_BLACK, BBEP_WHITE);
    bbep.setCursor(40, 70);
    bbep.println("E1004 T133A01 SPECTRA6 COLOR");
    bbep.setFont(FONT_16x16);
    bbep.setCursor(40, 125);
    bbep.println("BLACK WHITE RED YELLOW BLUE GREEN");

    const int marginX = 40;
    const int gap = 24;
    const int top = 210;
    const int tileW = (bbep.width() - marginX * 2 - gap) / 2;
    const int tileH = (bbep.height() - top - marginX - gap * 2) / 3;

    drawColorTile(marginX, top, tileW, tileH, BBEP_BLACK, "BLACK");
    drawColorTile(marginX + tileW + gap, top, tileW, tileH, BBEP_WHITE, "WHITE");
    drawColorTile(marginX, top + tileH + gap, tileW, tileH, BBEP_RED, "RED");
    drawColorTile(marginX + tileW + gap, top + tileH + gap, tileW, tileH, BBEP_YELLOW, "YELLOW");
    drawColorTile(marginX, top + (tileH + gap) * 2, tileW, tileH, BBEP_BLUE, "BLUE");
    drawColorTile(marginX + tileW + gap, top + (tileH + gap) * 2, tileW, tileH, BBEP_GREEN, "GREEN");
}

void setup(void)
{
    LOG.begin(115200, SERIAL_8N1, PIN_LOG_RX, PIN_LOG_TX);
    delay(500);
    LOG.println();
    LOG.println("=== E1004 diag (loops every 5 minutes) ===");
    LOG.printf("PSRAM: %s\n", psramFound() ? "yes" : "NO");

    pinMode(EPD_EN, OUTPUT);
    digitalWrite(EPD_EN, HIGH);
    pinMode(EPD_BUSY, INPUT);
    delay(20);
    LOG.printf("BUSY at rest (pre-init) = %d  (UC81xx idle should be HIGH=1)\n",
               digitalRead(EPD_BUSY));

    bbep.setPanelType(EP133A_SPECTRA_1200x1600);
    bbep.setCS2(EPD_CS2);

    unsigned long t0 = millis();
    bbep.initIO(EPD_DC, EPD_RST, EPD_BUSY, EPD_CS, EPD_MOSI, EPD_SCK, 8000000);
    LOG.printf("initIO took %lu ms; BUSY now = %d; size = %u x %u\n",
               millis() - t0, digitalRead(EPD_BUSY), bbep.width(), bbep.height());

    if (bbep.allocBuffer() != BBEP_SUCCESS) {
        LOG.println("allocBuffer FAILED. Halting.");
        while (true) delay(1000);
    }
    LOG.println("allocBuffer OK");
}

void loop(void)
{
    LOG.println("--- cycle start ---");
    unsigned long t0 = millis();
    drawPattern();
    LOG.printf("draw %lu ms\n", millis() - t0);

    t0 = millis();
    bbep.writePlane();
    LOG.printf("writePlane %lu ms; BUSY = %d\n", millis() - t0, digitalRead(EPD_BUSY));

    t0 = millis();
    bbep.refresh(REFRESH_FULL);
    unsigned long dt = millis() - t0;
    LOG.printf("refresh %lu ms; BUSY = %d\n", dt, digitalRead(EPD_BUSY));
    LOG.printf(">>> refresh %s (expected 20000-40000 ms for a real refresh)\n",
               dt > 5000 ? "looks REAL" : "TOO FAST - panel not refreshing");

    LOG.println("--- cycle end; wait 5 minutes ---");
    delay(REFRESH_INTERVAL_MS);
}
