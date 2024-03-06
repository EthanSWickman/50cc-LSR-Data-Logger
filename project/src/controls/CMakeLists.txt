add_library(controls controls.c)

target_include_directories(controls PUBLIC "${PROJECT_SOURCE_DIR}/include")

target_link_libraries(controls
    pico_stdlib
    lcd
    pico_rand
    hardware_adc
)