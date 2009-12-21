/*!\file  CDeviceLPC2103.h  LPC2103 Class
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

#ifndef __CDEVICELPC2103_H
#define __CDEVICELPC2103_H

#include <device/CDeviceBase.h>

/**
*\class CDeviceLPC2103
*\brief Class which provides flashing capabilities for LPC2103 based devices.
*
* This class inherits the CDeviceBase class and implements inteface for programming/flashing
* the LPC2103 based devices... at the moment this is being tested on arm7tdmi-s chips, for instance
* LPC2101/2/3 from Phillips semiconductors.
*
*/


// ISP Return codes definitions:
#define CMD_SUCCESS 				0
#define INVALID_COMMAND 			1
#define SRC_ADDR_ERROR 				2
#define DST_ADDR_ERROR 				3
#define SRC_ADDR_NOT_MAPPED 			4
#define DST_ADDR_NOT_MAPPED 			5
#define COUNT_ERROR 				6
#define INVALID_SECTOR 				7
#define SECTOR_NOT_BLANK 			8
#define SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION 9
#define COMPARE_ERROR 				10 
#define BUSY 					11
#define PARAM_ERROR 				12
#define ADDR_ERROR 				13
#define ADDR_NOT_MAPPED 			14
#define CMD_LOCKED 				15
#define INVALID_CODE 				16
#define INVALID_BAUD_RATE 			17
#define INVALID_STOP_BIT 			18
#define CODE_READ_PROTECTION_ENABLED 		19


class CDeviceLPC2103 : public CDeviceBase {
protected:
	int SendCommand(string strCmd, string strExpRep, unsigned int nTimeoutSec);
public:
	CDeviceLPC2103();
	CDeviceLPC2103(string strDevName, unsigned int unCrystalHz, speed_t stBaudRate );
	~CDeviceLPC2103();
	bool InitializeDevice();
	bool InitializeDevice(string strDevName);
	vector<string> GetDeviceInfo();
	bool FlashDevice(string strFirmwarePath);
    string GetConnDeviceName() const;
};

#endif




