/**
* @file  stream.h
* @brief Binary file stream helper
*
* @section License
*
* Copyright (C) 2013 Zachary Reedy
* This file is a part of the LateralGM IDE.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef __GMK_STREAM_H
#define __GMK_STREAM_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>

namespace Gmk
{
	#define BuildByte(a, b, c, d, e, f, g, h)			((static_cast<unsigned int>((a) & 0x01) << 7) | \
														(static_cast<unsigned int>((b) & 0x01) << 6) | \
														(static_cast<unsigned int>((c) & 0x01) << 5) | \
														(static_cast<unsigned int>((d) & 0x01) << 4) | \
														(static_cast<unsigned int>((e) & 0x01) << 3) | \
														(static_cast<unsigned int>((f) & 0x01) << 2) | \
														(static_cast<unsigned int>((g) & 0x01) << 1) | \
														(static_cast<unsigned int>((h) & 0x01) << 0))

	#define GetBit(a, b)								(((a) & (1 << (b))) != 0)

	#define BuildColor(r, g, b)							(0x00000000 | \
														  ((b) << 16) | \
														  ((g) << 8)  | \
														  (r))

	#define ColorGetRed(x)								((x) & 0xFF)
	#define ColorGetGreen(x)							(((x) >> 8) & 0xFF)
	#define ColorGetBlue(x)								(((x) >> 16) & 0xFF)

	class StreamException;

	class Stream
	{
	public:
		typedef std::vector<unsigned char> StreamBuffer;
		typedef enum _StreamMode
		{
			SmRead,
			SmWrite,
			SmMemory
		} StreamMode;

	private:
		static const unsigned long long GmTimestampEpoch = 0xFFFFFFFF7C5316BFULL;
		std::fstream fileStream;
		StreamBuffer buffer;
		StreamMode streamMode;
		std::size_t position;

		void ExpandMemoryStream(std::size_t length);

		// Compression
		unsigned char* DeflateStream(const StreamBuffer& sourceBuffer, std::size_t *deflatedLength);
		unsigned char* InflateStream(const StreamBuffer& sourceBuffer, std::size_t *inflatedLength);

	public:
		Stream(const std::string& filename, StreamMode mode);
		Stream();
		~Stream();

		// Operations
		void Rewind();
		void SetPosition(std::size_t value);
		std::size_t GetPosition();
		std::size_t GetLength();
		const StreamBuffer& GetMemoryBuffer() const;
		const unsigned char* GetRawMemoryBuffer() const;

		// Reading
		void ReadData(const StreamBuffer& value);
		bool ReadBoolean();	
		unsigned char ReadByte();
		unsigned short ReadWord();
		unsigned int ReadDword();
		unsigned long long ReadQword();
		double ReadDouble();
		float ReadFloat();
		std::string ReadString();
		time_t ReadTimestamp();
		Stream* ReadBitmap();
		Stream* ReadBitmapOld();

		// Writing
		void WriteData(const StreamBuffer& value);
		void WriteBoolean(bool value);
		void WriteByte(unsigned char value);
		void WriteWord(unsigned short value);
		void WriteDword(unsigned int value);
		void WriteQword(unsigned long long value);
		void WriteDouble(double value);
		void WriteFloat(float value);
		void WriteString(const std::string& value);
		void WriteTimestamp();
		void WriteBitmap(Stream* value);
		void WriteBitmapOld(Stream* value);
 
		// Compression
		void Deflate();
		void Inflate();

		// Serialization
		Stream* Deserialize(bool decompress = true);
		void Serialize(Stream* stream, bool compress = true);
	};

	class StreamException : public std::exception
	{
	private:
		std::string message;

	public:
		StreamException(const std::string& _message);
		~StreamException() throw();

		const char* what() const throw();
	};
}

#endif
