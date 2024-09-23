/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* ULP-RISC-V example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

   This code runs on ULP-RISC-V  coprocessor
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "ulp_riscv.h"
#include "ulp_riscv_utils.h"
#include "ulp_riscv_gpio.h"

// Tell the bb_epaper library not to include code related to
// drawing with a back buffer for the epaper panel
#define NO_RAM
#include "../../../../src/bb_epaper.h"
#include "../../../s3_ulp_io.inl"
#include "../../../../src/bb_ep.inl"
#include "../../../../src/bb_ep_gfx.inl"
#include "Roboto_Black_20r.h"
BBEPDISP bbep; // the main display structure
static bool gpio_level = false;
// Pinout for the LilyGo mini-epaper S3
#define PIN_BUSY 10
#define PIN_RST 11
#define PIN_DC 12
#define PIN_CS 13
#define PIN_SCK 14
#define PIN_MOSI 15

/* this variable will be exported as a public symbol, visible from main CPU: */
bool gpio_level_previous = false;

int main (void)
{
    bbepSetPanelType(&bbep, EPD102_80x128);
    bbepInitIO(&bbep, PIN_DC, PIN_RST, PIN_BUSY, PIN_CS, PIN_MOSI, PIN_SCK, 0);
    bbepWakeUp(&bbep);
    bbepSendCMDSequence(&bbep, bbep.pInitFull);
    bbep.iPlane = 1; // clear both buffers to white
    bbepRectangle(&bbep, 0, 0, 79, 127, BBEP_WHITE, 1);
    bbep.iPlane = 0;
    bbepRectangle(&bbep, 0, 0, 79, 127, BBEP_WHITE, 1);
    bbepWriteStringCustom(&bbep, (BB_FONT *)Roboto_Black_20r, 48, 4, "12:34", BBEP_BLACK, 0);
    bbepRefresh(&bbep, REFRESH_FULL);
    bbepWaitBusy(&bbep);
    bbepSleep(&bbep, 1);
    bbepDeInitIO(&bbep); // release GPIO before going back to sleep
        /* Wakes up the main CPU if pin changed its state */
//        if(gpio_level != gpio_level_previous) {
//            gpio_level_previous = gpio_level;
//            ulp_riscv_wakeup_main_processor();
//            break;
//        }
//    }
    /* ulp_riscv_halt() is called automatically when main exits */
//    ulp_set_wakeup_period(0, 5000000); // period in usec
    return 0;
}
