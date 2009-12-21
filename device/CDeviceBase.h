/*!\file  CDeviceBase.h  Implementation of base class for various devices/chips.
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


#ifndef __CDEVICEBASE_H
#define __CDEVICEBASE_H

#include <string>
#include <vector>
#include <map>
#include <core/serial.h>
#include <device/CFlashingStatus.h>

using namespace std;

enum DeviceConnectionType {
	DEVICE_CONN_TYPE_SERIAL,
	DEVICE_CONN_TYPE_UNSPECIFIED
};

#define STR_DEVICE_ERROR "DEVICE ERROR: "

/**
*\class CDeviceBase
*\brief Base class for the actual device implementations.
*
* Base class for the actual devices. It is designed so that it will be easy to support new
* devices. Its interface should be everything that all the devices have in common. Always
* inherit this class if you want to implement new device since we want to support programing
* of various devices in parallel. 
*
*\author Gabriel Zabusek
*/

class CDeviceBase {
	protected:
		//! Size of available random access memory on the device.
		unsigned int m_unRamSize;
		//! Size of available read only memory on the device.
		unsigned int m_unRomSize;
		//! Speed of the connected crystal in Hz.
		unsigned int m_unCrystalHz;
		//! Flag whether device was initialized
		bool m_bInitialized;
		//! The device name of PC port where the board is connected.
		string m_strConnDevice;
		//! The path to the firmware stored for the purpose of threading
		string m_strFirmwarePath;
		//! The device type of PC port where the board is connected.
		DeviceConnectionType m_DeviceType;
		//! Serial port support object
		CSerial *m_pclSerialPort;
		//! This map should explain all the error codes for the device: for instance m_mapErrorCodes[BAD_ADDR] == "The device received an invalid address"
		map<int,string> m_mapErrorCodes;
        //! This class holds all status information about flashing.
        CFlashingStatus *m_pclFlashingStatus;

		/**
		*\brief Sets the random access memory size available for the device.
		*@param size The size of RAM in Bytes.
		*/
		void SetRamSize( unsigned int size );
		
		/**
		*\brief Sets the read only memory size available for the device.
		*@param size The size in Bytes.
		*/
		void SetRomSize( unsigned int size );

		/**
		*\brief Sets the device to which the board with mcu is connected - ie /dev/ttyS0
		*@param strDevName The name of the device in the system
		*/
		void SetConnDeviceName(string strDevName);

		/**
		*\brief Sets the device type to which the board with mcu is connected - ie serial,usb,...
		*@param strDevName The name of the device in the system
		*/
		void SetConnDeviceType(DeviceConnectionType type);

		/**
		*\brief Gets the device name to which the board with mcu is connected.
		*@return The device name.
		*/
		string GetConnDeviceName() const;

		/**
		*\brief Gets the type of the device to which is the mcu connected.
		*@return The device type - ie. serial,usb,...
		*/
		DeviceConnectionType GetConnDeviceType() const;

		/**
		*\brief Start the flashing of the device in a separate thread.
		*
		* Since this will be used as thread function for the create_thread() pthread library function
		* it has got to be static member. That is why I had to do this nasty looking workaround.
		*
		*/
		static void StartFlashThread(void *obj)
		{
			reinterpret_cast<CDeviceBase *>(obj)->FlashDevice( reinterpret_cast<CDeviceBase *>(obj)->GetFirmwarePath() );
		}

		virtual int SendCommand(string strCmd, string strExpRep, unsigned int nTimeoutSec) = 0;


	public:
		CDeviceBase();

		/**
		*\brief Gets the random access memory size available for the device.
		*@return The size in Bytes.
		*/
		unsigned int GetRamSize() const;

		/**
		*\brief Gets the read only memory size available for the device.
		*@return The size in Bytes.
		*/
		unsigned int GetRomSize() const;

		/**
		*\brief Gets the speed of the connected crystal in Hz.
		*@return The speed in Hz.
		*/
		unsigned int GetCrystalSpeedHz() const;

		/**
		*\brief Gets the path to the firmware stored as a member.
		*@return The path to the firmware.
		*/
		string GetFirmwarePath() const;

		/**
		*\brief Sets the speed of the connected crystal in Hz.
		*@param speed_hz The speed in Hz.
		*/
		void SetCrystalSpeedHz( unsigned int speed_hz );

		/**
		*\brief Sets the path to the firmware.
		*/
		void SetFirmwarePath(string strFirmwarePath);

		/**
		*\brief Initializes the device.
		*
		* Pure virtual member. Adds possiblility to initialize the device for flashing.
		* This can really mean anything and is dependent on the actual device.
		*
		*@return Should return true if everything went OK, false otherwise.
		*/
		virtual bool InitializeDevice() = 0;

		/**
		*\brief Initializes the device.
		*
		* Pure virtual member. Adds possiblility to initialize the device for flashing.
		* This can really mean anything and is dependent on the actual device. 
		*
		*@param The name of the device to which the mcu board is connected
		*@return Should return true if everything went OK, false otherwise.
		*/
		virtual bool InitializeDevice(string strDevName) = 0;

		/**
		*\brief Gets the info about the device stored in it.
		*
		* Pure virtual member. Adds possiblility to get information stored directly on 
		* the device - if the device supports such thing.
		*
		*@return Should return vector of strings where each member represents a line stored on the device. Empty vector if no such thing is supported.
		*/
		virtual vector<string> GetDeviceInfo() = 0;

		/**
		*\brief Does the actual flashing of the device.
		*@param strFirmwarePath The path to the firmware file.
		*@return true if everything went OK, false otherwise.
		*/
		virtual bool FlashDevice(string strFirmwarePath) = 0;

		//! Virtual destructor, does nothing in our case.
		virtual ~CDeviceBase() {}
};

#endif
