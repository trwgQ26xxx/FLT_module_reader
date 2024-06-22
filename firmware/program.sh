#! /bin/bash

clear

avrdude -c usbasp -p m64 -U flash:w:"out/main.hex":a -B 3