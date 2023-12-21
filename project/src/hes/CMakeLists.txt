add_library(hes hes.c)

target_include_directories(hes PUBLIC "${PROJECT_SOURCE_DIR}/include")

target_link_libraries(hes
    pico_stdlib
    hardware_pwm
)