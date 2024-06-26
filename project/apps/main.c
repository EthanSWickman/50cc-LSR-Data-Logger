#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/binary_info.h"
#include "pico/multicore.h"
#include  "hardware/spi.h"

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"
#include "hardware/timer.h"

#include "picowbell_pcf8520.h"
#include "picowbell_sd_card.h"
#include "writebuffer.h"
#include "hes.h"
#include "max31855.h"
#include "lcd.h"
#include "controls.h"
#include "pin_config.h"

auto_init_mutex(my_mutex);

const uint HZ = 4; // logs per second (keep under 50 until we solve high frequency problems!)
uint DELTA_T = 1000000 / HZ; // time in microseconds between logs
const uint SAVE_INTERVAL = 16; // should be 2^n for some integer n, represents how many logs until we save results to sd card

volatile uint signal_wrap_count = 0;
volatile uint pwm_slice;

struct writebuffer wb;
datetime_t t;

bool verbose = 0;  // just for thermos atm

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
        // protect writebuffer access and read from writebuffer
        mutex_enter_blocking(&my_mutex);
        char* wb_out = writebuffer_out(wb);
        mutex_exit(&my_mutex);

        // continue if no logs to write
        if (wb_out == NULL) {
            continue;
        }

        // write logs to sd card
        else {
            f_printf(&f, "%s\n", wb_out);

            // sync sd card every SAVE_INTERVAL logs
            if ((i++ & (SAVE_INTERVAL - 1)) == 0) {
                f_sync(&f);
            }
        }
    }
}

void on_pwm_wrap() {
    pwm_clear_irq(pwm_slice);
    signal_wrap_count++;
}

int main() {
    // init stdio
    stdio_init_all();

    // init cyw43 
    cyw43_arch_init();

    // init pcf8520 clock
    picowbell_pcf8520_init();

    //setup velocity PWM counter
    pwm_slice = pwm_counter_setup(VELOCITY_PWM_PIN, on_pwm_wrap);

    // init max31855 thermocouples
    // max31855_init(Thermo_SCK_PIN, Thermo_RX_PIN, Thermo1_CSN_PIN, spi1);
    // max31855_init(Thermo_SCK_PIN, Thermo_RX_PIN, Thermo2_CSN_PIN, spi1);
    // max31855_init(Thermo_SCK_PIN, Thermo_RX_PIN, Thermo3_CSN_PIN, spi1);

    //controls_init();

    //controls_startup();

    gpio_init(LOG_START_BUTTON_PIN);
    gpio_set_dir(LOG_START_BUTTON_PIN, GPIO_IN);
    while (gpio_get(LOG_START_BUTTON_PIN) != 1) {
        continue;
    }
    printf("starting logs\n");

    // lcd_init();

    // launch core 1
    multicore_launch_core1(core1_entry);

    // light up the logging indicator
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    // initialize writebuffer for cross-core communication
    struct writebuffer wb;
    wb.in = wb.out = 0;

    // push writebuffer address to core 1
    multicore_fifo_push_blocking((uintptr_t) &wb);

    // get current time
    t = picowbell_pcf8520_get_time();

    // main logging loop
    while (true) {
        // wait until next second
        picowbell_pcf8520_wait_next_second(&t);
                
        for (uint i = 0; i < HZ; i++) {
            // current time
            absolute_time_t log_time = get_absolute_time();

            // flash led for 1st half of second
            // send a log string to core 1 every DELTA_T microseconds  
            if (i + 1 > (HZ / 2)) cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            else {cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);}

            // calc rotations
            uint rotations = calc_rotations(pwm_slice, &signal_wrap_count);

            // get temperature
            //uint8_t thermo_data[4];
            //float thermo1_data_output;
            //float thermo2_data_output;
            //float thermo3_data_output;
            //max31855_readToBuffer(Thermo1_CSN_PIN, thermo_data, &thermo1_data_output, verbose);
            //max31855_readToBuffer(Thermo2_CSN_PIN, thermo_data, &thermo2_data_output, verbose);
            //max31855_readToBuffer(Thermo3_CSN_PIN, thermo_data, &thermo3_data_output, verbose);

            // protect writebuffer access and get next buffer slot to write to 
            mutex_enter_blocking(&my_mutex);
            char* wb_in = writebuffer_in(&wb);
            mutex_exit(&my_mutex);

            // write to buffer
            if (wb_in != NULL) {
                sprintf(wb_in, "%02d:%02d:%02d,%d ", t.hour, t.min, t.sec, rotations); //, thermo1_data_output, thermo2_data_output, thermo3_data_output);
            printf("%02d:%02d:%02d,%d\n", t.hour, t.min, t.sec, rotations);
            }
            //else {
                //// full buffer error
                //printf("core 0: full buffer!\n");
            //}

            // write to lcd
            // if (i == (HZ / 2)) {
                // write to lcd
            //char buffer[33];
                //// int n = sprintf(buffer, "%.0f %.0f %.0f"); // , thermo1_data_output, thermo2_data_output, thermo3_data_output);
            //int n = sprintf(buffer, "%d", rotations);
            //for (int i = 0; i < 32 - n; i++) {
                //sprintf(&buffer[i], " ");
            //}
            //lcd_write(buffer);
            // }

            // sleep until time to write next log
            log_time = delayed_by_us(log_time, DELTA_T);
            sleep_until(log_time);
        }
    }
}
