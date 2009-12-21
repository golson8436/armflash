/*!\file  serial.cxx  Serial communication handling
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


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <core/serial.h>
#include <fcntl.h>
#include <errno.h>

using namespace std;

int
serial_autodetect(void)
{
	const char * tty_drivers_file          = "/proc/tty/drivers";
	const char * tty_driver_file_serial    = "/proc/tty/driver/serial";
	const char * tty_driver_file_usbserial = "/proc/tty/driver/usbserial";
	const char * tty_drivers_serial        = "serial";
	const char * tty_drivers_usbserial     = "usbserial";
	const int lineBufferSize = 512;

	set<string> setAlivePorts;

	bool bSerialDetected = false;
	bool bUSBSerialDetected = false;

	string sSerialDevPrefix,
	       sUSBSerialDevPrefix;

	char lineBuffer[lineBufferSize]; //half KB buffer for line data

	ifstream fDevTTYDrivers(tty_drivers_file);
	
	while( fDevTTYDrivers.getline( lineBuffer, lineBufferSize ) )
	{
		stringstream ss(lineBuffer);
		string word;

		while( ss >> word )
		{
			//we are not interested in anything but serial and usbserial
			if( word != tty_drivers_serial && word != tty_drivers_usbserial )
				break;

			if( word == tty_drivers_serial )
			{
				bSerialDetected = true;
				ss >> sSerialDevPrefix;
			} 

			if( word == tty_drivers_usbserial )
			{
				bUSBSerialDetected = true;
				ss >> sUSBSerialDevPrefix;
			}
		}
		
		
	}

	if( !bSerialDetected && !bUSBSerialDetected )
	{
		cout << ERRSTR << "You dont seem to have serial port drivers installed!" << endl;
	}

	if( bSerialDetected )
	{
		cout << "Serial driver detected with device prefix: " << sSerialDevPrefix << endl;
		cout << "\tScanning ports..." << endl;

		ifstream fDevTTYDriverSerial(tty_driver_file_serial);

		if( !fDevTTYDriverSerial.is_open() )
		{
			cerr << '\t' << ERRSTR << "Could not open " << tty_driver_file_serial << " make sure you have the read permission for this file or whether the file exists!" << endl;
			cerr << "\tIf you do and it does please report this as a bug." << endl;
			return 0;
		}

		fDevTTYDriverSerial.getline( lineBuffer, lineBufferSize ); //ommit the first line

		while( fDevTTYDriverSerial.getline( lineBuffer, lineBufferSize ) )
		{
			stringstream ss(lineBuffer);
			string portnum, portname;

			ss >> portnum >> portname;

			//if the available port is actually wired out on the mother board
			//its what we were looking for
			//Note: ie. laptop motherboards usually have at least 2 serial port
			// support but sometimes they (some or all of them) are not wired out  
			if( portname != "uart:unknown" )
			{
				setAlivePorts.insert( sSerialDevPrefix + portnum.substr(0, portnum.length()-1) );
			}
		}

		cout << "\tDone scanning..." << endl;

	}

	if( bUSBSerialDetected )
	{
		cout << "USB serial driver detected with device prefix: " << sUSBSerialDevPrefix << endl;
		cout << "\tScanning ports..." << endl;

		//open the driver file
		ifstream fDevTTYDriverUsberial(tty_driver_file_usbserial);

		//check if the file was open
		if( !fDevTTYDriverUsberial.is_open() )
		{
			cerr << '\t' << ERRSTR << "Could not open " << tty_driver_file_usbserial << " make sure you have the read permission for this file!" << endl;
			return 0;
		}

		//ommit the first line - its not interesting for us
		fDevTTYDriverUsberial.getline( lineBuffer, lineBufferSize );

		//and modify the rest to 'human readable' format
		while( fDevTTYDriverUsberial.getline( lineBuffer, lineBufferSize ) )
		{
			stringstream ss(lineBuffer);
			string portnum;

			ss >> portnum;

			setAlivePorts.insert( sUSBSerialDevPrefix + portnum.substr(0, portnum.length()-1) );
		}	

		cout << "\tDone scanning..." << endl;

	}	

	if( setAlivePorts.empty() )
	{
		cout << "No alive ports detected!" << endl;
	}
	 else
	{
		cout << endl << "Alive ports list:" << endl;
		
		//print the detected serial ports
		for( set<string>::iterator i = setAlivePorts.begin(); i != setAlivePorts.end(); ++i )
		{
			cout << *i << endl;
		}
	}

	return setAlivePorts.size();
}

int
CSerial::Init(void)
{
	// If there was serial activity already,
	// Save current serial port settings, also do an error check
	if( fdSerialDevice > BAD_DEVICE )
	{ 
		if( tcgetattr(fdSerialDevice, &stTioNew) != SUCCESS )
			return FAILURE;
	}


	// Set the port baud rate speed (WARNING: non-standard function this should be resolved) 
	//if( cfsetspeed(&stTioNew, stBaudRate) != SUCCESS )
	//	return FAILURE;

	cfsetispeed(&stTioNew, stBaudRate);
	cfsetospeed(&stTioNew, stBaudRate);

	stTioNew.c_iflag = (IGNBRK | IGNPAR);
	//stTioNew.c_cflag = (stBaudRate | CS8 | CREAD | CLOCAL | HUPCL);
	stTioNew.c_cflag |= (CLOCAL | CREAD | HUPCL);

	stTioNew.c_oflag = 0;
	stTioNew.c_lflag = 0;

	/*stTioNew.c_iflag = (IGNBRK | IGNPAR);
	stTioNew.c_cflag = (stBaudRate | CS8 | CREAD | CLOCAL | HUPCL);
	stTioNew.c_oflag = 0;
	stTioNew.c_lflag = 0;*/

	//stTioNew.c_cflag &= ~CSIZE; /* Mask the character size bits */
	//stTioNew.c_cflag |= CS8;    /* Select 8 data bits */

	stTioNew.c_cflag &= ~PARENB;		
	stTioNew.c_cflag &= ~CSTOPB;
	stTioNew.c_cflag &= ~CSIZE;
	stTioNew.c_cflag |= CS8;

	//stTioNew.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

 	//stTioNew.c_lflag |= (ICANON | ECHO | ECHOE);


	//stTioNew.c_cc[VTIME] = 5;


	// Clean the port line and activate the settings for the port
	if( fdSerialDevice > BAD_DEVICE )
	{
		if( tcflush(fdSerialDevice, TCIFLUSH | TCOFLUSH) != SUCCESS )
			return FAILURE;
	}

	if( fdSerialDevice > BAD_DEVICE )
	{
		if( tcsetattr(fdSerialDevice, TCSANOW, &stTioNew) != SUCCESS )
			return FAILURE;
	}

	//if we get here then everything went fine
	return SUCCESS;
}


