/*!\file  serial.h  Argument handling
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


#ifndef __CMDARGS_H
#define __CMDARGS_H

#include <getopt.h>

//! constant for help argument
#define OPT_HELP 'h'
//! constant for version argument
#define OPT_VER	 'v'
//! constant for serial autodetection argument
#define OPT_DETECT_RS232 'd'
//! constant for raw assembler dump argument
#define OPT_RAWDUMP 'b'

//! long options definitions
extern const struct option rgstArmFlashLo[];

//! short options definitions
extern const char * pszArmFlashSo;

//! prints the arm_flash help
extern void print_help(const char * pszPrgName);

#endif
