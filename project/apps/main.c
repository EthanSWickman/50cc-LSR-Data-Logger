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

auto_init_mutex(my_mutex);

const uint HZ = 500;
const uint DELTA_T = 1000000 / HZ;
const uint SAVE_INTERVAL = 512; // 2^13 = 8192

// generally a f_write takes 70 * 10^-6 seconds, and fsync takes 15000 * 10^-6 seconds
// in theory, this means we should be able to write several thousand times per second if we only sync once in a while
// the desmos equation 1 > (100 * 10^-6)x + (20000 * 10^-6)(1/y) where x is HZ and y is the seconds in between saves gives us a SOLID 9000 hz maximum

void core1_entry() {
    // init sd card
    FATFS fs;
    picowbell_sd_card_init(&fs);

    // open new log file
    FIL f;
    picowbell_sd_card_new_log(&f, 0);

    struct writebuffer* wb = (struct writebuffer*) multicore_fifo_pop_blocking();

    uint i = 0;
    while (true) {
        mutex_enter_blocking(&my_mutex);
        char* wb_out = writebuffer_out(wb);
        mutex_exit(&my_mutex);
        if (wb_out == NULL) {
            continue;
        }
        else {
            absolute_time_t t = get_absolute_time();
            f_printf(&f, "%s\n", wb_out);
            if ((i++ & (SAVE_INTERVAL - 1)) == 0) {
                f_sync(&f);
            }
        }
    }
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

    // light up the logging indicator
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    // start logs
    printf("starting logs...\n");
    
    // get current time
    datetime_t t = picowbell_pcf8520_get_time();

    struct writebuffer wb;
    wb.in = wb.out = 0;

    // push writebuffer address to core 1
    multicore_fifo_push_blocking((uintptr_t) &wb);

    // main logging loop
    while (true) {
        picowbell_pcf8520_wait_next_second(&t);
        absolute_time_t log_time = get_absolute_time();
        for (uint i = 0; i < HZ; i++) {
            // flash led
            if (i > (HZ / 2)) cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

            // log data
            mutex_enter_blocking(&my_mutex);
            char* wb_in = writebuffer_in(&wb);
            //printf("size of buffer = %d\n", wb.in - wb.out);
            mutex_exit(&my_mutex);
            if (wb_in == NULL) {
                printf("core 0: full buffer!\n");
            }
            else {
                sprintf(wb_in, "%02d/%02d/%02d-%02d:%02d:%02d", t.month, t.day, t.year, t.hour, t.min, t.sec);
            }

            log_time += DELTA_T;
            sleep_until(log_time);
        }
        // flash led
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    }

    return 0;
}


