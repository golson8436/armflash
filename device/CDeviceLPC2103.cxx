/*!\file  CDeviceLPC2103.cxx  LPC2103 Class
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

#include <device/CDeviceLPC2103.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <firmware/CFirmwareHEX32.h>
#include <tools/UUcoder.h>
#include <errno.h>

using namespace std;

#define CR_LF '\n'
#define CRYSTAL_STR_LENGTH 7
#define MAXIMUM_ROOLS 0xFFFF

#define CMD_SYNCHRONIZED "Synchronized\r\n"
#define CMD_OK		 "OK\r\n"
#define CMD_RESEND	 "RESEND\r\n"
#define CMD_INIT	 "?"
#define CMD_UNLOCK	 "U 23130\r\n"
#define CMD_MAX_TRIES	 5

#define FULL_CHUNK_SIZE	 900
#define UU_MAX_LINE_BYTES 45
#define SECTOR_SIZE 4096
#define SECTOR_SIZE_STR "4096"

#define MAX_REPS	1000

#define USEC_PER_SEC	1000000
#define USEC_POLL	100000

//#define USE_ROLLING_STICK

// Tries to parse numeric reply from the input buffer
int
GetRep(const char *rgRepBuffer, int nLen)
{
	string strNumRep;
	stringstream ssParser;
	int rnds = 0;
	int nToRet;

	while(--nLen)
	{
		if( rgRepBuffer[nLen] >= '0' && rgRepBuffer[nLen] <= '9' )
		{
			strNumRep = rgRepBuffer[nLen] + strNumRep;
		}
		rnds++;

		if( rnds >= 4 )
			break;
	}
	
	//cout << "REPLY: " << strNumRep << endl;
	ssParser << strNumRep;
	ssParser >> nToRet;

	return nToRet;
}	


CDeviceLPC2103::CDeviceLPC2103()
{
	SetConnDeviceType(DEVICE_CONN_TYPE_SERIAL); 	//this device can only be programmed through ISP or JTAG
	SetRomSize( 32*1024 );	 			// 32Kb of ROM
	SetRamSize( 8*1024 ); 	 			// 8Kb of RAM
}


CDeviceLPC2103::CDeviceLPC2103(string strDevName, unsigned int unCrystalHz, speed_t stBaudRate)
{
	SetConnDeviceType( DEVICE_CONN_TYPE_SERIAL );
	SetConnDeviceName( strDevName );
	SetRomSize( 32*1024 );	 			// 32Kb of ROM
	SetRamSize( 8*1024 ); 	 			// 8Kb of RAM
	SetCrystalSpeedHz( unCrystalHz );

	m_pclSerialPort = new CSerial( strDevName.c_str(), stBaudRate );
    m_pclFlashingStatus = new CFlashingStatus();

	// fill in the ERROR code explanations:
	m_mapErrorCodes[INVALID_COMMAND] = "Invalid command.";
	m_mapErrorCodes[SRC_ADDR_ERROR] = "Source address is not on a word boundary.";
	m_mapErrorCodes[DST_ADDR_ERROR]	= "Destination address is not on a correct boundary.";
	m_mapErrorCodes[SRC_ADDR_NOT_MAPPED] = "Source address is not mapped in the memory map. Count value is taken in to consideration where applicable.";
	m_mapErrorCodes[DST_ADDR_NOT_MAPPED] = "Destination address is not mapped in the memory map. Count value is taken in to consideration where applicable.";
	m_mapErrorCodes[COUNT_ERROR] = "Byte count is not multiple of 4 or is not a permitted value.";
	m_mapErrorCodes[INVALID_SECTOR] = "Sector number is invalid or end sector number is greater than start sector number.";
	m_mapErrorCodes[SECTOR_NOT_BLANK] = "Sector is not blank.";
	m_mapErrorCodes[SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION] = "Command to prepare sector for write operation was not executed.";
	m_mapErrorCodes[COMPARE_ERROR] = "Source and destination data not equal.";
	m_mapErrorCodes[BUSY] = "Flash programming hardware interface is busy.";
	m_mapErrorCodes[PARAM_ERROR] = "Insufficient number of parameters or invalid parameter.";
	m_mapErrorCodes[ADDR_ERROR] = "Address is not on word boundary.";
	m_mapErrorCodes[ADDR_NOT_MAPPED] = "Address is not mapped in the memory map. Count value is taken in to consideration where applicable.";
	m_mapErrorCodes[CMD_LOCKED] = "Command is locked.";
	m_mapErrorCodes[INVALID_CODE] = "Unlock code is invalid.";
	m_mapErrorCodes[INVALID_BAUD_RATE] = "Invalid baud rate setting.";
	m_mapErrorCodes[INVALID_STOP_BIT] = "Invalid stop bit setting.";	
	m_mapErrorCodes[CODE_READ_PROTECTION_ENABLED] = "Code read protection enabled.";

	m_bInitialized = false;
}

CDeviceLPC2103::~CDeviceLPC2103()
{
	delete m_pclSerialPort;
    delete m_pclFlashingStatus;
}

bool 
CDeviceLPC2103::InitializeDevice()
{
	if( GetConnDeviceName() == "" )
	{
		cout << "ERROR: InitializeDevice() called before SetConnDeviceName()" << endl;
		return false;
	}

	string strResponse;
    unsigned int nRollCount = 0;
	//unsigned char rgBuffer[256];
	//int nReadBytes = 0;	
	bool bSynchStart = false;
#ifdef USE_ROLLING_STICK
	unsigned int nRollingStick = 0;
#endif
	stringstream ssCrystalParser;
	ssCrystalParser << GetCrystalSpeedHz();
	string strCrystalHz;
	ssCrystalParser >> strCrystalHz;
	strCrystalHz += "\r\n";

	// open the serial port
	if( m_pclSerialPort->Open() != SUCCESS )
	{
		cerr << "Error during opening " << m_strConnDevice << " device." << endl;
        return false;
	}

	// initialize the serial port
	if( m_pclSerialPort->Init() != SUCCESS )
	{
		cerr << "Error while initializing " << m_strConnDevice << endl;
        return false;
	}

SYNC_START:

    if( nRollCount > 60 )
    {
        cout << GetConnDeviceName() << ": Timed out waiting for synchronization!" << endl;
        return false;
    }

	if( SendCommand(CMD_INIT, CMD_SYNCHRONIZED, 1) != SUCCESS )
	{
		goto DO_ROLL;
	}

	if( SendCommand( string(CMD_SYNCHRONIZED), string(CMD_OK), 5) != SUCCESS )
	{
		
		goto DO_ROLL;
	}

	if( SendCommand( strCrystalHz, string(CMD_OK), 5) != SUCCESS )
	{
		
		goto DO_ROLL;
	}

	m_bInitialized = true;

    cout << GetConnDeviceName() << ": Synchronized OK." << endl;

	return true;

DO_ROLL:
    nRollCount++;

#ifdef USE_ROLLING_STICK
	if( !bSynchStart )
	{
		bSynchStart = true;
		cout << "Waiting to synchronize (press reset while P0.14 LOW)." << endl;
		cout << " \\";
		goto SYNC_START;
	} else {

		cout << char(8);

		if( nRollingStick == MAXIMUM_ROOLS )
		{
			cout << "Giving up..." << endl;	
			return false;
		}

		int tmp = nRollingStick%4;

		if( tmp == 0 )
			cout << "|";
		if( tmp == 1 )
			cout << "/";
		if( tmp == 2 )
			cout << "-";
		if( tmp == 3 )
			cout << "\\";

		cout.flush();
		nRollingStick++;
		goto SYNC_START;
	}
#else
	if( !bSynchStart )
	{
		bSynchStart = true;
		cout << GetConnDeviceName() << ": Waiting to synchronize (press reset while P0.14 LOW)" << endl;
        cout.flush();
		goto SYNC_START;
	} else {
		//cout << ".";
		//cout.flush();
		goto SYNC_START;
	}
#endif		

	//m_pclSerialPort->Close();*/
	return true;
}

