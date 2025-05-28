#include <stdio.h>
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../../../src/bb_epaper.h"
#include "../../esp_generic.inl"
#include "../../../src/bb_ep.inl"
#include "../../../src/bb_ep_gfx.inl"

// Pinout for the LilyGo mini-epaper S3
#define PIN_BUSY 10
#define PIN_RST 11
#define PIN_DC 12
#define PIN_CS 13
#define PIN_SCK 14
#define PIN_MOSI 15

// static instance of bb_epaper state
static BBEPDISP disp;

void app_main(void)
{
    printf("Starting...\n");
    bbepSetPanelType(&disp, EP102_80x128); // This must be called first
    bbepInitIO(&disp, PIN_DC, PIN_RST, PIN_BUSY, PIN_CS, PIN_MOSI, PIN_SCK, 4000000);
    bbepAllocBuffer(&disp);
    bbepFill(&disp, BBEP_WHITE, PLANE_BOTH);
    bbepWriteString(&disp, 0, 4, "Hello", FONT_12x16, BBEP_BLACK, BBEP_WHITE);
    bbepWriteString(&disp, 0, 20, "World!", FONT_12x16, BBEP_BLACK, BBEP_WHITE);
    bbepWritePlane(&disp, PLANE_BOTH);
    bbepRefresh(&disp, REFRESH_FULL);
    bbepWaitBusy(&disp);
    bbepSleep(&disp, 1); // deep sleep
    printf("Finished\n");
    while (1) {
        vTaskDelay(1);
    }
}
