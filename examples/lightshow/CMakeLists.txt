cmake_minimum_required(VERSION 3.27.7)

include(pico_sdk_import.cmake)

project(logger C CXX ASM)

set(PICO_BOARD pico_w)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(runloop ./runloop.c)

target_link_libraries(runloop 
pico_stdlib
pico_cyw43_arch_none
hardware_pwm
hardware_adc
)

pico_enable_stdio_usb(runloop 1)
pico_enable_stdio_uart(runloop 0)

pico_add_extra_outputs(runloop)