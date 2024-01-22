#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "max31855.h"
#include "pin_config.h"


/*
    name: max31855_init
    function: initialize thermocouple instance, must be done before any readings
    params:
        SCK_PIN = thermocouple X's SCK (clock) pin #
        RX_PIN = thermocouple X's RX (recieve) pin # 
        CSN_PIN = thermocouple X's CSN (chip select) pin #
        SPI = spi0 or spi1
*/
void max31855_init(int SCK_PIN, int RX_PIN, int CSN_PIN, spi_inst_t* SPI) {
    
    // set spi # and baudrate (5000k works)
    spi_init(SPI, 5000 * 1000);
    
    // setup pins for SPI
    gpio_set_function(RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SCK_PIN, GPIO_FUNC_SPI);

    // setup chip select
    gpio_init(CSN_PIN);
    gpio_set_dir(CSN_PIN, GPIO_OUT);
    gpio_put(CSN_PIN, true);
}

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
void max31855_getTemp(uint8_t data[], float* buff, int verbose){

    // check for circuit errors
    // if d16 == 1, fault has occured
    if (data[1] & 0b00000001) {
        printf("Thermocouple Error - ");
        
        // if d0 == 1
        if (data[3] & 0b00000001) {
            printf("Circuit disconnected!\n");
        }

        // if d1 == 1
        if (data[3] & 0b00000010) {
            printf("Thermocouple shorted to GND\n");
        }

        // if d2 == 1
        if (data[3] & 0b00000100) {
            printf("Thermocouple shorted to VCC\n");
        }
    }
    
    // convert data into temperature
    // based on table 2 from datasheet (https://www.analog.com/media/en/technical-documentation/data-sheets/max31855.pdf)

    // show data's hexadecimal representation (for debugging)
    /*for(int i = 0; i < 4; i++){
        printf("%x ", data[i]);
    }
    printf("\n");*/

    // concatenate first and second bytes into 16 bits
    // 0 = sign, 1-11 = 2^10 through 2^0, 12-13 = 2^-1 through 2^-2, 14 = reserved (0), 15 = fault
    uint16_t int_part = (data[0] << 8) | (data[1]);

    // subtract away sign bit, if there, so we can pretend first 12 bits are one "normal" number
    if(int_part >= 0b1000000000000000){
        int_part -= 0b1000000000000000;
    }

    // shift away bits not part of int and we're left with complete integer part of data
    int_part = int_part >> 4;

    // convert anything at 2^-1 or 2^-2 bits to float value
    float dec_part = (float)((data[1] & 0b00001100)) / 16;
    
    // add postdecimal to integer
    float sensor_temp_C = int_part + dec_part;
    
    // check sign bit for negatives
    if((data[0] & 0b10000000) == 128){
        sensor_temp_C = sensor_temp_C * -1;
    }

    // convert to fahrenheit
    float sensor_temp_F = (sensor_temp_C * 1.8 + 32); 

    // print temp if verbose is true
    if(verbose){
        printf("Sensor temp: %.2f F\n", sensor_temp_F);
    }

    // take sensor temperature and put it at buffer
    *buff = sensor_temp_F;
}


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
void max31855_readToBuffer(int CSN_PIN, uint8_t data[], float* buff, int verbose){

    // read current thermocouple signal to data
    gpio_put(CSN_PIN, false);
    spi_read_blocking(spi1, 0, data, 4);
    gpio_put(CSN_PIN, true);

    // interpret signal and write it to buff
    max31855_getTemp(data, buff, verbose);
}
