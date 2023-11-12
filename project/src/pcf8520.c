#include <stdio.h>

#include "pcf8520.h"

#include "hardware/i2c.h"

const int I2CADDR = 0x68;

const int SDAPIN = 4;
const int SCLPIN = 5;

void pcf8520_init() {
    // initialize i2c0 controller
    i2c_init(i2c0, 400 * 1000);

    // set up pins
    gpio_set_function(SCLPIN, GPIO_FUNC_I2C);
    gpio_set_function(SDAPIN, GPIO_FUNC_I2C);

    // pull pins high
    gpio_pull_up(SCLPIN);
    gpio_pull_up(SDAPIN);
}

void pcf8520_get_time_string(char* buffer) {
    struct time t = pcf8520_get_time();

    sprintf(buffer, "%02u:%02u:%02u %02u/%02u/%02u", t.hours, t.minutes, t.seconds, t.months, t.days, t.years);
}

struct time pcf8520_get_time() {
    struct time currTime; 

    uint8_t reg = 0x03;
    uint8_t rawTime[7];

    // read raw time from clock registers
    i2c_write_blocking(i2c0, I2CADDR, &reg, 1, true);
    i2c_read_blocking(i2c0, I2CADDR, rawTime, 7, false);

    // convert raw data to time
    currTime.seconds = (10 * ((rawTime[0] & 0x70) >> 4) + (rawTime[0] & 0xF));
    currTime.minutes = (10 * ((rawTime[1] & 0x70) >> 4) + (rawTime[1] & 0xF));
    currTime.hours = (10 * ((rawTime[2] & 0x18) >> 4) + (rawTime[2] & 0xF));
    currTime.days = (10 * ((rawTime[3] & 0x18) >> 4) + (rawTime[3] & 0xF));
    currTime.weekDays = rawTime[4] & 0x7;
    currTime.months = (10 * ((rawTime[5] & 0x10) >> 4) + (rawTime[5] & 0xF));
    currTime.years = (10 * ((rawTime[6] & 0xF0) >> 4) + (rawTime[6] & 0xF));

    return currTime;
}