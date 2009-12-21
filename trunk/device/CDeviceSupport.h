/*!\file  CDeviceSupport.h  Supported device handling
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

#ifndef CDEVICESUPPORT_H
#define CDEVICESUPPORT_H

#include <set>
#include <string>

using namespace std;

// singleton class

class CDeviceSupport
{
private:
    set<string> m_clDeviceSupportSet;
    static CDeviceSupport *m_pInstance;
    CDeviceSupport();
public:
    
    ~CDeviceSupport();

    bool IsSupported(string strDevice);
    
    static CDeviceSupport *Instance()
    {
        if( !m_pInstance )
            m_pInstance = new CDeviceSupport();

        return m_pInstance;
    }
};


#endif
