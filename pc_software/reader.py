import serial
import sys
import time

baudrate = 57600
ser_timeout	= 2

DET_CMD	= ord("t")
DET_CMD_RESPONSE_LEN = 7

READ_CMD = ord("d")
ROM_A_SEL =	ord("A")
ROM_B_SEL =	ord("B")
ROM_C_SEL =	ord("C")
READ_CMD_RESPONSE_LEN =	516

FULL_ROM_SIZE =	65536
ROM_A_SIZE = 8192
ROM_B_SIZE = 65536
ROM_C_SIZE = 65536
SINGLE_BLOCK_SIZE =	512

SOF_CHAR = 0xFA
EOF_CHAR = 0xFB
ESC_CHAR = 0xFC
expected = b'\xFB'

MIN_RX_CMD_LEN	= 7

# Class	of different styles
class style():
	BLACK = '\033[30m'
	RED = '\033[31m'
	GREEN = '\033[32m'
	YELLOW	= '\033[33m'
	BLUE =	'\033[34m'
	MAGENTA = '\033[35m'
	CYAN =	'\033[36m'
	WHITE = '\033[37m'
	UNDERLINE = '\033[4m'
	RESET = '\033[0m'

def xmodem_crc(data):
    crc = 0x0000
    for byte in data:
        crc ^= (byte << 8)
        for _ in range(8):
            if (crc & 0x8000) == 0x8000:
                crc = (crc << 1) ^ 0x1021
            else:
                crc <<= 1
        crc &= 0xFFFF
    return crc

def encode_cmd(cmd):
	det_cmd = bytearray(cmd)

	crc16 = xmodem_crc(det_cmd)
	crc_h = (crc16 >> 8) & 0xFF
	crc_l = (crc16 >> 0) & 0xFF

	det_cmd.extend([crc_h, crc_l])

	encoded_cmd = bytearray([SOF_CHAR])

	for byte in det_cmd:

		if byte == SOF_CHAR or byte == EOF_CHAR or byte == ESC_CHAR:
			encoded_cmd.extend([ESC_CHAR, (~byte) & 0xFF])
		else:
			encoded_cmd.extend([byte])

	encoded_cmd.extend([EOF_CHAR])

	return encoded_cmd

def decode_cmd(cmd):
	decoded_cmd = bytearray()
	esc_char_received = False

	for byte in cmd:
		if byte == SOF_CHAR:
			decoded_cmd.clear()
		elif byte == EOF_CHAR:
			return decoded_cmd
		elif byte == ESC_CHAR:
			esc_char_received = True
		else:
			if esc_char_received	== True:
				decoded_cmd.extend([(~byte)	& 0xFF])
				esc_char_received =	False
			else:
				decoded_cmd.extend([byte])
		
	return decoded_cmd

def check_crc(cmd):
	cmd_len = len(cmd)

	if cmd_len >= MIN_RX_CMD_LEN:
		crc_received = ((cmd[cmd_len - 2] << 8) | cmd[cmd_len - 1]) & 0xFFFF
		
		if xmodem_crc(cmd[:-2]) == crc_received:
			return True
		else:
			return False
	
	else:
		return False

def is_module_connected(decoded_cmd):
	if decoded_cmd[1] != 0x00:
		return True
	else:
		return False 

def is_ROMA_present(decoded_cmd):
	if decoded_cmd[2] != 0x00:
		return True
	else:
		return False
		
def is_ROMB_present(decoded_cmd):
	if decoded_cmd[3] != 0x00:
		return True
	else:
		return False
		
def is_ROMC_present(decoded_cmd):
	if decoded_cmd[4] != 0x00:
		return True
	else:
		return False 

def print_module_info(cmd):
	if is_module_connected(cmd):
		print(style.GREEN + "ROM module is connected.");
		
		if is_ROMA_present(cmd):
			print(style.GREEN + "ROM A is present, ", end = '')
		else:
			print(style.RED + "ROM A is NOT present or EMPTY, ", end = '')
		
		if is_ROMB_present(cmd):
			print(style.GREEN + "ROM B is present, ", end = '')
		else:
			print(style.RED + "ROM B is NOT present or EMPTY, ", end = '')
			
		if is_ROMC_present(cmd):
			print(style.GREEN + "ROM C is present.")
		else:
			print(style.RED + "ROM C is NOT present or EMPTY.")
	else:
		print(style.RED + "ROM module is NOT connected.");


def read_ROM(min_block, max_block, rom_sel):
	reading_OK = True
	ROM_content = bytearray()

	print(style.RESET + "Reading block:", end = " ")

	for block in range(min_block, max_block):
		
		print(block, end = " ", flush=True)

		encoded_read_cmd = encode_cmd([READ_CMD, rom_sel, block])
		ser.write(encoded_read_cmd)
		ser.flush()
		
		rx_read_cmd =	ser.read_until(expected)
		decoded_read_cmd = decode_cmd(rx_read_cmd)
		ser.reset_input_buffer()
		
		#if lenght is	OK,	cmd	is OK, and data	is valid
		if len(decoded_read_cmd) == READ_CMD_RESPONSE_LEN and decoded_read_cmd[0] == READ_CMD and decoded_read_cmd[1] != 0x00:
			if check_crc(decoded_read_cmd) == True:
				ROM_content.extend(decoded_read_cmd[2:514])
			else:
				print(style.RED + "Wrong CRC!", end = " ")
				reading_OK = False
				break
		else:
			print(style.RED + "Wrong response!", end = " ")
			reading_OK = False
			break
			
	return [reading_OK, ROM_content]

