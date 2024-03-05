#ifndef MAX31855_H
#define MAX31855_H

#include "pico/stdlib.h"

/*
    functions for interacting with max31855 thermocouple
    datasheet available at https://www.analog.com/media/en/technical-documentation/data-sheets/max31855.pdf
*/


/*
    name: max31855_init
    function: initialize thermocouple instance, must be done before any readings
    params:
        SCK_PIN = thermocouple X's SCK (clock) pin #
        RX_PIN = thermocouple X's RX (recieve) pin # 
        CSN_PIN = thermocouple X's CSN (chip select) pin #
        SPI = spi0 or spi1
*/
void max31855_init(
    int SCK_PIN,
    int RX_PIN,
    int CSN_PIN,
    spi_inst_t* SPI
);

/*
    name: max31855_getTemp
    function: read a max31855 signal and convert it into a temperature (currently Fahrenheit)
    params:
        data[(4)] = array of 4 (neccesarily) uint8_ts, will hold all 32 bits of max31855 signal
        buff = memory address of variable temperature will be stored into
        verbose = 0 or 1, select whether or not to print temperature to serial as well
    output:
        number at buff's memory address updated to contain current temperature (float)
        outputs errors for various disconnections/shorts
        if verbose, prints temperature to serial
*/
void max31855_getTemp(
    uint8_t data[],
    float* buff,
    int verbose
);

/*
    name: max31855_readToBuffer
    function: select a given thermocouple and call max31855_getTemp on it to read it to a buffer
    params:
        CSN_PIN = thermocouple X's chip select pin #
        data[(4)] = same as getTemp, will temporarily hold max31855 signal
        buff = memory address temperature will be written to
        verbose = 0 or 1, select whether or not to print reading as well
    output:
        buff's address updated to contain current temperature (float)
        if verbose, prints temperature to serial
*/
void max31855_readToBuffer(
    int CSN_PIN,
    uint8_t data[],
    float* buff,
    int verbose
);

#endif