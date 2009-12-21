/*!\file  CFirmwareHEX32.cxx  32-bit hex file handling
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

#include <firmware/CFirmwareHEX32.h>
#include <stdlib.h>
#include <sstream>
#include <stdio.h>
#include <core/defs.h>

CFirmwareHEX32::CFirmwareHEX32()
{
	m_bFileOpen = false;
	m_nEIP = 0;
	m_nULBA = 0;
}

/*
* Simply opens the firmware file
*/
bool 
CFirmwareHEX32::OpenFirmware(const char * pszPathName)
{

	m_clInputFile.open( pszPathName );

    if( !m_clInputFile.good() )
        cout << "File " << pszPathName << " is not good()..." << endl;

	m_strLastFileName = pszPathName;

	m_bFileOpen = m_clInputFile.is_open();

	return m_bFileOpen;
}

bool
CFirmwareHEX32::ChecksumSupported()
{
	//intel hex 32 supports checksums so we just return true
	return true;
}

/*
* If the actual firmware file supports checksums (ChecksumSupported==true) this does the 
* checking and returns true if everything seems ok, false otherwise
*/
bool 
CFirmwareHEX32::CheckFirmware(bool bVerbose)
{
	const int _lineBufferSize = 2048;
	char _lineBuffer[_lineBufferSize];
	unsigned long _lineNum=0;

	//cout << "Checking firmware..." << endl;

    ifstream clTempFile;
    clTempFile.open( m_strLastFileName.c_str() );

	// return if file not open
	if( !clTempFile.is_open() )
		return false;
		
	while(clTempFile.getline(_lineBuffer, _lineBufferSize))
	{
		stringstream _ss(_lineBuffer);
		string _line;

		_ss >> _line;

		//if the first char isnt : - firmware is most likely not consistent (is broken)
		if( _line.at(0) != ':' )
			return false;

		unsigned char _checksum = 0;
		for(size_t i=1; i<_line.length(); i+=2){
			char _byte[2];
			
			_byte[0] = _line.at(i);
			_byte[1] = _line.at(i+1);

			unsigned int _byte_num;

			sscanf(_byte,"%02x", &_byte_num);
			_checksum += (unsigned char)_byte_num;
		}

		if( (int)_checksum == 0 )
		{
			if( bVerbose )
				cout << '.';
		}
		 else
		{
			cerr << ERRSTR << "Checksum error on line " << _lineNum << " " << _line << endl;
			return false;
 		}

		_lineNum++;
	}

	//close the file
	clTempFile.close();

	if( bVerbose )
		cout << endl;
	//cout << endl << "Done checking firmware - everything OK" << endl;

	return true;
}

/*
* This function returns the next address and the data supposed to be stored
* at this address in memory of the chip
* By next i mean the next address according to the current possition in the
* file being processed - this can be usefull so we can process the file nicely
* in some loop or someting like that.
*/
bool 
CFirmwareHEX32::GetNextAdrData(uint32_t & u32Adr, uint32_t * pu32Data, uint32_t & cData, bool bVerbose)
{
	const int _lineBufferSize = 2048;
	char _lineBuffer[_lineBufferSize];

	//we are probably at the end of the file so reset it and return false
	if( !m_clInputFile.getline(_lineBuffer, _lineBufferSize) )
	{
		//reset the file
		//seekg could be used - see http://www.cplusplus.com/reference/iostream/istream/seekg.html
		m_clInputFile.close();
		m_clInputFile.open( m_strLastFileName.c_str() );
		m_bFileOpen = m_clInputFile.is_open();
		return false;
	}

	stringstream _ss(_lineBuffer);
	string _line;
		
	_ss >> _line;
	
	uint32_t _reclen=0,
			 _loadOffset=0,
			 _rectyp=0; 
	
	//1 ascii pair startint at position 1 -> after ':' character (1B) == data length
	sscanf(_line.substr(1,2).c_str(), "%02x", &_reclen);		
	//next 2 ascii pairs (2B) == actual address - this is not all yet as 2B == 16b...
	sscanf(_line.substr(3,4).c_str(), "%04x", &_loadOffset);	
	//next 1 ascii pair  (1B)
	sscanf(_line.substr(7,2).c_str(), "%02x", &_rectyp);		

	_line = _line.substr(9, _line.length()-9);

	//now we have only raw data in ascii pairs in the _line
	//except the last pair which is the checkum byte
	//1 pair = 2 ascii chars so this equation is right
	if( (_line.length() - 2) != _reclen*2 )
	{
		cerr << "ERROR: Something wrong with the " << m_strLastFileName << " file... closing!" << endl;
		return false;
		
	} 
	 else 
	{
		// this is not datarecord ... TODO: not handled yet
		if( _rectyp != RECTYP_DATAREC )
		{
			switch(_rectyp)
			{
				case RECTYP_EXTENDED_LIN_AR:
					sscanf(_line.substr(0,4).c_str(), "%04x", &m_nULBA);
					m_nULBA <<= 16; //upper linear base address
					if( bVerbose )
						printf("ULBA: 0x%08x\n", m_nULBA);
					cData = 0;
					return true;
				case RECTYP_ENDREC:
					if( bVerbose )
						cout << "End of file " << m_strLastFileName << " reached." << endl;
					return false;
				case RECTYP_START_LIN_AR:
					sscanf(_line.substr(0,8).c_str(), "%08x", &m_nEIP);
					if( bVerbose )
						printf("EIP:  0x%08x\n", m_nEIP);
					cData = 0;
					return true;
				default:
					cout << "ERROR: Unsupported record type flag!" << endl;
					return false;
			}
		}

		uint32_t _cBytesLeft = _reclen;
		
		if( _reclen > cData )
		{
			cerr << ERRSTR << "Buffer not big enough!" << endl;
			return false;
		}

		
		while(_cBytesLeft)
		{
			//uint32_t cur_byte;
			sscanf(_line.substr(0,2).c_str(), "%02x", &(pu32Data[ _reclen - _cBytesLeft ]));
			//sscanf(_line.substr(0,2).c_str(), "%02x", &cur_byte );
			_line = _line.substr(2, _line.length()-2);

			//pu32Data[ (_reclen - _cBytesLeft) / 4 ] |= cur_byte;
			//pu32Data[ (_reclen - _cBytesLeft) / 4 ] <<= 8;

			_cBytesLeft--;
		}
	}

	u32Adr = _loadOffset | m_nULBA;
	cData  = _reclen;

	return true;
}
