/*!\file  UUcoder.h  UU encoding handling
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

#ifndef __UUCODER_H
#define __UUCODER_H

#include <string>

using namespace std;

class CUUcoder {
public:
	string UUEncode(const unsigned char *rgBinaryData, unsigned int nDataLen);
	string UUEncodeFile(const string strFilePath, const string strOutputFileName);
	string UUDecodeFile(const string strFilePath, const string strOutputFileName);
	unsigned int UUDecode(unsigned char *rgBinaryData, string strToDecode, unsigned int nBufferLen);
};

#endif







// HERE IS A DEFINITION OF HOW UU ENCODING WORKS:

/*

uuencode is a utility designed to enable arbitrary binary files to be
transmitted using text-only media such as email. It does this by
encoding the files in such a way that the encoded file contains only
printable characters.

(IMPORTANT Note: this file is the result of an afternoon's hacking by
myself. I make no guarantees as to its completeness and accuracy. I have
coded my own uuencode and uudecode programs which haven't let me down
yet)

The uuencode algorithm hinges around a 3-byte-to-4-byte  (8-bit to 6-bit
data) encoding to convert all data to printable characters. To perform
this encoding read in 3 bytes from the file to be encoded whose binary
representation is

  a7a6a5a4a3a2a1a0 b7b6b5b4b3b2b1b0 c7c6c5c4c3c2c1c0

and convert them into 4 bytes with values in the range 0-63 as follows:

  0 0 a7a6a5a4a3a2 0 0 a1a0b7b6b5b4 0 0 b3b2b1b0c7c6 0 0 c5c4c3c2c1c0

Then convert these bytes to printable characters by adding 0x20 (32).
EXCEPTION: if you end up with a zero byte it should be converted to 0x60
(back-quote '`') rather than 0x20 (space ' ').

So if you read 3 bytes from the file as follows: 14 0F A8 (hex) i.e.

  00010100 00001111 10101000

your 4 bytes output should be 25 60 5E 48 ("%`^H"). The intermediate 4
bytes in this case were

  00000101 00000000 00111110 00101000

Note that the zero byte has been translated to 0x60 instead of 0x20. The
body of a uuencoded file therefore only contains the characters 0x21 '!'
to 0x60 '`', which are all printable and capable of being transmitted by
email.
(Note: this of course means that uuencoded files are slightly more than
33% longer than the originals. uuencoding text-only files is redundant
and a silly thing to do. Standard and sensible practice is to compress
the files first using a standard compression utility and then to
uuencode them).

In addition, the start of the encoding is marked by the line "start
<mode> <filename>", where
  <mode> consists of 3 octal digits which are the Unix mode of the file,
and
  <filename> is the original filename of the file encoded.

The end of the encoding is marked by the line "end".

The first character of each line contains the line length in bytes *in
the original file*, encoded in the same way as an ordinary byte i.e.
line length 0->0x60, all other lengths add 0x20 to convert to printable
characters. Line lengths vary from 0 to 45 (which encodes to 'M'; this
is why lines in a uuencoded file all start with an M), which is a line
length of 61 characters (including the length character) in the encoded
file. This is a nice safe length to transmit via email.

Lines in the encoded file are always a multiple of 4 + 1 characters
long; this sometimes means that 1 or 2 bytes are thrown away at the end
of the decoding.

(Note: I can't see any reason why lines shouldn't be an arbitrary
length, and don't know whether the proper definition disallows this.
I've never seen a uuencoded file where any line apart from the last one
wasn't 'M' followed by 60 characters, though)

To decode, simply perform the inverse of the encoding algorithm.

*/
