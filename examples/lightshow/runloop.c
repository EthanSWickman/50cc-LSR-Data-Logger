#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"

int ledState = 0;
int ledPins[] = {16, 17, 18, 19, 20, 21, 22, 28, 27, 13};
int ledStates[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void toggle_pin(int);

long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int main() {
    stdio_init_all();


    if (cyw43_arch_init()) {
        printf("wifi startup failed\n");
        return -1;
    }

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    // potentiometer pin
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    // led pins
    for (int i = 0; i < 10; i++) {
        gpio_init(ledPins[i]);
        gpio_set_dir(ledPins[i], GPIO_OUT);
    }

    // button pin
    gpio_init(14);
    gpio_set_dir(14, GPIO_IN);

    // led pwm pin
    gpio_set_function(0, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(0);

    pwm_set_wrap(slice_num, 255);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);
    pwm_set_enabled(slice_num, true);

    int currLed = 0;
    while (true) {
        if (gpio_get(14) == 1) {
            sleep_ms(1);
            if (gpio_get(14) == 1) {
                ledState = (ledState ? 0 : 1);
                for (int i = 0; i < 10; i++) {
                    gpio_put(ledPins[i], (ledState ? ledStates[i] : ledState));
                }
                while (gpio_get(14) == 1) {
                    sleep_ms(1);
                }
            }
        }

        uint16_t result = adc_read();
        printf("adc result: %d\n", result);


        if (ledState) {
            toggle_pin(currLed);
            sleep_ms(50);
        }

        currLed = (currLed == 9 ? 0 : currLed+1);
    
        pwm_set_chan_level(slice_num, PWM_CHAN_A, map(result, 0, 4095, 0, 255));
    }

    return 0;
}

void toggle_pin(int pin) {
    ledStates[pin] = (ledStates[pin] ? 0 : 1);
    gpio_put(ledPins[pin], ledStates[pin]);
}