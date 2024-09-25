#include "debug.h"
#include "debug.h"
#include "../../../src/bb_epaper.h"
#include "../../ch32v_io.inl"
#include "../../../src/bb_ep.inl"
#include "../../../src/bb_ep_gfx.inl"
#include "Roboto_Black_40r.h"

BBEPDISP bbep;
#define PIN_DC 0xd4
#define PIN_CS 0xd2
#define PIN_BUSY 0xd3
#define PIN_RST 0xd5

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();

    bbepSetPanelType(&bbep, EPD295_128x296);
    bbepInitIO(&bbep, PIN_DC, PIN_RST, PIN_BUSY, PIN_CS, -1, -1, 8000000);
    bbepWakeUp(&bbep);
    bbepSendCMDSequence(&bbep, bbep.pInitFull);
    bbep.iPlane = 1; // clear both buffers to white
    bbepRectangle(&bbep, 0, 0, 127, 295, BBEP_WHITE, 1);
    bbep.iPlane = 0;
    bbepRectangle(&bbep, 0, 0, 127, 295, BBEP_WHITE, 1);
    bbepWriteStringCustom(&bbep, (BB_FONT *)Roboto_Black_40r, 120, 24, "CH32V", BBEP_BLACK, 0);
    bbepWriteStringCustom(&bbep, (BB_FONT *)Roboto_Black_40r, 60, 60, "FTW!", BBEP_BLACK, 0);
    bbepRefresh(&bbep, REFRESH_FULL);
    bbepWaitBusy(&bbep);
    bbepSleep(&bbep, 1);
    while (1) {};
} /* main() */
