set(PROJECT velocity)
cmake_minimum_required(VERSION 3.5)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
project(${PROJECT} C CXX ASM)
set(CMAKE_C_STANDARD 11)
pico_sdk_init()
add_executable(${PROJECT} velocity.c)

add_library(linked_list ll.c)

target_link_libraries(${PROJECT} PUBLIC
    pico_multicore
    pico_stdlib
    linked_list
)

target_include_directories(${PROJECT} PUBLIC
    "."
    "$ENV{PICO_SDK_PATH}"
)

pico_enable_stdio_usb(${PROJECT} 1)
pico_enable_stdio_uart(${PROJECT} 0)

pico_add_extra_outputs(${PROJECT})