/*!\file  UUcoder.cxx  UU encoding handling
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

#include "UUcoder.h"
#include <bitset>
#include <iostream>
#include <fstream>

using namespace std;

#define MAX_LINE_BYTES 46

string 
CUUcoder::UUEncode(const unsigned char *rgBinaryData, unsigned int nDataLen)
{
	string strToRet;


	unsigned int nProperRounds = nDataLen / 3;
	unsigned int nBytesLeft    = nDataLen - nProperRounds*3; //nDataLen % 3;
	//unsigned int nToAdd	  = 3 - nBytesLeft;

	//cout << "PROPER: " << nProperRounds << " BYTES: " << nBytesLeft << " FIRST: " << char(nDataLen + 0x20) << endl;

	strToRet += (nDataLen == 0 ? 0x60 : (nDataLen+0x20));

	unsigned int i,j,k;
	unsigned char rgBuffer[4];

	i=0;
	for(k=0; k<nProperRounds; k++)
	{
		// this may look nasty but its no such a big deal - read the specs in the header file
		rgBuffer[0] = (rgBinaryData[i] >> 2);
		rgBuffer[1] = (((rgBinaryData[i] & 0x03) << 4) | (rgBinaryData[i+1] >> 4));
		rgBuffer[2] = (((rgBinaryData[i+1] & 0x0F) << 2) | ((rgBinaryData[i+2] & 0xC0) >> 6));
		rgBuffer[3] = (rgBinaryData[i+2] & 0x3F);

		//cout << bitset<8>(rgBuffer[0]) << " " << bitset<8>(rgBuffer[1]) << " " << bitset<8>(rgBuffer[2]) << " " << bitset<8>(rgBuffer[3]) << " " << endl;

		for(j=0; j<4; j++)
		{
			// this is the exception - read the specs in the header file
			if( rgBuffer[j] == 0x00 )
				rgBuffer[j] += 0x60;
			else
				rgBuffer[j] += 0x20;

			strToRet += char( rgBuffer[j] ); 
			//cout << rgBuffer[j] << " " << i << " " << j << " " << strToRet.length() << endl;
		}

		i += 3;

	}

	if( !nBytesLeft ) return strToRet;

	rgBuffer[0] = (rgBinaryData[i] >> 2);
	if( i < nDataLen ) 
	{
		rgBuffer[1] = (((rgBinaryData[i] & 0x03) << 4) | (rgBinaryData[i+1] >> 4));

		rgBuffer[2] = (((rgBinaryData[i+1] & 0x0F) << 2) | ((int('\0') & 0xC0) >> 6));
		rgBuffer[3] = (int('\0') & 0x3F);
	}
	else
	{
		rgBuffer[1] = (((rgBinaryData[i] & 0x03) << 4) | (int('\0') >> 4));

		rgBuffer[2] = (((int('\0') & 0x0F) << 2) | ((int('\0') & 0xC0) >> 6));
		rgBuffer[3] = (int('\0') & 0x3F);
	}

	for(j=0; j<4; j++)
	{
		// this is the exception - read the specs in the header file
		if( rgBuffer[j] == 0x00 )
			rgBuffer[j] += 0x60;
		else
			rgBuffer[j] += 0x20;

		strToRet += char( rgBuffer[j] ); 
		//cout << rgBuffer[j] << " " << i << " " << j << " " << strToRet.length() << endl;
	}

	
	
		
	//nToAdd++;

	//cout << "ORIG: " << nDataLen << endl;
	//cout << "ENCD: " << strToRet.length() << endl;


	return strToRet;
}


unsigned int 
CUUcoder::UUDecode(unsigned char *rgBinaryData, string strToDecode, unsigned int nBufferLen)
{
	unsigned int i,j,k;
	unsigned char rgBuffer[4];
	unsigned char tmp;

	if(  strToDecode.length() % 4 != 1 ) { cerr << "ERROR: decoding string not 4*n + 1 (" << strToDecode.length() << ")" << endl; return 0; }
	if( (strToDecode.length() / 4) * 3 > nBufferLen ) { cerr << "ERROR: buffer not big enough!" << endl; return 0; }

	k=0;
	
	// strToDecode.length() must be a multiple of 4 from definition
	for(i=1; i<strToDecode.length(); i+=4)
	{
		// first copy in the chars and substract the constants
		for(j=0; j<4; j++)
		{
			tmp = strToDecode.at(i+j);
	  
			if( tmp == 0x60 )   //take care of the 'exception' byte
				tmp -= 0x60;
			else
			   	tmp -= 0x20;

			rgBuffer[j] = tmp;	
		}

		//cout << bitset<8>(rgBuffer[0]) << " " << bitset<8>(rgBuffer[1]) << " " << bitset<8>(rgBuffer[2]) << " " << bitset<8>(rgBuffer[3]) << " " << endl;

		rgBinaryData[k]   = (rgBuffer[0] << 2) | ((rgBuffer[1] & 0x18) >> 3);
		rgBinaryData[k+1] = ((rgBuffer[1] & 0x0F) << 4) | ((rgBuffer[2] & 0x3C) >> 2);
		rgBinaryData[k+2] = ((rgBuffer[2] & 0x03) << 6) | rgBuffer[3];

		k += 3;
	}

	return k;
}

string 
CUUcoder::UUEncodeFile(const string strFilePath, const string strOutputFileName)
{
	string strToRet;
	unsigned char *rgBuffer, *p_rgBuffer;
	long lSize;
 	FILE *pFile = NULL, *pOutFile = NULL;

	if(!(pFile = fopen(strFilePath.c_str(), "rb")))
		return strToRet;
	if(!(pOutFile = fopen(strOutputFileName.c_str(), "w")))
		return strToRet;

	//FILE* f1 = fopen(string strFilePath,"rb");

	fseek (pFile , 0 , SEEK_END);
	lSize = ftell (pFile);
	rewind (pFile);

	rgBuffer = new unsigned char[lSize];

	fread(rgBuffer,1,lSize,pFile);
	p_rgBuffer = rgBuffer;

	long lFullRounds = lSize / 45;

	for(int i=0; i<lFullRounds; i++)
	{
		string strLine = UUEncode( p_rgBuffer, 45 );
		//cout << strLine << endl;

		strLine += '\n';
		fwrite (strLine.c_str() , strLine.length() , sizeof(char) , pOutFile );

		p_rgBuffer += 45;
	}

	string strLastLine = UUEncode( p_rgBuffer, (lSize - (lFullRounds*45)) );

	strLastLine += '\n';
	fwrite (strLastLine.c_str() , strLastLine.length() , sizeof(char) , pOutFile );

	fclose(pFile);
	fclose(pOutFile);
	
	delete rgBuffer;

	return strToRet;
}

string 
CUUcoder::UUDecodeFile(const string strFilePath, const string strOutputFileName)
{
	string strToRet;
 	FILE *pOutFile = NULL;

	fstream filestr ( strFilePath.c_str() , fstream::in | fstream::out);
	
	if(!(pOutFile = fopen(strOutputFileName.c_str(), "wb")))
		return string("ERROR while opening output file!\n");

	string ss;
	unsigned char rgLineBuf[MAX_LINE_BYTES];
	int line = 0;
	while( filestr >> ss )
	{
		//if( ss.length() % 10 == 0 ){  cout <<  ss << " XXX: " << line << endl; }
		unsigned int tmp = UUDecode(rgLineBuf, ss, MAX_LINE_BYTES);
		cout << rgLineBuf << endl;
		fwrite(rgLineBuf, 1, tmp, pOutFile); 


		//cout << "SS: " << ss << endl;
		line++;
	}

	fclose(pOutFile);

	return strToRet;
}


