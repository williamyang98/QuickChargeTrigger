#!/bin/sh
./avrdude.exe -C ./avrdude.conf -c usbasp -p t85 -B 16 -U flash:w:"./QuickChargeTrigger.hex":i -P usb
