/*!\file  CFirmwareHEX32.h  Implements class to work with Intel HEX 32 firmware files.
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

#ifndef __CFIRMWARE_HEX32_H
#define __CFIRMWARE_HEX32_H


#ifdef __linux__
#include <linux/types.h>
#endif

#ifdef __FreeBSD__
#include <sys/types.>
#endif


#include <fstream>
#include <iostream>
#include "CFirmwareBase.h"

//! TODO there is a problem compiling arm_flash in Fedora 9 with including <linux/types.h> due to multiple uint32_t definitions.... :(((. Thats the reason for this definition.
//typedef unsigned int uint32_t;

using namespace std;

#define RECTYP_DATAREC 		0x00
#define RECTYP_ENDREC  		0x01
#define RECTYP_EXTENDED_SEG_AR  0x02
#define RECTYP_START_SEG_AR 	0x03
#define RECTYP_EXTENDED_LIN_AR  0x04
#define RECTYP_START_LIN_AR  	0x05

#define HEX32_DATA_MAXLEN	0xFF

/**
*\class CFirmwareHEX32
*\brief Implements interface to work with Intel HEX32 firmware files.
*\author Gabriel Zabusek
*/

class CFirmwareHEX32 : CFirmwareBase<uint32_t>
{
	private:
		//! Firmware file handler TODO: should this be part of base class??
		ifstream m_clInputFile;
		//! Used for checking whether file has been open already... there is probably better way to do this by directly checking it using ifstream
		bool	 m_bFileOpen;
		//! Path to the last open file name
		string	 m_strLastFileName;
		//! EIP address - the entry point of the firmware
		uint32_t m_nEIP;
		//! ULBA address - used for 32-bit adressing
		uint32_t m_nULBA;

	public:
		//! Constructor, currently only initializes the private members.
		CFirmwareHEX32();

		/**
		*\brief Opens the firmware at given path.
		*
		* Opens the firmware at given path and sets the private members appropriately.
		*
		*@param pszPathName Zero terminated string containing the path to the firmware
		*@return Method returns true on success false otherwise.
		*/
		virtual bool OpenFirmware(const char * pszPathName);

		/**
		*\brief Checks the integrity of the firmware file.
		*
		* Checks the integrity of the Intel HEX32 firmware file using checksums.
		*
		*@return Method returns true on success false otherwise.
		*/
		virtual bool CheckFirmware(bool bVerbose=false);

		/**
		*\brief Gets next address to write to and the data to write to that address.
		*
		* Gets next address to write to and the data to write to that address in the
		* file being currently processed. This method is therefore suitable for use
		* in a while loop for instance.
		*
		*@return Method returns true on success false otherwise.
		*/
		virtual bool GetNextAdrData(uint32_t & u32Adr, uint32_t * pu32Data, uint32_t & cData, bool bVerbose = false);
		
		/**
		*\brief This is Intel HEX32 firmware file implementation so this method always returns true.
		*@return Always true.
		*/
		virtual bool ChecksumSupported();

		//! Destructor, does nothing at the moment.
		~CFirmwareHEX32(){}
};

#endif
