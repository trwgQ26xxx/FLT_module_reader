#! /bin/bash

clear

avrdude -c usbasp -p m64 -U flash:v:"out/main.hex":a -B 3