#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "max31855.h"
#include "pin_config.h"


void max31855_init(int SCK_PIN, int TX_PIN, int RX_PIN, int CSN_PIN) {

    spi_init(spi1, 5000 * 1000);
    gpio_set_function(RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SCK_PIN, GPIO_FUNC_SPI);

    gpio_init(CSN_PIN);
    gpio_set_dir(CSN_PIN, GPIO_OUT);
    gpio_put(CSN_PIN, true);

    bool circuit_open = false;
    bool circuit_vcc = false;
    bool circuit_gnd = false;

}

void max31855_getTemp(uint8_t data[], float* buf){

    /*
    float media_temp, media_counter = 0;
    */
    if (data[1] & 0b00000001) {
        printf("Thermocouple Error - ");

        if (data[3] & 0b00000001) {
            printf("Circuit disconnected!\n");
        }

        if (data[3] & 0b00000010) {
            printf("Thermocouple shorted to GND\n");
        }

        if (data[3] & 0b00000100) {
            printf("Thermocouple shorted to VCC\n");
        }
    }
    

    float part_decimal = ((data[1] & 0b00001100) >> 2) * 0.25;
    int part_entire = data[1] >> 4;
    part_entire = part_entire + (data[0] << 4);

    float sensor_temp = part_entire + part_decimal;
    sensor_temp = sensor_temp * (9/5) + 52;

    printf("Sensor temp: %.2f\n", sensor_temp);
    *buf = sensor_temp;

    /*if (media_counter < total_iterations) {
        media_temp += temperatura_sensor;
        media_counter++;
    } else {
        media_temp = media_temp / total_iterations;
        p
        media_counter = 0;
        media_temp = 0;
    }*/ 
    //sleep_ms(100);
    
}

void max31855_readToBuffer(int CSN_PIN, uint8_t data[], float* buff){
    gpio_put(CSN_PIN, false);
    //sleep_us(10);
    spi_read_blocking(spi1, 0, &data, 4);
    //sleep_us(10);
    gpio_put(CSN_PIN, true);
    
    max31855_getTemp(data, &buff);
}