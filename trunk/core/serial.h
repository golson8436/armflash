/*!\file  serial.cxx  Classes to handle serial communication
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

#ifndef __SERIAL_H
#define __SERIAL_H

#include <termios.h>
#include <core/defs.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <unistd.h>
#include <sys/ioctl.h>

using namespace std;

//#ifndef DEV_PREFIX
//	#define DEF_PREFIX "/dev/" 
//#endif

//! Represents an unitialized file descriptor.
#define BAD_DEVICE -1

/**
*\fn serial_autodetect(void)
*\author Gabriel Zabusek
*\brief Autodetects the available serial ports on the system.
*
*@return The amount of active serial ports detected.
*@see CSerial
*
* This function autodetects all available serial ports on the system, this includes the ports
* directly put on the motherboard and also external - for instance usb - serial ports.
*/
extern int serial_autodetect(void);

/**
*\class CSerial
*\author Gabriel Zabusek
*\brief Handles the serial port communication.
*
* This class handles all the serial communication. It is pretty much a wrapper
* above the standard unix termios API designed in a way so that it is easy to 
* use it in our project.
*/

class CSerial {

	private:
		//! File descriptor of currently open serial port
		int fdSerialDevice;

		//! Termios structures used for the opened serial port
		struct termios stTioOld,
                       stTioNew;

		//! Zero terminated string for the serial device
		const char *pszDeviceName;

		//! Baud rate used to communicate with the currently open serial device
		speed_t stBaudRate;

	public:

		/**
		*\brief CSerial constructor
		*@param _pszDeviceName The device to open, for instance /dev/ttyS0
		*@param _stBaudRate The baud rate used to communicate with this device
		*
		* Simply sets the device we want to work with and the baud rate we want to use
		* to communicate with this device.
		*/
		CSerial(const char *_pszDeviceName, speed_t _stBaudRate)
			: pszDeviceName(_pszDeviceName), stBaudRate(_stBaudRate) 
		{
			fdSerialDevice = BAD_DEVICE;
		};

		/// Destructor, does nothing...
		~CSerial(void) {};

		/**
		*\brief Initializes the serial port.
		*@return FAILURE if error occures and SUCCESS if everything went fine.
		*
		* Initializes the serial port with speed given in the constructor.
		*@see CSerial(const char *_pszDeviceName, speed_t _stBaudRate)
		*/
		int Init(void);

		/**
		*\brief Restores the original serial port settings.
		*@return FAILURE on error SUCCESS if everything goes ok.
		*
		* Restores the original serial port settings and cleans the serial line.
		*/
		int Restore(void);

		/**
		*\brief Open serial device for reading and writing.
		*@return FAILURE on error SUCCESS if everything goes ok.
		*
		* Open serial device for reading and writing, however not as controlling tty
		* since we dont want to get killed if we recieve ctrl+c character.
		*/
		int Open(void);

		//! Closes the serial port.
		void Close(void);

		//! Flushes the serial port.
		void Flush(void);
		void FlushI(void);
		void FlushO(void);		

		/**
		*\brief Reads one byte from the serial line.
		*@return The byte read.
		*
		* Reads one byte from the serial line, note that this function blocks the 
		* current thread until there is some byte/character available on the line.
		*/
		unsigned char Read(void);

		string ReadLine(void);
		unsigned int GetReady(void);

		/**
		*\brief Reads nToRead bytes from the serial line.
		*
		* Reads nToRead bytes from the serial line, note that this function doesnt block
		*
		*@return The number of bytes actually read		
		*@param rgBuffer the output buffer
		*@param nToRead number of bytes to read
		*/
		int Read_NonBlock(unsigned char *rgBuffer, int nToRead);

		/**
		*\brief Writes one byte to the serial line.
		*@param u8Byte The byte to be written.
		*/
		void Write(unsigned char u8Byte);

		/**
		*\brief Writes an array of bytes to the serial line.
		*@param pu8Byte The array byte to be written.
		*@param nLength The length of the given array.
		*/
		size_t Write(const unsigned char *rgu8Bytes, const unsigned int nLength);

