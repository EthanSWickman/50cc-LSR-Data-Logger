#include <stdio.h>

#include "picowbell_pcf8520.h"

#include "hardware/i2c.h"

const int CLOCK_ADDR = 0x68;

const int SDA_PIN = 4;
const int SCL_PIN = 5;

void picowbell_pcf8520_init() {
    // initialize i2c0 controller
    i2c_init(i2c0, 400 * 1000);

    // set up pins
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);

    // pull pins high
    gpio_pull_up(SCL_PIN);
    gpio_pull_up(SDA_PIN);
}

void picowbell_pcf8520_get_time_string(char* buffer) {
    // get time struct
    struct time t = picowbell_pcf8520_get_time();

    // format time struct into string
    sprintf(buffer, "%02u:%02u:%02u %02u/%02u/%02u", t.hours, t.minutes, t.seconds, t.months, t.days, t.years);
}

struct time picowbell_pcf8520_get_time() {
    uint8_t reg = 0x03; // 0x03 is address of seconds register on pcf8520
    uint8_t rawTime[7];

    // read raw time from clock registers
    i2c_write_blocking(i2c0, CLOCK_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c0, CLOCK_ADDR, rawTime, 7, false);

    // convert raw data to time
    struct time currTime; 
    currTime.seconds = (10 * ((rawTime[0] & 0x70) >> 4) + (rawTime[0] & 0x0F));
    currTime.minutes = (10 * ((rawTime[1] & 0x70) >> 4) + (rawTime[1] & 0x0F));
    currTime.hours = (10 * ((rawTime[2] & 0x30) >> 4) + (rawTime[2] & 0x0F));
    currTime.days = (10 * ((rawTime[3] & 0x30) >> 4) + (rawTime[3] & 0x0F));
    currTime.weekDays = rawTime[4] & 0x07;
    currTime.months = (10 * ((rawTime[5] & 0x10) >> 4) + (rawTime[5] & 0x0F));
    currTime.years = (10 * ((rawTime[6] & 0xF0) >> 4) + (rawTime[6] & 0x0F));

    return currTime;
}

void picowbell_pcf8520_set_time(struct time time) {
    uint8_t timeBuf[7]; 

    // convert time struct to pcf8520 BCD format
    timeBuf[0] = (time.seconds / 10 << 4) + (time.seconds - 10 * (time.seconds / 10));
    timeBuf[1] = (time.minutes / 10 << 4) + (time.minutes - 10 * (time.minutes / 10));
    timeBuf[2] = (time.hours / 10 << 4) + (time.hours - 10 * (time.hours / 10));
    timeBuf[3] = (time.days / 10 << 4) + (time.days - 10 * (time.days / 10));
    timeBuf[4] = time.weekDays;
    timeBuf[5] = (time.months / 10 << 4) + (time.months - 10 * (time.months / 10));
    timeBuf[6] = (time.years / 10 << 4) + (time.years - 10 * (time.years / 10));

    // write time to clock registers
    uint8_t writeBuf[2];
    for (int i = 0; i < 7; i++) {
        writeBuf[0] = i + 3;
        writeBuf[1] = timeBuf[i];
        i2c_write_blocking(i2c0, CLOCK_ADDR, writeBuf, 2, false);
    }
}