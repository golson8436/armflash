/*!\file  UUcoder.cxx  Handling of flashing info data
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

#ifndef CFLASHDATA_H
#define CFLASHDATA_H

#include <string>
#include <sstream>
#include <device/CDeviceBase.h>
#include <device/CDeviceSupport.h>
#include <vector>

typedef struct SFlashData_
{
    string strPortName;
    string strFirmwarePath;
    speed_t stBaudRate;
    string strBaudRate;
    string strDevice;
    string strCrystalSpeed;
    unsigned int nCrystalSpeed;

    friend bool operator==(const struct SFlashData_ & x, const struct SFlashData_ & y)
    {
        return ( 
            (x.strPortName == y.strPortName) && 
            (x.strFirmwarePath == y.strFirmwarePath) && 
            (x.stBaudRate == y.stBaudRate) && 
            (x.strDevice == y.strDevice) &&
            (x.strCrystalSpeed == y.strCrystalSpeed)
        );
    }

    friend bool operator<(const struct SFlashData_ & x, const struct SFlashData_ & y)
    {
        return x.strPortName < y.strPortName;
    }

    friend ostream& operator<<(ostream& out, struct SFlashData_& x)
    {
        return (out << "DATA: " << x.strPortName << " ; " << x.strFirmwarePath << " ; " << x.strBaudRate << " ; " << x.nCrystalSpeed << " ; " << x.strDevice << endl);
    }

} SFlashData;

class CFlashData
{
private:
    vector<SFlashData> clDataSet;

    CFlashData();
public:
    CFlashData(int argc, char ** argv);
    ~CFlashData();

    unsigned int GetDataCount();
    SFlashData   GetData(unsigned int num);
};

#endif
