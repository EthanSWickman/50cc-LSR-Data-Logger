add_library(max31855 max31855.c)

target_include_directories(max31855 PUBLIC "${PROJECT_SOURCE_DIR}/include")

target_link_libraries(max31855
    pico_stdlib
    hardware_spi
)