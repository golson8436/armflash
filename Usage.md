# Using armflash #


**EXAMPLE:**

Programmes devices connected to ttyS0 and ttyUSB0 with selected firmwares at the same time
  * armflash /dev/ttyS0 firmware1.hex 38400 10000 LPC2103 /dev/ttyUSB0 firmware2.hex 38400 10000 LPC2103

Programmes device LCP2103 connected to ttyS0 having 10000Hz crystal at 9600 baud with firmware firmware1.hex
  * armflash /dev/ttyS0 firmware1.hex 9600 10000 LPC2103

Detects available serial ports on the system and lists them
  * armflash -d, armflash --detect\_rs232


**Usage:**
> armflash SEQ1 SEQ2 ... OPTIONS
**Where:**

**SEQn:**
> Is in format PORT FIRMWARE BAUDRATE CRYSTAL\_HZ DEVICE
**OPTIONS:**
> --help (-h)
> > prints this help

> --version (-v)
> > displays your version of armflash

> --detect\_rs232 (-d)
> > autodetects your serial port devices and lists them. USE THIS OPTION ALONE

> --dump\_binary BINFILE (-b BINFILE)
> > dumps raw BINFILE data with memory adresses
**PORT:**

> Some serial port used to program the device. Use -d to detect available ports
**FIRMWARE:**
> The firmware to program.
> Supported formats:
> > - Intel hex 32-bit
**BAUDRATE:**

> Baudrate used to program the device
**CRYSTAL\_HZ:**
> Crystal speed in Hz
**DEVICE:**
> The processor which we are programming.
> Currently supported (micro)processors:
> > - LPC2103	(Phillips)