        /**
		*\brief Returns array of available serial ports on the system.
		*
		* Returns vector of strings containing names of available serial port devices.
		*
		*@return Vector of strings containing names of available serial port devices
		*/
		static vector<string> SerialAutodetect()
        {
                vector<string> clToRet;
                char * rgDevices[] = {
                    "/dev/ttyS0", //Linux
                    "/dev/ttyS1",
                    "/dev/ttyS2",
                    "/dev/ttyS3",
                    "/dev/tty00", // OpenBSD / Digital UNIX
                    "/dev/tty01",
                    "/dev/tty02",
                    "/dev/tty03",
                    "/dev/ttyU0",
                    "/dev/ttyU1",
                    "/dev/ttyU2",
                    "/dev/ttyU3",
                    "/dev/ttyUSB0", //Linux
                    "/dev/ttyUSB1",
                    "/dev/ttyUSB2",
                    "/dev/ttyUSB3",
                    "/dev/ttyf0", // IRIX
                    "/dev/ttyf1",
                    "/dev/ttyf2",
                    "/dev/ttyf3",
                    "/dev/tty1p0", // HP-UX
                    "/dev/tty2p0",
                    "/dev/tty3p0",
                    "/dev/tty3p0",
                    "/dev/ttya", // Sun-OS/Solaris
                    "/dev/ttyb",
                    "/dev/ttyc",
                    "/dev/ttyd"
                };
                char rgPipeBuffer[4096];
                vector<string> clPossiblePorts(rgDevices, rgDevices + (sizeof(rgDevices) / sizeof(*rgDevices)) );
                FILE *fd;
                string strPipeResult;
                bool bSttyArgument = false; // false for -f true for -F

                // first we must find out if we want to execute stty -f OR stty -F instead... :*(
                fd = popen("stty -f /dev/ttyS0 2>&1", "r");
                if (fd == NULL)
                {
                    cerr << "Error opening pipe while detecting serial ports!" << endl;
                    return clToRet;                    
                }

                while (fgets(rgPipeBuffer, 4096, fd) != NULL)
                {
                    strPipeResult += rgPipeBuffer;
                }
                if( pclose(fd) == -1 )
                {
                    cout << "Pipe closing error!" << endl;
                    return clToRet;
                }

                //cout << endl << "::" << strPipeResult << endl << "::" << endl;

                // if the result contains 'invalid' we must use -F
                if( strPipeResult.find( "invalid" ) != string::npos )
                    bSttyArgument = true;
                else
                    bSttyArgument = false;

                string strCmdStart, strCmdEnd;
                
                if( bSttyArgument )
                    strCmdStart = "stty -F ";
                else
                    strCmdStart = "stty -f ";

                strCmdEnd = " 2>&1";

                for(unsigned int i=0; i<clPossiblePorts.size(); i++)
                {
                    string strCurrentCommand = strCmdStart + clPossiblePorts[i] + strCmdEnd;
                    strPipeResult = "";

                    fd = popen(strCurrentCommand.c_str(), "r");
                    if (fd == NULL)
                    {
                        cerr << "Error opening pipe while detecting serial ports!" << endl;
                        return clToRet;                    
                    }

                    while (fgets(rgPipeBuffer, 4096, fd) != NULL)
                    {
                        strPipeResult += rgPipeBuffer;
                    }

                    if( pclose(fd) == -1 )
                    {
                        cout << "Pipe closing error!" << endl;
                        return clToRet;
                    }

                    if( (strPipeResult.find("speed") != string::npos)  || 
                        (strPipeResult.find("baud") != string::npos)   ||
                        (strPipeResult.find("ispeed") != string::npos) ||
                        (strPipeResult.find("ospeed") != string::npos) 
                      )
                    {
                        clToRet.push_back( clPossiblePorts[i] );
                    }
                }
          
                return clToRet;
        }
};

#endif
