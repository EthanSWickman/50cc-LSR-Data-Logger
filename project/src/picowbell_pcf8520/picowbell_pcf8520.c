#include <stdio.h>

#include "hardware/i2c.h"

#include "picowbell_pcf8520.h"
#include "pin_config.h"

const int CLOCK_ADDR = 0x68;

void picowbell_pcf8520_init() {
    // initialize i2c0 controller
    i2c_init(i2c0, 400 * 1000);

    // set up pins
    gpio_set_function(PCF8520_SCL_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PCF8520_SDA_PIN, GPIO_FUNC_I2C);

    // pull pins high
    gpio_pull_up(PCF8520_SCL_PIN);
    gpio_pull_up(PCF8520_SDA_PIN);
}

void picowbell_pcf8520_wait_next_second(datetime_t* latestTime) {
    while(true) {
        datetime_t t = picowbell_pcf8520_get_time();
        if (t.sec == (latestTime->sec + 1) % 60) {
            *latestTime = t;
            break;
        }
    }
}

void picowbell_pcf8520_get_time_string(char* buffer) {
    // get time struct
    datetime_t t = picowbell_pcf8520_get_time();

    // format time struct into string
    sprintf(buffer, "%02d:%02d:%02d %02d/%02d/%02d", t.hour, t.min, t.sec, t.month, t.day, t.year);
}

datetime_t picowbell_pcf8520_get_time() {
    uint8_t reg = 0x03; // 0x03 is address of seconds register on pcf8520
    uint8_t rawTime[7];

    // read raw time from clock registers
    i2c_write_blocking(i2c0, CLOCK_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c0, CLOCK_ADDR, rawTime, 7, false);

    // convert raw data to time
    datetime_t currTime; 
    currTime.sec = (10 * ((rawTime[0] & 0x70) >> 4) + (rawTime[0] & 0x0F));
    currTime.min = (10 * ((rawTime[1] & 0x70) >> 4) + (rawTime[1] & 0x0F));
    currTime.hour = (10 * ((rawTime[2] & 0x30) >> 4) + (rawTime[2] & 0x0F));
    currTime.day = (10 * ((rawTime[3] & 0x30) >> 4) + (rawTime[3] & 0x0F));
    currTime.dotw = rawTime[4] & 0x07;
    currTime.month = (10 * ((rawTime[5] & 0x10) >> 4) + (rawTime[5] & 0x0F));
    currTime.year = (10 * ((rawTime[6] & 0xF0) >> 4) + (rawTime[6] & 0x0F));

    return currTime;
}

void picowbell_pcf8520_set_time(datetime_t time) {
    uint8_t timeBuf[7]; 

    // convert time struct to pcf8520 BCD format
    timeBuf[0] = (time.sec / 10 << 4) + (time.sec - 10 * (time.sec / 10));
    timeBuf[1] = (time.min / 10 << 4) + (time.min - 10 * (time.min / 10));
    timeBuf[2] = (time.hour / 10 << 4) + (time.hour - 10 * (time.hour / 10));
    timeBuf[3] = (time.day / 10 << 4) + (time.day - 10 * (time.day / 10));
    timeBuf[4] = time.dotw;
    timeBuf[5] = (time.month / 10 << 4) + (time.month - 10 * (time.month / 10));
    timeBuf[6] = (time.year / 10 << 4) + (time.year - 10 * (time.year / 10));

    // write time to clock registers
    uint8_t writeBuf[2];
    for (int i = 0; i < 7; i++) {
        writeBuf[0] = i + 3;
        writeBuf[1] = timeBuf[i];
        i2c_write_blocking(i2c0, CLOCK_ADDR, writeBuf, 2, false);
    }
}