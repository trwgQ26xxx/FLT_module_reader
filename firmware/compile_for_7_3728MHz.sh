#! /bin/bash

clear

echo "Cleaning out directory..."
rm -rf out
mkdir out

echo "Compiling..."
avr-gcc -Wall -c drv/hw_ext_mem.c 			-o out/hw_ext_mem.o 			-mmcu=atmega64a -D F_CPU=7372800UL -O2
avr-gcc -Wall -c drv/hw_uart.c 				-o out/hw_uart.o 				-mmcu=atmega64a -D F_CPU=7372800UL -O2
avr-gcc -Wall -c drv/hw_rom_switching.c 	-o out/hw_rom_switching.o 		-mmcu=atmega64a -D F_CPU=7372800UL -O2
avr-gcc -Wall -c drv/hw_leds.c 				-o out/hw_leds.o 				-mmcu=atmega64a -D F_CPU=7372800UL -O2
avr-gcc -Wall -c drv/hw_tmr.c 				-o out/hw_tmr.o 				-mmcu=atmega64a -D F_CPU=7372800UL -O2
avr-gcc -Wall -c serial_transmission.c 		-o out/serial_transmission.o 	-mmcu=atmega64a -D F_CPU=7372800UL -O2
avr-gcc -Wall -c main.c 					-o out/main.o 					-mmcu=atmega64a -D F_CPU=7372800UL -O2

avr-gcc -mmcu=atmega64a -o out/main.elf out/main.o out/hw_uart.o out/hw_ext_mem.o out/hw_rom_switching.o out/hw_leds.o out/hw_tmr.o out/serial_transmission.o

avr-size -C -x --mcu=atmega64a out/main.elf

echo "Writing output files..."
avr-objcopy out/main.elf -O ihex 	out/main.hex
avr-objcopy out/main.elf -O binary 	out/main.bin

echo "Done!"