add_library(picowbell_pcf8520 picowbell_pcf8520.c)

target_include_directories(picowbell_pcf8520 PUBLIC "${PROJECT_SOURCE_DIR}/include")

target_link_libraries(picowbell_pcf8520
    pico_stdlib
    hardware_i2c
)