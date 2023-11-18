#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "pico/multicore.h"

#include "picowbell_pcf8520.h"
#include "picowbell_sd_card.h"
#include "writebuffer.h"

const uint HZ = 10;
const uint DELTA_T = 1000000 / HZ;

void core1_entry() {
    // init sd card
    FATFS fs;
    picowbell_sd_card_init(&fs);

    // open new log file
    FIL f;
    picowbell_sd_card_new_log(&f, 0);
}

int main() {
    // init stdio
    stdio_init_all();

    // launch core 1
    multicore_launch_core1(core1_entry);

    // init cyw43 
    cyw43_arch_init();
    
    // init pcf8520 clock
    picowbell_pcf8520_init();

    // wait for button press on pin 0
    gpio_init(0);
    gpio_set_dir(0, GPIO_IN);
    while (gpio_get(0) != 1) {
        continue;
    }
    printf("starting logs...\n");
    
    datetime_t t = picowbell_pcf8520_get_time();

    struct writebuffer wb;

    // test
    printf("wb in: %u, wb out: %u", wb.in, wb.out);
    for (int i = 0; i < 50; i++) {
        sprintf(writebuffer_in(&wb), "hello there%d", i);
    }
    for (int i = 0; i < 50; i++) {
        printf("%s\n", writebuffer_out(&wb));
    }
    for (int i = 0; i < 51; i++) {
        sprintf(writebuffer_in(&wb), "bad%d", i);
    }
    for (int i = 0; i < 51; i++) {
        printf("%s\n", writebuffer_out(&wb));
    }

    return 1;
    // main logging loop
    while (true) {
        picowbell_pcf8520_wait_next_second(&t);
        absolute_time_t log_time = get_absolute_time();
        for (uint i = 0; i < HZ; i++) {
            // log data
            sprintf(writebuffer_in(&wb), "%02d/%02d/%02d-%02d:%02d:%02d\n", t.month, t.day, t.year, t.hour, t.min, t.sec);

            log_time += DELTA_T;
            sleep_until(log_time);
        }
        
    }

    return 0;
}


