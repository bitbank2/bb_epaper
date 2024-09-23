#include <stdio.h>
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "ulp_riscv.h"
#include "ulp_main.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_main_bin_start");
extern const uint8_t ulp_main_bin_end[]   asm("_binary_ulp_main_bin_end");

static void init_ulp_program(void);


void app_main(void)
{

    init_ulp_program();
    /* Go to sleep, only the ULP Risc-V will run */
    printf("Entering in deep sleep\n\n");

    /* 5 second delay to ensure we can reflash from USB */
//    vTaskDelay(500);

    ESP_ERROR_CHECK( esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON)); // RTC domain must be on or ULP won't work
    ESP_ERROR_CHECK( esp_sleep_enable_ulp_wakeup());
    esp_deep_sleep_start(); // only wake up if the ULP allows!

}

static void init_ulp_program(void)
{
    esp_err_t err = ulp_riscv_load_binary(ulp_main_bin_start, (ulp_main_bin_end - ulp_main_bin_start));
    ESP_ERROR_CHECK(err);

    /* The first argument is the period index, which is not used by the ULP-RISC-V timer
     * The second argument is the period in microseconds, which gives a wakeup time period of: 20ms
     */
    ulp_set_wakeup_period(0, 5000000); // 5 seconds

    /* Start the program */
    err = ulp_riscv_run();
    ESP_ERROR_CHECK(err);
}

