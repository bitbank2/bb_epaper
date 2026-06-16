//
// bb_epaper test for the Seeed reTerminal E1004
// 13.3" 1200x1600 dual-chip Spectra6 panel (T133A01).
//
// This standalone sketch exercises the new EP133A_SPECTRA_1200x1600 panel:
//   - manual init (panel power enable + second chip-select + initIO)
//   - full-width colour bands that cross the dual-chip seam at x=600
//   - a box and text drawn over the seam
//   - one full refresh (PON/DRF/POF) then light sleep
//
// Serial log goes to Serial1 on the carrier USB-UART bridge (GPIO44 RX /
// GPIO43 TX) at 115200, the same path the reTerminal firmware uses.
//
#include <bb_epaper.h>
#include "Fonts/Roboto_Black_24.h"

// ---- E1004 display pins (from Seeed Setup523) ----
#define EPD_SCK   7
#define EPD_MOSI  9
#define EPD_CS    10  // controller #1 (left 600 columns)
#define EPD_CS2   2   // controller #2 (right 600 columns)
#define EPD_DC    11
#define EPD_BUSY  13
#define EPD_RST   38
#define EPD_EN    12  // panel power enable (active HIGH)

// ---- Serial log over the carrier bridge ----
#define LOG Serial1
#define PIN_LOG_RX 44
#define PIN_LOG_TX 43

BBEPAPER bbep;

void setup(void)
{
    LOG.begin(115200, SERIAL_8N1, PIN_LOG_RX, PIN_LOG_TX);
    delay(500);
    LOG.println();
    LOG.println("=== E1004 T133A01 bb_epaper test ===");
    LOG.printf("PSRAM found: %s\n", psramFound() ? "yes" : "NO");

    // 1) enable the panel power rail
    pinMode(EPD_EN, OUTPUT);
    digitalWrite(EPD_EN, HIGH);
    delay(10);

    // 2) select the panel and register the second controller CS BEFORE initIO
    bbep.setPanelType(EP133A_SPECTRA_1200x1600);
    bbep.setCS2(EPD_CS2);

    // 3) bring up the bus and initialise both controllers
    bbep.initIO(EPD_DC, EPD_RST, EPD_BUSY, EPD_CS, EPD_MOSI, EPD_SCK, 8000000);
    LOG.printf("Panel: %u x %u\n", bbep.width(), bbep.height());

    // 4) allocate the 4bpp framebuffer (1200*1600/2 = 960000 bytes, needs PSRAM)
    if (bbep.allocBuffer() != BBEP_SUCCESS) {
        LOG.println("allocBuffer FAILED (need OPI PSRAM). Halting.");
        while (true) delay(1000);
    }

    // 5) draw a test pattern
    bbep.fillScreen(BBEP_WHITE);

    // Full-width colour bands: each row is split across the two chips at x=600,
    // so a clean horizontal band proves the dual-chip stitching works.
    const uint8_t bands[6] = {
        BBEP_BLACK, BBEP_RED, BBEP_GREEN, BBEP_BLUE, BBEP_YELLOW, BBEP_WHITE
    };
    const int bandH = bbep.height() / 6; // ~266 px
    for (int i = 0; i < 6; i++) {
        bbep.fillRect(0, i * bandH, bbep.width(), bandH - 2, bands[i]);
    }

    // Box centred over the seam (x=600) to check left/right alignment.
    bbep.fillRect(bbep.width()/2 - 150, 700, 300, 220, BBEP_BLACK);
    bbep.fillRect(bbep.width()/2 - 140, 710, 280, 200, BBEP_WHITE);

    // Text near the top.
    bbep.setFont(Roboto_Black_24);
    bbep.setTextColor(BBEP_WHITE, BBEP_BLACK);
    bbep.setCursor(40, 70);
    bbep.println("E1004 T133A01 dual-chip test");

    LOG.println("writePlane ...");
    bbep.writePlane();
    LOG.println("refresh (full, ~25-40s) ...");
    bbep.refresh(REFRESH_FULL);
    LOG.println("done; sleeping.");
    bbep.sleep(DEEP_SLEEP);
}

void loop(void)
{
}
