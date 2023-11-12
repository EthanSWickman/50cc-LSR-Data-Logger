add_executable(runloop ./main.c)

target_link_libraries(runloop 
    pico_stdlib
    pico_cyw43_arch_none
    hardware_i2c
    picowbell_pcf8520
)

pico_enable_stdio_usb(runloop 1)
pico_enable_stdio_uart(runloop 0)

pico_add_extra_outputs(runloop)

