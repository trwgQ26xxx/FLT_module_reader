import secrets
import sys

DATA_SIZE = 0x7FF0 #32752b

COLUMNS = 16
ROWS = int(DATA_SIZE / COLUMNS)

if DATA_SIZE % COLUMNS != 0:
	print("Wrong DATA_SIZE to COLUMNS ratio!")
	sys.exit()

file = open("drv/ram_test_data.h", "w")

file.write("/*\n")
file.write("Project name:	FLT/ART/CU1 ROM Reader\n")
file.write("Author: 		trwgQ26xxx\n")
file.write("Date:			18.01.2025\n")
file.write("Target MCUs:	ATmega64/ATmega64L/ATmega64A/ATmega128/ATmega128L/ATmega128A\n")
file.write("Compiler:		AVR-GCC version 5.4.0 (32-bit)\n")
file.write("*/\n")
file.write("\n")
file.write("#ifndef _DRV_RAM_TEST_DATA_H\n")
file.write("#define _DRV_RAM_TEST_DATA_H\n")
file.write("\n")
file.write("#include <stdint.h>\n")
file.write("\n")
file.write("#include <avr/pgmspace.h>\n")
file.write("\n")
file.write("//Test data array has to be smaller than 32k due to avr-gcc limitation\n")
file.write("#define RAM_TEST_DATA_LENGTH\t0x" + f"{DATA_SIZE:X}\t//{int(DATA_SIZE)}b" + "\n")
file.write("\n")
file.write("const uint8_t ram_test_data[RAM_TEST_DATA_LENGTH] PROGMEM = {\n")

for I in range(ROWS):
	rand_bytes = secrets.token_bytes(COLUMNS)
	
	file.write("\t")
	
	for J in range(COLUMNS-1):
		file.write("0x" + '{:02X}'.format(rand_bytes[J]) + ", ")
	
	file.write("0x" + '{:02X}'.format(rand_bytes[COLUMNS-1]) + ",\n")


file.write("};\n")
file.write("\n")
file.write("#endif\n")
file.write("\n")

file.close()

print("Generated OK!")