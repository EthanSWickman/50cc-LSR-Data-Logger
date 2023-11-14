add_library(picowbell_sd_card picowbell_sd_card.c)

target_include_directories(picowbell_sd_card PUBLIC "${PROJECT_SOURCE_DIR}/include")

target_link_libraries(picowbell_sd_card
    pico_stdlib
    FatFs_SPI
)