bool 
CDeviceLPC2103::InitializeDevice(string strDevName)
{
	SetConnDeviceName(strDevName);
	return InitializeDevice();
}

vector<string> 
CDeviceLPC2103::GetDeviceInfo()
{
	vector<string> clToRet;

	return clToRet;
}

int 
CDeviceLPC2103::SendCommand(string strCmd, string strExpRep, unsigned int nTimeoutSec)
{
	size_t nWroteBytes = 0;
	string strTmp;

	unsigned int nReps = (nTimeoutSec * USEC_PER_SEC) / USEC_POLL;

	//cout << "SendCommand:: " << strCmd << " " << strCmd.length() << " : " << strExpRep << " " << strExpRep.length() << endl;

	//m_pclSerialPort->Flush();
	nWroteBytes = m_pclSerialPort->Write( (unsigned char *)strCmd.c_str(), strCmd.length() );
	//usleep(50000);

	if( nWroteBytes != strCmd.length() )
	{
		cerr << "ERROR: didnt get to write all the bytes during " << strCmd << " command!" << endl;
		return FAILURE;
	} else {
		//cout << "Wrote " << nWroteBytes << "B" << endl;
	}

	
	//int nReply;
	char rgBuffer[256];


	while( 1 )
	{
		//usleep(USEC_POLL);
		if( strExpRep.length() == 0 )
			return SUCCESS;

		int nRead = m_pclSerialPort->Read_NonBlock((unsigned char *)rgBuffer, 128);

		if( nRead > 0 )	
		{
			rgBuffer[nRead] = '\0';
			strTmp += rgBuffer;

			//cout << "STR_TMP: " << strTmp << endl;

			size_t found = strTmp.find( strCmd );
			if( found != string::npos )
			{
				//cout << "Erasing" << endl;
				strTmp.erase(found, strCmd.length());
			}

			if( strTmp.find( strExpRep ) != string::npos )
			{
				//m_pclSerialPort->Flush();
				return SUCCESS;

			}
		} 
		else if( nRead < 0 )
		{
			if(errno == EAGAIN)
			{
				//printf("SERIAL EAGAIN ERROR \n");
				usleep(100000);
			}
			else
			{
				printf("SERIAL read error %d %s\n",errno, strerror(errno));
			}

		}

		nReps--;

		if( nReps <= 0 )
		{
			return FAILURE;
		}
	}

}

