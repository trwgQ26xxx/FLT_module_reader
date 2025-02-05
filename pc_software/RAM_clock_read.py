import serial
import sys
import time
import calendar

baudrate = 57600
ser_timeout	= 0.5

clock_req_period = 0.5	#2Hz

GET_RAM_CLOCK_CMD	= ord("g")
GET_RAM_CLOCK_CMD_RESPONSE_LEN = 12

M48T02_RAM_TYPE = ord("t")
M48T08_RAM_TYPE = ord("T")

SOF_CHAR = 0xFA
EOF_CHAR = 0xFB
ESC_CHAR = 0xFC
expected = b'\xFB'

MIN_RX_CMD_LEN	= 5

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
	if decoded_cmd[1] == 0x01:
		return True
	else:
		return False 

def is_data_valid(decoded_cmd):
	#Check if data valid flag is set and weekday is in range from 1 to 7
	#(range end is inclusive, so +1 is used)
	if decoded_cmd[2] == 0x01 and decoded_cmd[6] in range(1, 7+1):
		return True
	else:
		return False 

def print_time(cmd):
	if is_module_connected(cmd):
		if is_data_valid(cmd):
			print(style.GREEN + f"Time is {cmd[7]:02d}:{cmd[8]:02d}:{cmd[9]:02d} {cmd[5]:02d}-{cmd[4]:02d}-20{cmd[3]:02d} (" + calendar.day_name[cmd[6]-1] + ")")
		else:
			print(style.RED + "Data is invalid!");

	else:
		print(style.RED + "RAM adapter is NOT connected.");


#MAIN PROGRAM
try:
	print("------------------------------------------")
	print("FLT/ART/CU-1 ROM reader by trwgQ26xxx v1.0")
	print("------------------------------------------")

	if len(sys.argv) == 3:

		ram_type = sys.argv[1]
		serial_port = sys.argv[2]
		
		#Check RAM type
		if ram_type == "M48T02":
			ram_type_id = M48T02_RAM_TYPE
		elif ram_type == "M48T08":
			ram_type_id = M48T08_RAM_TYPE
		else:
			print(style.RESET + "Unsupported RAM type, can be either M48T02 or M48T08.")
			sys.exit()

		opened_ok = True

		try:
			ser = serial.Serial(serial_port, baudrate, timeout = ser_timeout, bytesize = serial.EIGHTBITS, parity = serial.PARITY_NONE, stopbits = serial.STOPBITS_ONE)

		except serial.SerialException:
			print("Serial port " + serial_port + " not found!")
			opened_ok = False

		if opened_ok == True:

			print(style.RESET + "Requesting time...")

			while True:
			
				try:
					encoded_ramtest_cmd = encode_cmd([GET_RAM_CLOCK_CMD, ram_type_id])
					ser.write(encoded_ramtest_cmd)
					ser.flush()

					rx_ramtest_cmd = ser.read_until(expected)
					decoded_ramtest_cmd = decode_cmd(rx_ramtest_cmd)
					ser.reset_input_buffer()
					
				except serial.SerialException:
					print(style.RESET + "Device disconnected or serial port error, terminated.")
					sys.exit()

				if len(decoded_ramtest_cmd) == GET_RAM_CLOCK_CMD_RESPONSE_LEN and decoded_ramtest_cmd[0] == GET_RAM_CLOCK_CMD:
					if check_crc(decoded_ramtest_cmd) == True:
						print_time(decoded_ramtest_cmd)
					else:
						print(style.RED + "Wrong response (CRC or length wrong)!")
				else:
					print(style.RED	+ "No response!")
					
				time.sleep(clock_req_period)
	else:
		print("Wrong usage! Two arguments (RAM type and serial port name) should be provided.")
		
except KeyboardInterrupt:
	if opened_ok == True:
		ser.close()

	print(style.RESET + "Terminated.")
