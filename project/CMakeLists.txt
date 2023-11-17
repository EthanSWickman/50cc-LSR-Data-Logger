cmake_minimum_required(VERSION 3.27.7)

include(pico_sdk_import.cmake)

project(LSRLogger 
    VERSION 0.01
    DESCRIPTION "50cc LSR Data Logger"
    LANGUAGES C CXX ASM
)

set(PICO_BOARD pico_w)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_subdirectory(src)
add_subdirectory(apps)
add_subdirectory(extern)

# ignore this line, helps Ethan compile code, if you want your own flash script, we can work on a cross-platform script!
install(CODE "execute_process(COMMAND bash ${PROJECT_SOURCE_DIR}/scripts/flash_program.sh)")