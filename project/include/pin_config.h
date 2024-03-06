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
SPI1: max31855

I2C0: pcf8520 
I2C1: lcd

UART0:
UART1:

*/

#define LOG_START_BUTTON_PIN 0 // bridged by button to start logs
#define VELOCITY_PWM_PIN 1 // hall effect sensor on wheel
#define RPM_PWM_PIN 2 // unimplemented, frequency counter measuring rpm of engine
//#define PIN_3 = 3
#define PCF8520_SDA_PIN 4 // ADALOGGER IMPOSED
#define PCF8520_SCL_PIN 5 // ADALOGGER IMPOSED
#define LCD_SDA_PIN 6 // unimplemented, lcd sda pin
#define LCD_SCL_PIN 7 // unimplemented, lcd scl pin
//#define PIN_8 = 8
//#define PIN_9 = 9
#define Thermo_SCK_PIN 10 // max31855, flexible (SPI)
#define Thermo1_CSN_PIN 11 // chip select for first thermocouple
#define Thermo_RX_PIN 12 // max31855, flexible (SPI), connected to max31855 DO pin
#define Thermo2_CSN_PIN 13 // chip select for first thermocouple
#define Thermo3_CSN_PIN 14 // chip select for first thermocouple
#define Thermo4_CSN_PIN 15 // chip select for first thermocouple
#define SD_MISO_PIN 16 // ADALOGGER IMPOSED
#define SD_CS_PIN 17 // ADALOGGER IMPOSED
#define SD_SCK_PIN 18 // ADALOGGER IMPOSED
#define SD_MOSI_PIN 19 // ADALOGGER IMPOSED
//#define PIN_20 = 20
//#define PIN_21 = 21
//#define PIN_22 = 22
#define AIR_FUEL_RATIO_PIN 26 // unimplemented, 0-3.3V sensing attached to voltage divider attached to 0-5V AFR sensor
#define BUTTON_PIN 27 // unimplemented, analog input to allow for several buttons on one pin
//#define PIN_25 = 28

#endif