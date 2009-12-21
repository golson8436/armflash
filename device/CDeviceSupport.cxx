/*!\file  CDeviceSupport.cxx	Supported devices handling
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

#include <device/CDeviceSupport.h>

CDeviceSupport::CDeviceSupport()
{
    m_clDeviceSupportSet.insert("LPC2103");
}

bool
CDeviceSupport::IsSupported(string strDevice)
{
    if( m_clDeviceSupportSet.find(strDevice) != m_clDeviceSupportSet.end() )
        return true;

    return false;
}
