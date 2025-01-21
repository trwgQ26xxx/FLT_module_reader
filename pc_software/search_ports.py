from serial.tools import list_ports

print("------------------------------------------")
print("FLT/ART/CU-1 ROM reader by trwgQ26xxx v1.0")
print("------------------------------------------")

print("Looking for serial ports...")

found_ports = list(list_ports.comports())

if not found_ports:
	print("No ports found!")
else:
	for p in found_ports:
		print("Found: " + p.device + "; description: " + p.description + "; HWID: " + p.hwid + ".")
		
print("Done!")
