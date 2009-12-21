/*!\file  CDeviceBase.cxx  Base class for device classes
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

#include <device/CDeviceBase.h>

CDeviceBase::CDeviceBase()
{
	m_DeviceType = DEVICE_CONN_TYPE_UNSPECIFIED;
}

void 
CDeviceBase::SetRamSize(unsigned int size)
{
	m_unRamSize = size;
}

void 
CDeviceBase::SetRomSize(unsigned int size)
{
	m_unRomSize = size;
}

unsigned int
CDeviceBase::GetRamSize() const
{
	return m_unRamSize;
}

unsigned int
CDeviceBase::GetRomSize() const
{
	return m_unRomSize;
}

unsigned int 
CDeviceBase::GetCrystalSpeedHz() const
{
	return m_unCrystalHz;
}

string
CDeviceBase::GetConnDeviceName() const
{
	return m_strConnDevice;
}

DeviceConnectionType 
CDeviceBase::GetConnDeviceType() const
{
	return m_DeviceType;
}

string 
CDeviceBase::GetFirmwarePath() const
{
	return m_strFirmwarePath;
}

void
CDeviceBase::SetCrystalSpeedHz( unsigned int speed_hz )
{
	m_unCrystalHz = speed_hz;
}

void
CDeviceBase::SetConnDeviceName(string strDevName)
{
	m_strConnDevice = strDevName;
}

void 
CDeviceBase::SetConnDeviceType(DeviceConnectionType type)
{
	m_DeviceType = type;
}

void 
CDeviceBase::SetFirmwarePath(string strFirmwarePath)
{
	m_strFirmwarePath = strFirmwarePath;
}

