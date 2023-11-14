cmake_minimum_required(VERSION 3.27.7)

include(pico_sdk_import.cmake)

project(logger C CXX ASM)

set(PICO_BOARD pico_w)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(led_indicator ./led_indicator.c)

target_link_libraries(led_indicator
pico_stdlib
pico_cyw43_arch_none
)

pico_enable_stdio_usb(led_indicator 1)
pico_enable_stdio_uart(led_indicator 0)

pico_add_extra_outputs(led_indicator)