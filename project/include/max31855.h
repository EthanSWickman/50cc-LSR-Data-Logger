#ifndef MAX31855_H
#define MAX31855_H

#include "pico/stdlib.h"

void max31855_init(
    int SCK_PIN,
    int TX_PIN,
    int RX_PIN,
    int CSN_PIN
);

void max31855_getTemp(
    uint8_t data[],
    float* buff
);

void max31855_readToBuffer(
    int CSN_PIN,
    uint8_t data[],
    float* buff
);

#endif