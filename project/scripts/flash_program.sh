#!/bin/bash

# builds logger binaries, mounts pico, and flashes it 
# to use, make sure Pico is in bootsel mode and run at the build directory
# used as a make install tool to help Ethan compile

DEVICE=`dmesg | grep -E "sd[a-z]+1" | awk '{printf "%s", $0}' | awk '{print $NF}'`

DEVICE=/dev/"$DEVICE"

mkdir -p /tmp/pico/
mount $DEVICE /tmp/pico/

cp ./apps/runloop.uf2 /tmp/pico/

sync

umount /tmp/pico/