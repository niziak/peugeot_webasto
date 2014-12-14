#!/bin/bash
avrdude -v -P /dev/ttyUSB0 -c arduino -y -p atmega328p -b57600 -U flash:r:../out/main.flash.raw:r