int
CSerial::Restore(void)
{
	// Clean the port line and restore the settings for the port
	if( fdSerialDevice > BAD_DEVICE )
	{
		if( tcsetattr(fdSerialDevice, TCSANOW, &stTioOld) != SUCCESS )
			return FAILURE;
	}

	return SUCCESS;
}

int
CSerial::Open(void)
{
	// Open serial device for reading and writing and not as controlling tty
	// because we don't want to get killed if linenoise sends CTRL-C.

    fdSerialDevice = open(pszDeviceName, O_RDWR | O_NOCTTY ); 

	if (fdSerialDevice < 0) {
		cout << ERRSTR << "Can't open file " << pszDeviceName << endl;
		return FAILURE;
	}
	
	return SUCCESS;
}

void
CSerial::Close(void)
{
	close(fdSerialDevice);
}

unsigned char
CSerial::Read(void)
{
	unsigned char uByte;
	read(fdSerialDevice, &uByte, 1);
	return uByte;
}

string
CSerial::ReadLine(void)
{
	string strToRet;
	while( 1 )
	{
		char ch = Read();
		if( ch == '\n' || ch == '\r' )
		{
//			strToRet += ch;
			return strToRet;
		}
		else
		{
			strToRet += ch;
		}
	}
}

unsigned int
CSerial::GetReady(void)
{
	int bytes = 0;

	ioctl(fdSerialDevice, FIONREAD, &bytes);

	return (unsigned int)bytes;

}

int 
CSerial::Read_NonBlock(unsigned char *rgBuffer, int nToRead)
{
	int current_settings = fcntl( fdSerialDevice, F_GETFL );

	fcntl( fdSerialDevice, F_SETFL, current_settings | O_NONBLOCK );
	int to_ret = read( fdSerialDevice, rgBuffer, nToRead );
	fcntl( fdSerialDevice, F_SETFL, current_settings );

	return to_ret;
}

void
CSerial::Write(unsigned char u8Byte)
{
	write(fdSerialDevice, &u8Byte, 1);
	//tcdrain(fdSerialDevice);
}

size_t
CSerial::Write(const unsigned char *rgu8Bytes, const unsigned int nLength)
{
	//Flush();
	//FlushI();
	return write(fdSerialDevice, (void *)rgu8Bytes, nLength);
	//tcdrain(fdSerialDevice);
}

void
CSerial::Flush(void)
{
	//tcflush(fdSerialDevice, TCIFLUSH);
	//tcflush(fdSerialDevice, TCOFLUSH);
	tcflush(fdSerialDevice, TCIOFLUSH);
}

void
CSerial::FlushI(void)
{
	tcflush(fdSerialDevice, TCIFLUSH);
}

void
CSerial::FlushO(void)
{
	tcflush(fdSerialDevice, TCOFLUSH);
}