bool 
CDeviceLPC2103::FlashDevice(string strFirmwarePath)
{
	CFirmwareHEX32 clHexToFlash;
	string strFileExt = strFirmwarePath.substr(strFirmwarePath.length()-3, 3);
	unsigned char rgFlashImage[32*1024];
	unsigned char *pFlashImage = rgFlashImage;
	unsigned int nDataCount=0;
	unsigned char rgBuffer[256];
	//int nReadBytes = 0;	
	CUUcoder clUUcoder;

	if( !m_bInitialized )
	{
		cerr << GetConnDeviceName() << ": The device was not initialized... Call InitializeDevice() first!" << endl;
		return false;
	}
		
	if( strFileExt == string("hex") || strFileExt == string("HEX") )
	{
		if( !clHexToFlash.OpenFirmware(strFirmwarePath.c_str()) )
		{
			cout << GetConnDeviceName() << ": ERROR: couldn't open " << strFirmwarePath << endl;
		}

		if( clHexToFlash.CheckFirmware() )
		{
			cout << GetConnDeviceName() << ": File " << strFirmwarePath << " seems to be valid! CRC test passed." << endl;
		}
		 else
		{
			cout << GetConnDeviceName() << ": File " << strFirmwarePath << " is invalid! CRC test failed!" << endl;
            return false;
		}


		uint32_t nAdr,rgData[HEX32_DATA_MAXLEN],nDataLen = HEX32_DATA_MAXLEN;
		for(int i=0; i<HEX32_DATA_MAXLEN; i++) rgData[i] = 0;
		while( clHexToFlash.GetNextAdrData(nAdr, rgData, nDataLen, false) )
		{
			if( nDataLen == 0 )
			{
				//reset the nDataLen and continue
				nDataLen = HEX32_DATA_MAXLEN;
				continue;
			}

			for( unsigned int i=0; i<nDataLen; i++ )
			{
				if( nDataCount >= 32*1024 ){
					cerr << GetConnDeviceName() << ": ERROR: Flash image bigger than the actual flash of the device!" << endl;
					return false;
				}

				*pFlashImage = (unsigned char)( rgData[i] & 0xFF );
				pFlashImage++;
				nDataCount++;
			}

			//reset the nDataLen parameter since it may have
			//been changed in the GetNextAdrData call
			nDataLen = HEX32_DATA_MAXLEN;
		}

		//cout << "Read " << nDataCount << "B from " << strFirmwarePath << endl;

	} 
	else
	{
		cout << GetConnDeviceName() << ": File doesn't end with .hex, no other files yet supported." << endl;
		return false;
	}

	// make valid code sector
	unsigned char *p_vcs = rgFlashImage;
	unsigned int sum=0;
	int addr;

	for (addr=0; addr<0x20; addr+=4) {
		if (addr != 0x14) {
			sum += (p_vcs[addr] | (p_vcs[addr+1] << 8) | (p_vcs[addr+2] << 16) | (p_vcs[addr+3] << 24));
		}
	}
	sum ^= 0xFFFFFFFF;
	sum++;

	p_vcs[0x14 + 0] = (sum >> 0)  & 255;
	p_vcs[0x14 + 1] = (sum >> 8)  & 255;
	p_vcs[0x14 + 2] = (sum >> 16) & 255;
	p_vcs[0x14 + 3] = (sum >> 24) & 255;


	//NOTE: If we get here, the whole image is buffered and ready to be sent to device


	// first lets pad the image with zeros so the size is of multiple of a sector size (4K)
	int nSectors = nDataCount / SECTOR_SIZE;
	int nPadLast = (nSectors+1)*SECTOR_SIZE;

	if( nDataCount % SECTOR_SIZE != 0 )
	{
		int i=nDataCount;
		for(; i<nPadLast; i++)
		{
			*pFlashImage = 0xFF;
			pFlashImage++;
			nDataCount++;
		}
	}

	if( SendCommand(CMD_UNLOCK, "0\r\n", 5) != SUCCESS )
	{
		cout << GetConnDeviceName() << ": Error while unlocking the device!" << endl;
		m_pclSerialPort->Close();
		return false;
	}
	else
	{
		cout << GetConnDeviceName() << ": Device unlocked! Flashing starting..." << endl;
	}

	int nTotalSectors 	 = nDataCount / SECTOR_SIZE;
	//int nFullChunksPerSector = SECTOR_SIZE / FULL_CHUNK_SIZE;
	pFlashImage = rgFlashImage;

	for(int nCurSector=0; nCurSector<nTotalSectors; nCurSector++)
	{
		unsigned int nRamAddress = 0x40000200;

		m_pclSerialPort->FlushI();
		m_pclSerialPort->FlushO();

		//prepare and erase sector
		stringstream ssCurSect,ssRamAddress;
		ssCurSect << nCurSector;
		string strCurSect;
		ssCurSect >> strCurSect;
		ssRamAddress << nRamAddress;
		string strRamAddress;
		ssRamAddress >> strRamAddress;

		string strPrepCmd, strEraseCmd;
	
		//strPrepCmd  = "P " + strCurSect + " " + strCurSect + "\r\n";
		strPrepCmd  = "P 0 " + strCurSect + "\r\n";
		strEraseCmd = "E " + strCurSect + " " + strCurSect + "\r\n";

		if( SendCommand( strPrepCmd, "0\r\n", 5 ) == SUCCESS )
		{
			//cout << "Sector " << strCurSect << " prepared." << endl;
		}
		else
		{
			cout << GetConnDeviceName() << ": Error while preparing sector " << strCurSect << endl;
			m_pclSerialPort->Close();
			return false;
		}

		if( SendCommand( strEraseCmd, "0\r\n", 5 ) == SUCCESS )
		{
			//cout << "Sector " << nCurSector << " erased." << endl;
		}
		else
		{
			cout << GetConnDeviceName() << ": Error while erasing sector " << strCurSect << endl;
		}

		

		//make ram ready to write full sector size
		string strRamWriteCmd = "W " + strRamAddress + " " + SECTOR_SIZE_STR + "\r\n";

		if( SendCommand( strRamWriteCmd, "0\r\n", 5 ) == SUCCESS )
		{
			//cout << "RAM ready to receive sector " << nCurSector << " data." << endl;
		}
		else
		{
			cout << GetConnDeviceName() << ": Error while getting RAM ready for write operation" << endl;
		}
		
		

		m_pclSerialPort->Flush();
	
		
		int nDataSent = 0;

		int nCurLine  = 0;
		int nChecksum = 0;
		string strCurLine;
		string strToSend;

		for(unsigned int nCurLineStart=0; nCurLineStart<SECTOR_SIZE; nCurLineStart+=UU_MAX_LINE_BYTES)
		{
			nCurLine++;

			if( (nCurLineStart + UU_MAX_LINE_BYTES >= SECTOR_SIZE) )
			{
				//cout << "Special..." << endl;
				int left = 0;
				for(int j=nCurLineStart; j<SECTOR_SIZE; j++)
				{
					nChecksum += (unsigned int)pFlashImage[j - nCurLineStart];	
					left++;
				}
			
				strCurLine = clUUcoder.UUEncode( pFlashImage, left );
				strCurLine += "\r\n";
			
				m_pclSerialPort->Write( (const unsigned char *)strCurLine.c_str(), strCurLine.length() );

				//while( m_pclSerialPort->Read_NonBlock( rgBuffer, 128 ) > 0 );
				//while( m_pclSerialPort->GetReady() > 0 );

				nDataSent   += left;

				//cout << nDataSent << "B Sent" << endl;

				pFlashImage += left;

				goto CHECKSUM_START;
			}


			

			strCurLine = clUUcoder.UUEncode( pFlashImage, UU_MAX_LINE_BYTES );
			strCurLine += "\r\n";

			m_pclSerialPort->Write( (const unsigned char *)strCurLine.c_str(), strCurLine.length() );
			usleep(50000);
			while( m_pclSerialPort->Read_NonBlock( rgBuffer, 128 ) > 0 );		
			//while( m_pclSerialPort->GetReady() > 0 );

			for(int j=0; j<UU_MAX_LINE_BYTES; j++)
					nChecksum += (unsigned int)pFlashImage[j];

			nDataSent   += UU_MAX_LINE_BYTES;

			pFlashImage += UU_MAX_LINE_BYTES;


			if( nCurLine % 20 == 0 )
			{
CHECKSUM_START:
				//cout << "checksum: " << nChecksum << " pFlashImage: " << (unsigned int)pFlashImage << endl;
				stringstream ssChecksum;

				ssChecksum << nChecksum;
				string strChecksumCmd;
				ssChecksum >> strChecksumCmd;
		
				strChecksumCmd += "\r\n";

				strToSend += strChecksumCmd;

				//m_pclSerialPort->Flush();

				if( SendCommand( strChecksumCmd, CMD_OK, 5 ) == SUCCESS )
				{
					//cout << "."; cout.flush();
					nChecksum = 0;
				}
				else
				{
					cout << GetConnDeviceName() << ": Error while getting reply to checksum!" << endl;
					m_pclSerialPort->Close();
					return false;
				}	
			}
			
		}
        cout << GetConnDeviceName() << ": Sector " << nCurSector + 1 << "/" << nTotalSectors << " programmed." << endl;
		//cout << "OK" << endl;

		//prepare sector again
		if( SendCommand( strPrepCmd, "0\r\n", 5 ) == SUCCESS )
		{
			//cout << "Sector " << strCurSect << " prepared." << endl;
		}
		else
		{
			cout << GetConnDeviceName() << ": Error while preparing sector " << strCurSect << endl;
			m_pclSerialPort->Close();
			return false;
		}

		stringstream ssRomAdr;
		ssRomAdr << (nCurSector*SECTOR_SIZE);
		string strRomAdr;
		ssRomAdr >> strRomAdr;

		// copy from ram to rom
		string strCopyRam2RomCmd = "C " + strRomAdr + " " + strRamAddress + " " + SECTOR_SIZE_STR + "\r\n";

		if( SendCommand( strCopyRam2RomCmd, "0\r\n", 5 ) == SUCCESS )
		{
			usleep(10000);
		}
		else
		{
			cout << GetConnDeviceName() << ": Error while copying to sector " << strCurSect << endl;
			m_pclSerialPort->Close();
			return false;
		}
		

		if( nCurSector == (nTotalSectors - 1) )
		{
			//prepare sector again
			if( SendCommand( strPrepCmd, "0\r\n", 5 ) == SUCCESS )
			{
				//cout << "Sector " << strCurSect << " prepared." << endl;
				m_pclSerialPort->Flush();
			}
			else
			{
				cout << GetConnDeviceName() << ": Error while preparing sector " << strCurSect << endl;
				m_pclSerialPort->Close();
				return false;
			}


			string strGoRun = "G 0 A\r\n";
		
			m_pclSerialPort->Write( (const unsigned char *)strGoRun.c_str(), strGoRun.length() );
			cout << GetConnDeviceName() << ": Running in ARM mode from 0x00000000." << endl;
			m_pclSerialPort->Close();	
			return true;
		}

	}

	
	m_pclSerialPort->Close();
	return true;
}

string
CDeviceLPC2103::GetConnDeviceName() const
{
    int orig_len = m_strConnDevice.length();
    string to_add;
    while(orig_len++ < 12)
        to_add += ' ';

	return m_strConnDevice + to_add;
}

