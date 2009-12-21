/*!\file  defs.h  global definitions
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


#ifndef __DEFS_H
#define __DEFS_H

#ifndef ARM_FLASH_VERSION_STR
	//! Current version of arm_flash.
	#define ARM_FLASH_VERSION_STR "0.8.3 (12.9.2009)"
#endif

//! Standard unix succes value, which is 0.
#define SUCCESS 0	 
//! Standard unix failure value, which is -1.
#define FAILURE -1	 
//! Error prefix used for printing failures or errors.
#define ERRSTR	"ERROR: "

#endif
