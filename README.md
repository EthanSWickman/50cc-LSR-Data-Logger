# 50CC LSR Data Logger 

This repository is the home of data logger development for the 50cc Land Speed Record Motorcycle Racing Team at Oregon State University .

## Project Structure 
- /source/ - code base (we use C and CMake) - loosely follows https://cliutils.gitlab.io/modern-cmake/chapters/basics/structure.html plus the following
    - /source/build/ - cmake build file
    - /source/schematic/ - wiring schematics 
- /examples/ - example cmake projects that exhibit important features of cmake and pico programming in C
- /notes/ - important notes for the datalogging team to reference

## Dev/Compile Instructions (check out https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html)

### Dev Requirements 
- install CMake, C compiler 
- install C/C++ pico-sdk in the root directory of this project 
- set the PICO_SDK_PATH environment variable to the location of your pico-sdk directory
- create empty build directory in the source directory

### Compile Instructions 
- enter build directory and run cmake ./../
- run cmake --build ./ inside build directory (might take some configuration of your C compiler)
- put pico into flash mode and copy the compiled .uf2 file onto the pico

## Git Etiquette
- make a branch for new development, keep master clean and compileable
- make targeted commits with meaningful messages
