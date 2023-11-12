add_library(pcf8520 pcf8520.c)

target_include_directories(pcf8520 PUBLIC "${PROJECT_SOURCE_DIR}/include")

target_link_libraries(pcf8520
    pico_stdlib
    hardware_i2c
)