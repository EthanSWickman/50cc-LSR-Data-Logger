cmake_minimum_required(VERSION 3.27.7)

include(pico_sdk_import.cmake)

project(logger C CXX ASM)

set(PICO_BOARD pico_w)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(main ./main.c)

target_link_libraries(main
pico_stdlib
)

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)

pico_add_extra_outputs(main)