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
#include "pin_config.h"

auto_init_mutex(my_mutex);

const volatile uint HZ = 50; // logs per second (keep under 50 until we solve high frequency problems!)
volatile uint DELTA_T;
const uint SAVE_INTERVAL = 256; // should be 2^n for some integer n, represents how many logs until we save results to sd card
const bool cowbell_enabled = true; //toggle to false if you are testing without a cowbell, toggle on if testing with

volatile uint signal_wrap_count = 0;
volatile uint pwm_slice;
volatile bool alarm0_flag; //tells main loop wether to process new information or not
volatile uint alarm0_triggers = 0;
struct writebuffer wb;
datetime_t t;

void core1_entry() {
    if (cowbell_enabled) {
        // init sd card
        FATFS fs;
        picowbell_sd_card_init(&fs);

        // open new log file
        FIL f;
        picowbell_sd_card_new_log(&f, 0);

        // receive writebuffer address for string communication between cores
        struct writebuffer* wb = (struct writebuffer*) multicore_fifo_pop_blocking();

        // forever write strings from core 0 to sd card
        uint i = 0; //could theoretically overflow, not extremely likely but may cause wierd bugs after several hours 
        while (true) {
            //printf("writing write buffer to sd card...\n");
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
    
}

void on_pwm_wrap() {
    pwm_clear_irq(pwm_slice);
    signal_wrap_count++;
}

void write_loop(uint64_t log_time, int numThermos) {    
    //calc rotations
    uint rotations = calc_rotations(pwm_slice, &signal_wrap_count);

    // max31855
    switch (numThermos) {
        case 2: 
            uint8_t thermo_data_2[4];
            float thermo2_data_output;
            max31855_readToBuffer(Thermo2_CSN_PIN, thermo_data_2, &thermo1_data_output, 1);
        case 3: 
            uint8_t thermo_data_3[4];
            float thermo3_data_output;
            max31855_readToBuffer(Thermo3_CSN_PIN, thermo_data_3, &thermo2_data_output, 1);
        case 4: 
            uint8_t thermo_data_4[4];
            float thermo4_data_output;
            max31855_readToBuffer(Thermo4_CSN_PIN, thermo_data_4, &thermo3_data_output, 1);
        default: 
            uint8_t thermo_data_1[4];
            float thermo1_data_output;
            max31855_readToBuffer(Thermo1_CSN_PIN, thermo_data_1, &thermo4_data_output, 1);
            break;
    }

    // protect writebuffer access 
    mutex_enter_blocking(&my_mutex);
    char* wb_in = writebuffer_in(&wb);
    mutex_exit(&my_mutex);

    // if buffer is full, print error
    if (wb_in == NULL) {
        printf("core 0: full buffer!\n");
        return;
    }
    else {
        printf("writing to write buffer...\n");
        // write to buffer
        sprintf(wb_in, "%02d/%02d/%02d-%02d:%02d:%02d:%02d", t.month, t.day, t.year, t.hour, t.min, t.sec, rotations, thermo1_data_output, thermo2_data_output, thermo3_data_output, thermo4_data_output);
    }

    puts("delaying until next log");
    // sleep until time to write next log
    log_time = delayed_by_us(log_time, DELTA_T);
    sleep_until(log_time);
}

int main(int argc, char **argv) {
    DELTA_T = 1000000 / HZ;

    // init pcf8520 clock
    picowbell_pcf8520_init();
    // get current time
    t = picowbell_pcf8520_get_time();

    // init stdio
    stdio_init_all();

    // init cyw43 
    if (cyw43_arch_init()) {
        printf("wifi init failed\n");
        return -1;
    }

    //setup HES
    gpio_pull_up(PWM_GPIO_PIN);

    //setup PWM counter
    pwm_slice = pwm_counter_setup(PWM_GPIO_PIN, on_pwm_wrap);

    gpio_init(LOG_START_BUTTON_PIN);
    gpio_set_dir(LOG_START_BUTTON_PIN, GPIO_IN);
    while (gpio_get(LOG_START_BUTTON_PIN) != 1) {
        continue;
    }

    // launch core 1
    multicore_launch_core1(core1_entry);

    // init max31855 thermocouples 
    int numThermos = 1;
    if (argc >= 1) 
        numThermos = atoi(argv[1]);

    //init only the amount specified by command line argument, number is set in the CMAKE file
    //only two spi channels so each set of 2 needs to share one
    switch (numThermos) {
        case 2:
            max31855_init(Thermo2_SCK_PIN, Thermo2_TX_PIN, Thermo2_RX_PIN, Thermo2_CSN_PIN, spi1);
        case 3:
            max31855_init(Thermo3_SCK_PIN, Thermo3_TX_PIN, Thermo3_RX_PIN, Thermo3_CSN_PIN, spi0);
        case 4:
            max31855_init(Thermo4_SCK_PIN, Thermo4_TX_PIN, Thermo4_RX_PIN, Thermo4_CSN_PIN, spi0);
        default:
            max31855_init(Thermo1_SCK_PIN, Thermo1_TX_PIN, Thermo1_RX_PIN, Thermo1_CSN_PIN, spi1);
            break;
    }

    // init data for thermocouples

    // light up the logging indicator
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    // initialize writebuffer for cross-core communication
    wb.in = wb.out = 0;

    // push writebuffer address to core 1
    multicore_fifo_push_blocking((uintptr_t) &wb);

    alarm0_flag = false;

    // start logs
    printf("starting logs...\n");
    // main logging loop
    while (true) {
        // wait until next second
        picowbell_pcf8520_wait_next_second(&t);

        for (uint i = 0; i < HZ; i++) {
            // flash led for 1st half of second
            // send a log string to core 1 every DELTA_T microseconds  
            uint64_t log_time = time_us_64();
            printf("current iteration is: %i\n", i);
            if (i + 1 > (HZ / 2)) cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            else {cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);}
            write_loop(log_time, numThermos);
        }
    }
}
