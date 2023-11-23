add_library(writebuffer writebuffer.c)

target_include_directories(writebuffer PUBLIC "${PROJECT_SOURCE_DIR}/include")

target_link_libraries(writebuffer
    pico_stdlib
    pico_multicore
)