# 50CC LSR Data Logger 

This repository is the home of data logger development for the 50cc Land Speed Record Motorcycle Racing Team at Oregon State University .

## Project Structure 
- /project/ - code base (we use C and CMake) - loosely follows https://cliutils.gitlab.io/modern-cmake/chapters/basics/structure.html
    - /project/apps - files to be compiled into executables
    - /project/src - library implementation files
    - /project/include - library header files
    - /project/build - build directory, contains binaries
    - /project/scripts - build/deploy scripts
    - /project/docs - documentation
    - /project/docs/schematic/ - wiring schematics 
- /examples/ - example cmake projects that exhibit important features of cmake and pico programming in C
- /notes/ - important notes for the datalogging team to reference

## Git Etiquette
- make a branch for new development, keep master clean, compileable, and in sync with the latest wiring diagram
- make targeted commits with meaningful messages
