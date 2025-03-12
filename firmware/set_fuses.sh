#! /bin/bash

clear

avrdude -c usbasp -p m64 -U efuse:w:0xFE:m -U hfuse:w:0xC9:m -U lfuse:w:0x1F:m -B 3