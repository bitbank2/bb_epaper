// Inkplate 6COLOR (EP585C_600x448) hardware test for bb_epaper.
//
// Draws one horizontal bar per ACeP color, each labeled, to verify the
// EP585C_600x448 panel entry and the u8Colors_6color remap on real hardware.
// If every bar's fill matches its label, the panel + color mapping are correct.
//
// Wiring = Inkplate 6COLOR on-board ESP32 (from Inkplate6COLOR/pins.h):
//   DC=33  RESET=19  BUSY=32  CS=27  MOSI/DIN=23  SCK/CLK=18
//
// The 7-color framebuffer is 600*448/2 = 134400 bytes and is allocated in
// PSRAM, so build with PSRAM enabled, e.g.:
//   arduino-cli compile --fqbn esp32:esp32:esp32:PSRAM=enabled \
//       --library <path-to-bb_epaper> examples/inkplate_6color_test
//   arduino-cli upload  --fqbn esp32:esp32:esp32:PSRAM=enabled \
//       -p /dev/cu.usbserial-110 examples/inkplate_6color_test

#include <bb_epaper.h>

BBEPAPER bbep(EP585C_600x448);

#define DC_PIN    33
#define RESET_PIN 19
#define BUSY_PIN  32
#define CS_PIN    27
#define MOSI_PIN  23
#define SCK_PIN   18

struct ColorBar { int fill; int text; const char *name; };

static const ColorBar kBars[7] = {
    { BBEP_BLACK,  BBEP_WHITE, "BLACK"  },
    { BBEP_WHITE,  BBEP_BLACK, "WHITE"  },
    { BBEP_RED,    BBEP_WHITE, "RED"    },
    { BBEP_GREEN,  BBEP_BLACK, "GREEN"  },
    { BBEP_BLUE,   BBEP_WHITE, "BLUE"   },
    { BBEP_YELLOW, BBEP_BLACK, "YELLOW" },
    { BBEP_ORANGE, BBEP_BLACK, "ORANGE" },
};

void setup()
{
    Serial.begin(115200);
    delay(200);
    Serial.println("Inkplate 6COLOR / EP585C_600x448 bb_epaper color test");

    bbep.initIO(DC_PIN, RESET_PIN, BUSY_PIN, CS_PIN, MOSI_PIN, SCK_PIN);
    if (bbep.allocBuffer() != BBEP_SUCCESS) {
        Serial.println("ERROR: allocBuffer failed (PSRAM not enabled?)");
        return;
    }

    bbep.fillScreen(BBEP_WHITE);
    bbep.setFont(FONT_12x16);

    const int W = 600;
    const int barH = 64; // 7 * 64 = 448

    for (int i = 0; i < 7; i++) {
        int y = i * barH;
        bbep.fillRect(0, y, W, barH, kBars[i].fill);
        bbep.setTextColor(kBars[i].text, BBEP_TRANSPARENT);
        bbep.setCursor(16, y + (barH - 16) / 2);
        bbep.print(kBars[i].name);
    }

    Serial.println("Refreshing (full ACeP update, ~12-15 s)...");
    bbep.writePlane();        // transmit the framebuffer to the panel (DTM 0x10 + data)
    bbep.refresh(REFRESH_FULL);
    bbep.sleep(DEEP_SLEEP);
    Serial.println("Done. Check the panel: 7 labeled color bars.");
}

void loop() {}
