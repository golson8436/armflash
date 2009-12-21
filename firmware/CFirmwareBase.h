/*!\file  CFirmwareBase.h  Defines the base class for doing various actions with firmware files.
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


#ifndef __CFIRMWARE_BASE_H
#define __CFIRMWARE_BASE_H


/**
*\class CFirmwareBase
*\brief Base class for firmware files
* 
* It is used to derive from by the actuall firmware classes - for instance the 
* intel hex 32 firmware format etc.
* 
*\author Gabriel Zabusek
*/

template<typename T> class CFirmwareBase
{
	public:

		/**
		*\brief Opens the given firmware file.
		*
		* A pure virtual member which is used as an interface for opening firmware files.
		*
		*@param path_name Zero terminated string containing the path to the firmware file.
		*@return true on success false otherwise.
		*/
		virtual bool OpenFirmware(const char * path_name) = 0;

		/**
		*\brief Checks integrity of the firmware.
		*
		* A pure virtual member which is used as an interface for integrity checking
		* of the firmware file. Obviously this method only makes sense if the actual
		* firmware file supports checksums.
		*
		*@return true on success false otherwise.
		*@see ChecksumSupported()
		*/		
		virtual bool CheckFirmware(bool bVerbose=false) = 0;

		/**
		*\brief Gets the data which are supposed to be written to address adr.
		*
		* A pure virtual member which is used as an interface for getting the next 
		* address to write to and the data to be written to this address. Both
		* parameters are output parameters. The actuall address/data are returned
		* according to the current position in the processed file.
		*
		*@param adr The next address to write to.
		*@param data The data to be written to adr parameter.
		*@param cData The size of the data array. This is also used to return the number of valid data bytes in data.
		*@return true on success false otherwise.
		*/
		virtual bool GetNextAdrData(T & adr, T * data, uint32_t & cData, bool bVerbose=false) = 0;

		/**
		*\brief Checks whether the firmware file supports checksums.
		*@return true on success false otherwise.
		*/ 
		virtual bool ChecksumSupported() = 0;

		//! Destructor, does nothing.
		virtual ~CFirmwareBase(){}
};

#endif
