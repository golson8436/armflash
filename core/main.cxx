/*!\file  main.cxx  The main file
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


#include <iostream>
#include <getopt.h>
#include <iomanip>
#include "CFirmwareHEX32.h"
#include "defs.h"
#include "cmdargs.h"
#include "serial.h"
#include "CDeviceLPC2103.h"
#include "UUcoder.h"
#include <tools/CFlashData.h>
#include <device/CDeviceSupport.h>
#include <pthread.h>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>

using namespace std;

/*#if defined(__FreeBSD__) || defined(__OpenBSD__)

extern "C"
{
	void print_help(const char * pszPrgName);
}

#endif*/



void *FlashThread(void *pData)
{
    SFlashData  *pRealData = (SFlashData *)pData;
    CDeviceBase *pFlashDevice;
  
    if( pRealData->strDevice == "LPC2103" )
    {
        pFlashDevice = new CDeviceLPC2103( pRealData->strPortName, pRealData->nCrystalSpeed, pRealData->stBaudRate );

	    pFlashDevice->InitializeDevice();
	    pFlashDevice->FlashDevice(pRealData->strFirmwarePath);
    }

    delete pRealData;
    return NULL;
}

int 
main(int argc, char ** argv)
{

	int nNextOpt;
	
	//control variables
	bool bPrintHelp = false,
	     bPrintVer = false,
	     bDetectSerial = false,
	     bRawDump = false,
         bFlashingData = false,
         bIsRoot = false;

	string strRawDumpFirmware;

	if( argc == 1 ) //only the program name is parameter
	{
		cerr << "No arguments specified!\n" << endl;
		bPrintHelp = true;
	}

    uid_t uid = geteuid();
    gid_t gid = getegid();

    if( uid == 0 && gid == 0 )
        bIsRoot = true;

    CFlashData clFlashDataArgs(argc, argv);

    if( clFlashDataArgs.GetDataCount() > 0 ) 
        bFlashingData = true;

	do {

		nNextOpt = getopt_long(argc, argv, pszArmFlashSo, rgstArmFlashLo, NULL);
		
		switch( nNextOpt ){
			case OPT_HELP:
				bPrintHelp = true;
				break;
			case OPT_VER:
				bPrintVer = true;
				break;
			case OPT_DETECT_RS232:
				bDetectSerial = true;
				break;
			case OPT_RAWDUMP:
				bRawDump = true;
				strRawDumpFirmware = optarg;	
				break;
			case -1:
				break;
			default:
				//bPrintHelp = true;
                cout << "Use armflash -h for help!" << endl;
				break;
		}

	} while( nNextOpt != -1 );

	if( bPrintHelp )
	{
		print_help(argv[0]);
		return 0;
	}

	if( bPrintVer )
		cout << "Your version: " << ARM_FLASH_VERSION_STR << endl;	

	if( bRawDump )
	{
		cout << "Raw dump of " << strRawDumpFirmware << ". Skipping all other options." << endl;

		//check if filename ends with .hex
		string strFileExt = strRawDumpFirmware.substr(strRawDumpFirmware.length()-3, 3);
		
		if( strFileExt == string("hex") || strFileExt == string("HEX") )
		{
			CFirmwareHEX32 clHexToDump;

			if( !clHexToDump.OpenFirmware(strRawDumpFirmware.c_str()) )
			{
				cout << "ERROR: couldn't open " << strRawDumpFirmware << endl;
			}

			cout << "Checking the firmware checksums" << endl;
			if( clHexToDump.CheckFirmware(true) )
				cout << "File seems to be valid! CRC test passed." << endl;
			else
				cout << "File is invalid! CRC test failed!" << endl;

			uint32_t nAdr,rgData[HEX32_DATA_MAXLEN],nDataLen = HEX32_DATA_MAXLEN;
			for(int i=0; i<HEX32_DATA_MAXLEN; i++) rgData[i] = 0;

			while( clHexToDump.GetNextAdrData(nAdr, rgData, nDataLen, true) )
			{
				if( nDataLen == 0 )
				{
					//reset the nDataLen and continue
					nDataLen = HEX32_DATA_MAXLEN;
					continue;
				}

				printf("Adr: 0x%08x Data: ",nAdr);

				for( unsigned int i=0; i<nDataLen; i++ )
				{
					printf("%02x ", rgData[i]);
				}
				cout << endl;

				//reset the nDataLen parameter since it may have
				//been changed in the GetNextAdrData call
				nDataLen = HEX32_DATA_MAXLEN;
			}
		}
		else
		{
			cerr << "ERROR: Extension of the file is not .hex (hex must be lower case)" << endl;
		}

		return 0;
	}

	if( bDetectSerial )
	{
        if( !bIsRoot )
        {
            cerr << "ERROR: Detection of serial ports only works if you are logged in as root!" << endl;
            return -1;
        }

		//serial_autodetect();
        vector<string> clPorts = CSerial::SerialAutodetect();
        
        if( clPorts.empty() )
        {
            cout << "No serial ports detected on your system!" << endl;
        }
         else
        {
            cout << "The following serial ports were detected on your system:" << endl;
            cout << "--------------------------------------------------------" << endl;
            copy(clPorts.begin(), clPorts.end(), ostream_iterator<string>(cout, "\n"));
        }

        return 0;
	}

    if( bFlashingData )
    {
        if( !bIsRoot )
        {
            cerr << "ERROR: Flashing only works if you are logged in as root!" << endl;
            return -1;
        }

        pthread_t flash_threads[32];

        for(unsigned int i=0; i<clFlashDataArgs.GetDataCount(); i++)
        {
            SFlashData *tmp_data = new SFlashData;
            *tmp_data = clFlashDataArgs.GetData(i);
            //cout << *tmp_data << endl;

            pthread_create(&flash_threads[i], NULL, &FlashThread, tmp_data);
        }

        for(unsigned int i=0; i<clFlashDataArgs.GetDataCount(); i++)
        {
            pthread_join(flash_threads[i], NULL);
        }
    }

	return 0;
}


/*
Linux output example:

core2 arm_flash # ./armflash /dev/ttyS0 ./GZe_RTOS.hex 9600 19660 LPC2103 /dev/ttyUSB0 ./GZe_RTOS.hex 38400 10000 LPC2103
/dev/ttyUSB0: Waiting to synchronize (press reset while P0.14 LOW)
/dev/ttyS0  : Waiting to synchronize (press reset while P0.14 LOW)
/dev/ttyUSB0: Synchronized OK.
/dev/ttyUSB0: File ./GZe_RTOS.hex seems to be valid! CRC test passed.
/dev/ttyUSB0: Device unlocked! Flashing starting...
/dev/ttyS0  : Synchronized OK.
/dev/ttyS0  : File ./GZe_RTOS.hex seems to be valid! CRC test passed.
/dev/ttyS0  : Device unlocked! Flashing starting...
/dev/ttyUSB0: Sector 1/2 programmed.
/dev/ttyUSB0: Sector 2/2 programmed.
/dev/ttyS0  : Sector 1/2 programmed.
/dev/ttyUSB0: Running in ARM mode from 0x00000000.
/dev/ttyS0  : Sector 2/2 programmed.
/dev/ttyS0  : Running in ARM mode from 0x00000000.

*/
