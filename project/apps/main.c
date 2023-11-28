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

const uint HZ = 50; // logs per second (keep under 50 until we solve high frequency problems!)
const uint64_t DELTA_T = 1000000 / HZ;
const uint SAVE_INTERVAL = 256; // should be 2^n for some integer n, represents how many logs until we save results to sd card

void core1_entry() {
    // init sd card
    FATFS fs;
    picowbell_sd_card_init(&fs);

    // open new log file
    FIL f;
    picowbell_sd_card_new_log(&f, 0);

    // receive writebuffer address for string communication between cores
    struct writebuffer* wb = (struct writebuffer*) multicore_fifo_pop_blocking();

    // forever write strings from core 0 to sd card
    uint i = 0;
    while (true) {
        // protect writebuffer access
        mutex_enter_blocking(&my_mutex);
        char* wb_out = writebuffer_out(wb);
        mutex_exit(&my_mutex);

        // continue if no logs to write
        if (wb_out == NULL) {
            continue;
        }

        // write logs
        else {
            f_printf(&f, "%s\n", wb_out);

            // sync sd card every SAVE_INTERVAL logs
            if ((i++ & (SAVE_INTERVAL - 1)) == 0) {
                f_sync(&f);
            }
        }
    }
}

int main() {
    // init stdio
    stdio_init_all();

    // init cyw43 
    if (cyw43_arch_init()) {
        printf("wifi init failed\n");
        return -1;
    }

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    gpio_init(0);
    gpio_set_dir(0, GPIO_IN);
    while (gpio_get(0) != 1) {
        continue;
    }

    // launch core 1
    multicore_launch_core1(core1_entry);
   
    // init pcf8520 clock
    picowbell_pcf8520_init();

    // light up the logging indicator
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    
    // get current time
    datetime_t t = picowbell_pcf8520_get_time();

    // initialize writebuffer for cross-core communication
    struct writebuffer wb;
    wb.in = wb.out = 0;

    // push writebuffer address to core 1
    multicore_fifo_push_blocking((uintptr_t) &wb);

    // start logs
    sleep_ms(1000);
    printf("starting logs...\n");

    // main logging loop
    while (true) {
        // wait until next second
        picowbell_pcf8520_wait_next_second(&t);

        // send a log string to core 1 every DELTA_T microseconds  
        absolute_time_t log_time = get_absolute_time();
        for (uint i = 0; i < HZ; i++) {
            // flash led for 1st half of second
            if (i > (HZ / 2)) cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

            // protect writebuffer access 
            mutex_enter_blocking(&my_mutex);
            char* wb_in = writebuffer_in(&wb);
            mutex_exit(&my_mutex);

            // if buffer is full, print error
            if (wb_in == NULL) {
                printf("core 0: full buffer!\n");
            }

            // write to buffer
            else {
                sprintf(wb_in, "%02d/%02d/%02d-%02d:%02d:%02d", t.month, t.day, t.year, t.hour, t.min, t.sec);
            }

            // sleep until time to write next log
            log_time = delayed_by_us(log_time, DELTA_T);
            sleep_until(log_time);
        }

        // turn led off
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    }

    return 0;
}


