#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

struct i2cpins {
    uint sda;
    uint scl;
};

struct i2cpins clock = {
    .sda = 4,
    .scl = 5,
};

int addr = 0x68;

void pcf8520_set_time() {

}

void pcf8520_read_time(uint8_t* buffer) {
    uint8_t registerVal = 0x03;

    i2c_write_blocking(i2c0, addr, &registerVal, 1, true);
    i2c_read_blocking(i2c0, addr, (uint8_t*) buffer, 7, false);
}

uint8_t pcf8520_convert_time(uint8_t* buffer) {
    buffer[0] = (10 * ((buffer[0] & 0x70) >> 4) + (buffer[0] & 0xF));
    buffer[1] = (10 * ((buffer[1] & 0x70) >> 4) + (buffer[1] & 0xF));
    buffer[2] = (10 * ((buffer[2] & 0x18) >> 4) + (buffer[2] & 0xF));
    buffer[3] = (10 * ((buffer[3] & 0x18) >> 4) + (buffer[3] & 0xF));
    buffer[4] = buffer[4] & 0x7;
    buffer[5] = (10 * ((buffer[5] & 0x10) >> 4) + (buffer[5] & 0xF));
    buffer[6] = (10 * ((buffer[6] & 0xF0) >> 4) + (buffer[6] & 0xF));
}

int main() {
    stdio_init_all();

    printf("starting...\n");

    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    // set up i2c ports
    i2c_init(i2c0, 400 * 1000); // use i2c0 and baud rate: 400,000
    gpio_set_function(clock.scl, GPIO_FUNC_I2C); // set scl pin to i2c mode
    gpio_set_function(clock.sda, GPIO_FUNC_I2C);
    gpio_pull_up(clock.scl); // high when this is floating
    gpio_pull_up(clock.sda);

    while (true) {
        uint8_t time[7] = {0, 0, 0, 0, 0, 0, 0};
        pcf8520_read_time(time);
        pcf8520_convert_time(time);
        printf("%02u:%02u:%02u - %02u/%02u/%02u\n", time[2], time[1], time[0], time[5], time[3], time[6]);
        sleep_ms(2000);
    }

    return 0;
}