import serial
import libscrc
from datetime import datetime
import sys

baudrate = 57600
ser_timeout	= 0.5

CONTROL_RAM_CLOCK_CMD	= ord("c")
SET_RAM_CLOCK_CMD		= ord("s")
CMD_RESPONSE_LEN = 5

START_CLOCK_OSCILLATOR	= ord("e")
STOP_CLOCK_OSCILLATOR	= ord("d")
RESET_CLOCK				= ord("r")

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

def encode_cmd(cmd):
	det_cmd = bytearray(cmd)

	crc16 = libscrc.xmodem(det_cmd)
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
		
		if libscrc.xmodem(cmd[:-2]) == crc_received:
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

def is_cmd_executed_OK(decoded_cmd):
	if decoded_cmd[2] == 0x01:
		return True
	else:
		return False

def print_command_result(cmd):
	if is_module_connected(cmd):
		print(style.GREEN + "RAM adaper is connected, ", end="")
					
		if is_cmd_executed_OK(cmd):
			print(style.GREEN + "RAM command executed OK!")
		else:
			print(style.RED + "RAM command execute failed.")
	else:
		print(style.RED + "RAM adapter is NOT connected.");


#MAIN PROGRAM
try:
	if len(sys.argv) == 4:

		ram_type = sys.argv[1]
		ram_cmd = sys.argv[2]
		serial_port = sys.argv[3]
		
		#Check RAM type
		if ram_type == "M48T02":
			ram_type_id = M48T02_RAM_TYPE
		elif ram_type == "M48T08":
			ram_type_id = M48T08_RAM_TYPE
		else:
			print(style.RESET + "Unsupported RAM type, can be either M48T02 or M48T08.")
			sys.exit()
		
		#Check RAM command
		if ram_cmd == "reset":
			general_command = CONTROL_RAM_CLOCK_CMD
			clock_command = RESET_CLOCK
		elif ram_cmd == "start":
			general_command = CONTROL_RAM_CLOCK_CMD
			clock_command = START_CLOCK_OSCILLATOR
		elif ram_cmd == "stop":
			general_command = CONTROL_RAM_CLOCK_CMD
			clock_command = STOP_CLOCK_OSCILLATOR
		elif ram_cmd == "set":
			general_command = SET_RAM_CLOCK_CMD
		else:
			print(style.RESET + "Unsupported command, can be: set, reset, start or stop.")
			sys.exit()

		opened_ok = True

		try:
			ser = serial.Serial(serial_port, baudrate, timeout = ser_timeout, bytesize = serial.EIGHTBITS, parity = serial.PARITY_NONE, stopbits = serial.STOPBITS_ONE)

		except serial.SerialException:
			print("Serial port " + serial_port + " not found!")
			opened_ok = False

		if opened_ok == True:

			print(style.RESET + "Executing command...")
			
			#Compose the appropriate command
			if general_command == SET_RAM_CLOCK_CMD:
				curr_time = datetime.now()
			
				tx_cmd = [SET_RAM_CLOCK_CMD, ram_type_id, int(curr_time.year - 2000), curr_time.month, curr_time.day, curr_time.weekday()+1, curr_time.hour, curr_time.minute, curr_time.second]
			else:
				tx_cmd = [CONTROL_RAM_CLOCK_CMD, ram_type_id, clock_command]
				
			try:
				encoded_ramtest_cmd = encode_cmd(tx_cmd)
				ser.write(encoded_ramtest_cmd)
				ser.flush()

				rx_ramtest_cmd = ser.read_until(expected)
				decoded_ramtest_cmd = decode_cmd(rx_ramtest_cmd)
				ser.reset_input_buffer()

			except serial.SerialException:
				print(style.RESET + "Device disconnected or serial port error, terminated.")
				sys.exit()

			if len(decoded_ramtest_cmd) == CMD_RESPONSE_LEN and decoded_ramtest_cmd[0] == general_command:
				if check_crc(decoded_ramtest_cmd) == True:
					print_command_result(decoded_ramtest_cmd)
				else:
					print(style.RED + "Wrong response (CRC or length wrong)!")
			else:
				print(style.RED	+ "No response!")

			ser.close()
			
			print(style.RESET + "Done.")
		
	else:
		print("Wrong usage! Three arguments (RAM type, command and serial port name) should be provided.")
		
except KeyboardInterrupt:
	if opened_ok == True:
		ser.close()

	print(style.RESET + "Terminated.")
