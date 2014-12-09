#!/bin/bash
avrdude -v -P /dev/ttyUSB0 -c arduino -y -p atmega328p -b57600 -U eeprom:r:..\out\main.eep.raw:r
