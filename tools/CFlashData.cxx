/*!\file  CFlashData.cxx  Data structure used to handle main flashing information
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

#include "CFlashData.h"

#include <iostream>
#include <sstream>

CFlashData::CFlashData(int argc, char ** argv)
{
    for(int i=1; i<argc; i+=5)
    {
        if( argv[i][0] == '-' )
            break;

        if( i+4 >= argc )
            break;

        SFlashData new_data;

        new_data.strPortName = argv[i];
        new_data.strFirmwarePath = argv[i+1];
        new_data.strBaudRate = argv[i+2];
        new_data.strCrystalSpeed = argv[i+3];
        new_data.strDevice = argv[i+4];

        string strBaudRate = argv[i+2];
        new_data.stBaudRate = B9600;

        stringstream ss;

        ss << new_data.strCrystalSpeed;
        ss >> new_data.nCrystalSpeed;

        if( strBaudRate == "9600" )
            new_data.stBaudRate = B9600;
        else if( strBaudRate == "38400" )
            new_data.stBaudRate = B38400;


        clDataSet.push_back( new_data );

    }
}

CFlashData::~CFlashData()
{
    
}

unsigned int 
CFlashData::GetDataCount()
{
    return clDataSet.size();
}

SFlashData   
CFlashData::GetData(unsigned int num)
{

    if( num >= GetDataCount() )
    {
        SFlashData dummy;
        return dummy;
    }

    return clDataSet[num];
}
