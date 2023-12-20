#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/binary_info.h"
#include "pico/multicore.h"

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"
#include "hardware/timer.h"

#include "picowbell_pcf8520.h"
#include "picowbell_sd_card.h"
#include "writebuffer.h"

auto_init_mutex(my_mutex);

const volatile uint HZ = 50; // logs per second (keep under 50 until we solve high frequency problems!)
volatile uint DELTA_T;
const uint SAVE_INTERVAL = 256; // should be 2^n for some integer n, represents how many logs until we save results to sd card
const uint PWM_COUNTER_MAX = 65535; // value pwm counter overflows at
const uint PWM_GPIO_PIN = 15; //chose gpio 22 because is even (see pwm_counter_setup) and doesn't have any other function
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

//setup PWM counter
/*
the basic idea of the counter is this: count the number of edges detected from 
the HES using a PWM slice. since the counter is only 16 bits, it will 
overflow when its value is 2^16 - 1 and you try to increment it.
this is why we keep track of the number of wraps using an interrupt handler.
code is heavily inspired by this github repository reccomended by ethan: 
https://github.com/richardjkendall/rp2040-freq-counter/blob/main/pwm/main.c
since we already have a timing system in place implenting a pps (pulse per second)
using a gpio was not needed.
*/ 
uint pwm_counter_setup(uint gpio) {
    //DO NOT try to use an even GPIO (gpio 0, 2, 4 etc) channel, will not work
    printf("pwm is on channel b: %b\n", pwm_gpio_to_channel(gpio) == PWM_CHAN_B);
    //convert the gpio number into a PWM slice
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_config cfg = pwm_get_default_config();
    //set the pwm to detect rising edge 
    pwm_config_set_clkdiv_mode(&cfg, PWM_DIV_B_RISING);
    pwm_config_set_clkdiv(&cfg, 1.f);
    pwm_init(slice_num, &cfg, false);
    //tell gpio to be assigned to a PWM slice 
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    //setup nterrupt handler to deal with overflows
    pwm_set_irq_enabled(slice_num, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, on_pwm_wrap);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    pwm_set_enabled(slice_num, true);
    return slice_num; 
}

/*
actual velocity calculation is done in other core so we dont affect the 
max detectable rpms. it would have to get to above 65k rpms to matter,
so we are probablly fine with or without the wrap checking. including just 
to be safe, will remove if multiply is negatively impacting performance.
*/
uint calc_rotations() {
    //calculate current rotations to add to the write buffer
    uint counter = pwm_get_counter(pwm_slice);
    printf("current pwm counter value %i for slice number %i\n", counter, pwm_slice);
    uint rotations = (signal_wrap_count * PWM_COUNTER_MAX) + counter;
    printf("total rotations: %i\n", rotations);
    signal_wrap_count = 0; //reset count for next time interval
    pwm_set_counter(pwm_slice, 0); 
    return rotations;
}

void alarm0_irq() {
    //library should clear interrupt for us
    alarm0_triggers++;
    hardware_alarm_set_target(0, 1000000 * (alarm0_triggers + 1)); //reset alarm to go off every time interval
    alarm0_flag = true;
    printf("alarm triggered, global timer is %i\n", time_us_64());
    return;
}

//purely exists to test program without cowbell 
//as that has the RTC that the timing system relies on
void setup_test_alarm(uint alarm_num) {
    puts("claiming alarm...");
    hardware_alarm_claim(alarm_num); //0 == alarm 0 interrupt
    hardware_alarm_set_target(alarm_num, 1000000); //set harware alarm to go off in DELTA_T micorseconds
    hardware_alarm_set_callback(alarm_num, alarm0_irq); //assign irq to the alarm to tell it what to do when it is set off
    return;
}

//only refactored to make toggling between alarm / rtc easier
void write_loop(uint64_t log_time) {    
    printf("input log time: %i\n", log_time);
    //calc rotations
    uint rotations = calc_rotations();
    printf("Current rotations: %i\n", rotations);

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
        if (cowbell_enabled)
            sprintf(wb_in, "%02d/%02d/%02d-%02d:%02d:%02d", t.month, t.day, t.year, t.hour, t.min, t.sec, rotations);
        else //purely for testing if dont have cowbell access
            sprintf(wb_in, "%02d/%02d/%02d-%02d:%02d:%02d", 0, 0, 0, 0, 0, 0, rotations);
    }

    //previous part of function takes a certain amount of time ot execute, must be accounted for
    if (cowbell_enabled) {
        puts("delaying until next log");
        // sleep until time to write next log
        log_time = delayed_by_us(log_time, DELTA_T);
        sleep_until(log_time);
    }
    puts("finished writing to write buffer, waiting for interrupt...");
    printf("global timer is %i\n", time_us_64());
}

int main() {
    DELTA_T = 1000000 / HZ;
    if (!cowbell_enabled) {
        //setup alarm if no cowbell
        setup_test_alarm(0);
    }
    else {
        // init pcf8520 clock
        picowbell_pcf8520_init();
        // get current time
        t = picowbell_pcf8520_get_time();
    }
    puts("Setting up...");
    // init stdio
    stdio_init_all();

    // init cyw43 
    if (cyw43_arch_init()) {
        printf("wifi init failed\n");
        return -1;
    }

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    //setup HES
    gpio_pull_up(PWM_GPIO_PIN);

    //setup PWM counter
    pwm_slice = pwm_counter_setup(PWM_GPIO_PIN);

    // launch core 1
    multicore_launch_core1(core1_entry);

    // light up the logging indicator
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    // initialize writebuffer for cross-core communication
    wb.in = wb.out = 0;

    // push writebuffer address to core 1
    multicore_fifo_push_blocking((uintptr_t) &wb);

    alarm0_flag = false;

    // start logs
    printf("starting logs...\n");
    printf("Cowbell is enabled: %b\n", cowbell_enabled);

    // main logging loop
    while (cowbell_enabled) {
        // wait until next second
        picowbell_pcf8520_wait_next_second(&t);

        for (uint i = 0; i < HZ; i++) {
            // flash led for 1st half of second
            // send a log string to core 1 every DELTA_T microseconds  
            uint64_t log_time = time_us_64();
            printf("log time before passed to function: %i\n");
            printf("current iteration is: %i\n", i);
            if (i + 1 > (HZ / 2)) cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            else {cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);}
            write_loop(log_time);
        }
    }

    while (!cowbell_enabled) {
        if (alarm0_flag) {
            puts("processing alarm in main thread");
            if (alarm0_triggers % 2 == 0) cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            else {cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);}
            write_loop(0);
            alarm0_flag = false;
        }
    }
}