def write_file(file_name_preffix, rom_name,	rom_content):
	f = open(file_name_preffix + "_" + rom_name + ".bin", "wb")
	f.write(rom_content)
	f.close()


#MAIN PROGRAM
try:
	print("------------------------------------------")
	print("FLT/ART/CU-1 ROM reader by trwgQ26xxx v1.0")
	print("------------------------------------------")

	if len(sys.argv) == 2:

		serial_port = sys.argv[1]

		tic = time.time()

		opened_ok = True

		try:
			ser = serial.Serial(serial_port, baudrate, timeout = ser_timeout, bytesize = serial.EIGHTBITS, parity = serial.PARITY_NONE, stopbits = serial.STOPBITS_ONE)

		except serial.SerialException:
			print("Serial port " + serial_port + " not found!")
			opened_ok = False

		if opened_ok == True:

			print(style.RESET + "Detecting module...")

			encoded_det_cmd = encode_cmd([DET_CMD])
			ser.write(encoded_det_cmd)
			ser.flush()

			rx_det_cmd = ser.read_until(expected)
			decoded_det_cmd = decode_cmd(rx_det_cmd)
			ser.reset_input_buffer()

			if len(decoded_det_cmd) == DET_CMD_RESPONSE_LEN and decoded_det_cmd[0] == DET_CMD:
				if check_crc(decoded_det_cmd) == True:
					print_module_info(decoded_det_cmd)
					
					#Check	if module is connected and at least	one	ROM	is present
					if is_module_connected(decoded_det_cmd) and (is_ROMA_present(decoded_det_cmd) or is_ROMB_present(decoded_det_cmd) or is_ROMC_present(decoded_det_cmd)):

						try:
							file_name_preffix = input(style.RESET + "Enter file name preffix, or Ctrl+C to exit:")
							
							tic = time.time()

							print(style.RESET + "Reading module...")
							
							#Read ROMA if it is present
							if is_ROMA_present(decoded_det_cmd):
								print(style.GREEN + "Reading ROM A")

								[reading_OK, rom_content] = read_ROM(0, int(ROM_A_SIZE/SINGLE_BLOCK_SIZE), ROM_A_SEL)
								print()
								
								if reading_OK:
									#Add padding to fill 64k
									rom_content.extend(bytearray(b'\xFF') * int(FULL_ROM_SIZE - ROM_A_SIZE))
									
									write_file(file_name_preffix, "ROMA", rom_content)
									
									print(style.GREEN + "OK")
								else:
									print(style.RED + "Fail!")
									
							if is_ROMB_present(decoded_det_cmd):
								print(style.GREEN + "Reading ROM B")

								[reading_OK, rom_content] =	read_ROM(0, int(ROM_B_SIZE/SINGLE_BLOCK_SIZE), ROM_B_SEL)
								print()
								
								if reading_OK:
									write_file(file_name_preffix, "ROMB", rom_content)
									#Debug
									write_file(file_name_preffix, "ROMB_L", rom_content[0:int(ROM_B_SIZE/2)])
									write_file(file_name_preffix, "ROMB_H", rom_content[int(ROM_B_SIZE/2):ROM_B_SIZE])
									
									print(style.GREEN + "OK")
								else:
									print(style.RED + "Fail!")
									
							if is_ROMC_present(decoded_det_cmd):
								print(style.GREEN + "Reading ROM C")

								[reading_OK, rom_content] = read_ROM(0, int(ROM_C_SIZE/SINGLE_BLOCK_SIZE), ROM_C_SEL)
								print()
								
								if reading_OK:
									write_file(file_name_preffix, "ROMC", rom_content)
									#Debug
									write_file(file_name_preffix, "ROMC_L", rom_content[0:int(ROM_C_SIZE/2)])
									write_file(file_name_preffix, "ROMC_H", rom_content[int(ROM_C_SIZE/2):ROM_C_SIZE])
									
									print(style.GREEN + "OK")
								else:
									print(style.RED + "Fail!")
							
						except KeyboardInterrupt:
							print(style.RESET)
							print("Terminated.")
				else:
					print(style.RED + "Wrong response!")
			else:
				print(style.RED	+ "No response!")

			toc = time.time()

			print(style.RESET + "Elapsed	time {:.1f}s. Done!".format(toc-tic))

			ser.close()
		
	else:
		print("Wrong usage! Only one argument	(serial	port name) should be provided.")
		
except KeyboardInterrupt:
	print(style.RESET)
	print("Terminated.")
