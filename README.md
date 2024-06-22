# Fiat Lancia Tester \ Alfa Romeo Tester \ Check Up 1 module reader

Dedicated module reader for FLT\ART\CU1 ROMs archiving.

![Top](PCB/TOP.jpg)
![Bottom](PCB/BOTTOM.jpg)

# Directories organization

- **firmware** - MCU firmware, written in C.
- **pc_software** - PC software, written in Python.
- **PCB** - PCB project, prepared in KiCad version 8.0.1.
- **DS** - data sheets of used components

# Usage

Under linux:

```
reader /dev/ttyUSB0
```

Under Windows:

```
reader COM1
```

# Basic instructions

1. Connect the reader to the PC. Running the program on a PC without a module connected is signaled with an error.

![NO_MODULE](pc_software/instructions/NO_MODULE.png)

2. If a module is connected, the reader detects which ROMs are present in connected module. In this example we have a module with all three (**ROM A, B and C**). If at least one ROM is installed in the module (and this will always be the case, because B must be present), the program will ask you to enter a prefix for the files that will be stored on the PC. I entered here, for example, *M99A_IT*. After confirming with enter, the ROM data downloading process begins:

![MODULE_ABC](pc_software/instructions/MODULE_ABC.png)

3. Data integrity is checked using CRC on an ongoing basis, while downloading individual blocks (one block is 512 bytes). In any case, **it may be advisable to repeat the dumping operation twice, disconnecting and reconnecting the ROM module to wipe the contacts a bit**. Finally, individual files with the contents of the ROMs are stored on the PC:

![STORED_FILES](pc_software/instructions/STORED_FILES.png)

The process is similar for other modules, here is an example if the module contained only **ROM A and B**:

![MODULE_AB](pc_software/instructions/MODULE_AB.png)

or **ROM B** only:

![MODULE_ABC](pc_software/instructions/MODULE_B.png)

The program reads the contents of only the ROMs present in the module.

# Notes

Atmega64A fusebits should be configured as follows:

| Extended | High | Low |
| - | - | - |
| 0xFF | 0xC9 | 0x1F |