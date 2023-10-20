cmake_minimum_required(VERSION 3.27.7)

include(pico_sdk_import.cmake)

project(logger C CXX ASM)

set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(runloop ./runloop.c)

target_link_libraries(runloop pico_stdlib)