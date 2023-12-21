#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

#include "pico/stdlib.h"

/*

This config file is for reserving pins. Take care to document your sensors

Pinout:

[0  - SPI0 RX  - I2C0 SDA - UART0 TX] -------- [Vbus]
[1  - SPI0 CSn - I2C0 SCL - UART0 RX] -------- [Vsys]
[GND] ---------------------------------------- [GND]
[2  - SPI0 SCK - I2C1 SDA] ------------------- [3V3_EN]
[3  - SPI0 TX  - I2C1 SCL] ------------------- [3V3(OUT)]
[4  - SPI0 RX  - I2C0 SDA - UART1 TX] -------- [ADC_VREF]
[5  - SPI0 CSn - I2C0 SCL - UART1 RX] -------- [28  - ADC2]
[GND] ---------------------------------------- [GND - AGND]
[6  - SPI0 SCK - I2C1 SDA] ------------------- [27  - ADC1     - I2C1 SCL]
[7  - SPI0 TX  - I2C1 SCL] ------------------- [26  - ADC0     - I2C1 SCL]
[8  - SPI1 RX  - I2C0 SDA - UART1 TX] -------- [RUN]
[9  - SPI1 CSn - I2C0 SCL - UART1 RX] -------- [22]
[GND] ---------------------------------------- [GND]
[10 - SPI1 SCK - I2C1 SDA] ------------------- [21  -          - I2C0 SCL]
[11 - SPI1 TX  - I2C1 SCL] ------------------- [20  -          - I2C0 SDA]
[12 - SPI1 RX  - I2C0 SDA - UART0 TX] -------- [19  - SPI0 TX  - I2C1 SCL]
[13 - SPI1 CSn - I2C0 SCL - UART0 RX] -------  [18  - SPI0 SCK - I2C1 SDA]
[GND] ---------------------------------------- [GND]
[14 - SPI1 SCK - I2C1 SDA] ------------------- [17  - SPI0 CSn - I2C0 SCL - UART0 RX]
[15 - SPI1 TX  - I2C1 SCL] ------------------- [16  - SPI0 RX  - I2C0 SDA - UART0 TX]


Controller Reservations (please avoid conflicts until necessary): 

SPI0: sd card
SPI1: 

I2C0: pcf8520 
I2C1: 

UART0:
UART1:

*/

#define LOG_START_BUTTON_PIN 0
#define PWM_GPIO_PIN 1
//const uint PIN = 2;
//const uint PIN = 3;
#define PCF8520_SDA_PIN 4 // ADALOGGER IMPOSED
#define PCF8520_SCL_PIN 5 // ADALOGGER IMPOSED
//const uint PIN = 6;
//const uint PIN = 7;
//const uint PIN = 8;
//const uint PIN = 9;
#define Thermo1_SCK_PIN 10 // max31855, flexible (SPI)
#define Thermo1_TX_PIN 11 // max31855, flexible (SPI), not actually used by max31855
#define Thermo1_RX_PIN 12 // max31855, flexible (SPI), connected to max31855 DO pin
#define Thermo1_CSN_PIN 13 // max31855, flexible (SPI)
//const uint PIN = 14;
//const uint PIN = 15;
#define SD_MISO_PIN 16 // ADALOGGER IMPOSED
#define SD_CS_PIN 17 // ADALOGGER IMPOSED
#define SD_SCK_PIN 18 // ADALOGGER IMPOSED
#define SD_MOSI_PIN 19 // ADALOGGER IMPOSED
//const uint PIN = 20;
//const uint PIN = 21;
//const uint PIN = 22;
//const uint PIN = 26;
//const uint PIN = 27;
//const uint PIN = 28;

#endif