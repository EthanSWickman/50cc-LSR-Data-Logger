#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#include "picowbell_pcf8520.h"
#include "picowbell_sd_card.h"

int main() {
    // init stdio
    stdio_init_all();

    // init cyw43 
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    // init pcf8520 clock
    picowbell_pcf8520_init();

    // main logging loop
    while (true) {
        // main data logging buffer
        char mainBuf[20];

        // get timestamp from clock
        char timeBuf[18];
        picowbell_pcf8520_get_time_string(timeBuf);

        // test buffer
        char buf[6] = {'h', 'e', 'l', 'l', 'o', '\0'};

        // assemble buffer
        sprintf(mainBuf, "%s,%s", timeBuf, buf);

        // log buffer
        picowbell_sd_card_write_line(mainBuf);
        sleep_ms(1000);
    }

    return 0;
}