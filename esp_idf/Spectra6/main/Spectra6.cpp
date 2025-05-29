#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <bb_epaper.h>
#include "Roboto_Black_40.h"
BBEPAPER bbep(EP81_SPECTRA_1024x576);

#define DC_PIN 14
#define BUSY_PIN 13
#define RESET_PIN 9
#define CS_PIN 10
#define CS_PIN2 8
#define SCK_PIN 12
#define MOSI_PIN 11

extern "C" void app_main(void)
{
    bbep.initIO(DC_PIN, RESET_PIN, BUSY_PIN, CS_PIN, MOSI_PIN, SCK_PIN, 8000000);
    bbep.setCS2(CS_PIN2);
    bbep.setRotation(0);
    bbep.allocBuffer();
    bbep.fillScreen(BBEP_WHITE);
    bbep.setFont(&Roboto_Black_40);
    bbep.setTextColor(BBEP_BLACK);
    bbep.setCursor(0, 75);
    bbep.print("Mañana");
    bbep.setTextColor(BBEP_RED);
    bbep.setCursor(100, 150);
    bbep.print("Ótimo!");
    bbep.setTextColor(BBEP_BLUE);
    bbep.setCursor(200, 225);
    bbep.print("Non è vero?");
    bbep.setTextColor(BBEP_YELLOW);
    bbep.setCursor(300, 300);
    bbep.print("Não foi eu!");
    bbep.writePlane();
    bbep.refresh(REFRESH_FULL);
    bbep.sleep(1); // deep sleep
    vTaskDelay(200); // wait 2 seconds
// try different content
    bbep.fillScreen(BBEP_YELLOW);
    bbep.setTextColor(BBEP_GREEN);
    bbep.setCursor(200, 300);
    bbep.print("Second time is broken?");
    bbep.writePlane();
    bbep.refresh(REFRESH_FULL);
    bbep.sleep(1);

    while (1) {
        vTaskDelay(1);
    }
}
