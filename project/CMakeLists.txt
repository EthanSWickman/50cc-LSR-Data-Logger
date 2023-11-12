cmake_minimum_required(VERSION 3.27.7)

include(pico_sdk_import.cmake)

project(logger C CXX ASM)

set(PICO_BOARD pico_w)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_subdirectory(src)

add_executable(runloop ./main.c)

target_include_directories(runloop PUBLIC include)

target_include_directories(pcf8520 PUBLIC include)

target_link_libraries(pcf8520
    pico_stdlib
    hardware_i2c
)

target_link_libraries(runloop 
    pico_stdlib
    pico_cyw43_arch_none
    hardware_i2c
    pcf8520
)

target_include_directories(runloop PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
)

pico_enable_stdio_usb(runloop 1)
pico_enable_stdio_uart(runloop 0)

pico_add_extra_outputs(runloop)
