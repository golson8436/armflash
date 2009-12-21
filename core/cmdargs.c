/*!\file  serial.h  Argument and help handling
 *
 *	This file is part of the armflash (arm flashing utility)
 *  package.
 *
 *  Copyright (c) 2008-2009 by Gabriel Zabusek <gabriel.zabusek@gmail.com>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */


#include <stdio.h>
#include "cmdargs.h"

const char * pszArmFlashSo = "hvdb:";

const struct option rgstArmFlashLo[] = {
	{ "help",         no_argument, 	     NULL, 'h'},
	{ "version",      no_argument, 	     NULL, 'v'},
	{ "detect_rs232", no_argument, 	     NULL, 'd'},
	{ "dump_binary",  required_argument, NULL, 'b'},
	{ NULL, 0, NULL, 0 } //this is required in the end of the struct
};

void 
print_help(const char * pszPrgName)
{

	printf("\n");
	printf("\t**************************************************\n");
	printf("\t*                                                *\n");
	printf("\t*  arm_flash - the Arm (micro)processor flasher  *\n");
	printf("\t*                                                *\n");
	printf("\t*                                                *\n");
	printf("\t*             Author: Gabriel Zabusek            *\n");
	printf("\t*                                                *\n");
	printf("\t*              Copyright 2008 - 2009             *\n");
	printf("\t**************************************************\n");
	printf("\n\n");
	printf("Usage:\n");
	printf("\t%s [SEQ1 SEQ2 ...] OPTIONS\n", pszPrgName);
	printf("Where: \n\n");
	printf("SEQn:\n");
	printf("\tIs in format PORT FIRMWARE BAUDRATE CRYSTAL_HZ DEVICE\n");
	printf("OPTIONS:\n");
	printf("\t--help (-h)\n\t  prints this help\n");
	printf("\t--version (-v)\n\t  displays your version of %s\n", pszPrgName);
	printf("\t--detect_rs232 (-d)\n\t  autodetects your serial port devices and lists them. USE THIS OPTION ALONE\n");
	printf("\t--dump_binary BINFILE (-b BINFILE)\n\t  dumps raw BINFILE data with memory adresses\n");
	printf("PORT:\n");
	printf("\tSome serial port used to program the device. Use -d to detect available ports\n");
	printf("FIRMWARE:\n");
	printf("\tThe firmware to program.\n");
	printf("\tSupported formats:\n");
	printf("\t\t - Intel hex 32-bit\n");
	printf("BAUDRATE:\n");
	printf("\tBaudrate used to program the device\n");
    printf("CRYSTAL_HZ:\n");
    printf("\tCrystal speed in Hz\n");
	printf("DEVICE:\n");
	printf("\tThe processor which we are programming.\n");
	printf("\tCurrently supported (micro)processors:\n");
	printf("\t\t- LPC2103\t(Phillips)\n");
	printf("EXAMPLE:\n");
	printf(":: programmes devices connected to ttyS0 and ttyUSB0 with selected firmwares at the same time\n");
	printf("\t%s /dev/ttyS0 firmware1.hex 38400 10000 LPC2103 /dev/ttyUSB0 firmware2.hex 38400 10000 LPC2103\n\n", pszPrgName);
	printf(":: detects available serial ports on the system and lists them\n");
	printf("\t%s -d, %s --detect_rs232\n\n", pszPrgName, pszPrgName);
}
