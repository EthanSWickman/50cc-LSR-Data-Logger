#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#include "picowbell_pcf8520.h"

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
        char buf[18];
        picowbell_pcf8520_get_time_string(buf);
        printf("%s\n", buf);
        sleep_ms(1000);
    }

    return 0;
}