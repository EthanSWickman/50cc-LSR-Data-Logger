#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#include "pcf8520.h"

int main() {
    stdio_init_all();

    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    pcf8520_init();

    while (true) {
        char buf[18];
        pcf8520_get_time_string(buf);
        printf("%s\n", buf);
        sleep_ms(1000);
    }

    return 0;
}