#!/bin/bash
avrdude -v -P /dev/ttyUSB0 -c arduino -y -p atmega328p -b 115200 -U eeprom:r:../out/main.eep.raw:r
