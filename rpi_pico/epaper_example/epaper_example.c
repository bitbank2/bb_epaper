#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19
#define PIN_BUSY 22
#define PIN_DC 21
#define PIN_RST 20

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

#include "../../src/bb_epaper.h"
#include "../pico_io.inl"
#include "../../src/bb_ep.inl"
#include "../../src/bb_ep_gfx.inl"
//#include "Roboto_Black_20r.h"
BBEPDISP bbep; // the main display structure


int main()
{
    bbepSetPanelType(&bbep, EP295_128x296); // must set this first
    bbepInitIO(&bbep, PIN_DC, PIN_RST, PIN_BUSY, PIN_CS, PIN_MOSI, PIN_SCK, 8000000);
    bbepFill(&bbep, BBEP_WHITE, 0);
    bbepWriteString(&bbep, 0, 0, "RPI Pico2!", FONT_12x16, BBEP_BLACK);
    bbepRefresh(&bbep, REFRESH_FULL);
    bbepWaitBusy(&bbep);
    bbepSleep(&bbep, DEEP_SLEEP);
} /* main() */
