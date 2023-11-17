#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#include "picowbell_pcf8520.h"
#include "picowbell_sd_card.h"

int main() {
    // init stdio
    stdio_init_all();

    // init cyw43 
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    // init sd card
    FATFS fs;
    picowbell_sd_card_init(&fs);

    // init pcf8520 clock
    picowbell_pcf8520_init();

    // wait for button press on pin 0
    gpio_init(0);
    gpio_set_dir(0, GPIO_IN);
    while (gpio_get(0) != 1) {
        continue;
    }
    printf("logging to file...\n");

    // open new log file
    FIL f;
    picowbell_sd_card_new_log(&f);

    int i = 0;
    // main logging loop
    while (true) {
        // get timestamp from clock
        char timeBuf[20];
        picowbell_pcf8520_get_time_string(timeBuf);

        // log data
        printf("logging data: %s\n", timeBuf);
        f_printf(&f, "%s\n", timeBuf);
        f_sync(&f);
        sleep_ms(1000);
    }

    return 0;
}