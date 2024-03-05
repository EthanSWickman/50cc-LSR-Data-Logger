add_library(lcd lcd.c)

target_include_directories(lcd PUBLIC "${PROJECT_SOURCE_DIR}/include")

target_link_libraries(lcd
    pico_stdlib
    hardware_i2c
)