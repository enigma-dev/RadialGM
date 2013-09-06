/*
 *	stream.cpp
 *	Binary file stream helper
 */

#include <cstdlib>
#include <cstring>
#include <stream.hpp>
#include <zlib.h>

namespace Gmk
{
	Stream::Stream(const std::string& filename, StreamMode mode)
		: fileStream(), buffer(), streamMode(mode), position(0)
	{
		if (streamMode == SmMemory)
			return;

		std::fstream::openmode openMode = std::fstream::binary;
		if (streamMode == SmRead)
			openMode |= std::fstream::in;
		else
			openMode |= std::fstream::out;

		fileStream.open(filename.c_str(), openMode);

		if (!fileStream)
			throw StreamException("Cannot open file for streaming!");
	}

	Stream::Stream()
		: fileStream(), buffer(), streamMode(SmMemory), position(0)
	{

	}

	Stream::~Stream()
	{
		if (streamMode != SmMemory)
			fileStream.close();
	}

	void Stream::ExpandMemoryStream(std::size_t length)
	{
		if (streamMode != SmMemory)
			return;

		if (position + length < buffer.size())
			return;

		if (position + length > buffer.size() + length)
			buffer.resize(position + length, 0);
		else
			buffer.resize(buffer.size() + length, 0);
	}

	unsigned char* Stream::DeflateStream(const StreamBuffer& sourceBuffer, std::size_t *deflatedLength)
	{
		uLongf destSize = static_cast<uLongf>(sourceBuffer.size() * 1.001f + 12);
		Bytef* destBuffer = new Bytef[destSize];

		// Deflate
		int result = compress2(destBuffer, &destSize, sourceBuffer.data(), sourceBuffer.size(), Z_BEST_SPEED);
		if (result != Z_OK)
		{
			*deflatedLength = 0;
			return NULL;
		}

		*deflatedLength = (size_t)destSize;

		return destBuffer;
	}

	unsigned char* Stream::InflateStream(const StreamBuffer& sourceBuffer, std::size_t *inflatedLength)
	{
		unsigned int destBufferSize = sourceBuffer.size();
		unsigned char* destBuffer = new unsigned char[destBufferSize];
		z_stream stream;
		int result;

		std::memset(&stream, 0, sizeof(z_stream));

		stream.next_in			= const_cast<Bytef*>(sourceBuffer.data());
		stream.avail_in			= sourceBuffer.size();
		stream.next_out			= destBuffer;
		stream.avail_out		= destBufferSize;

		// Begin inflating
		inflateInit(&stream);

		// Inflate blocks
		while(stream.avail_in && !(result = inflate(&stream, 0)))
		{
			int offset = stream.next_out - destBuffer;

			destBufferSize += 2048;
			stream.avail_out += 2048;

			destBuffer = static_cast<unsigned char*>(std::realloc(destBuffer, destBufferSize));
			if (destBuffer == NULL)
				return NULL;

			stream.next_out = destBuffer + offset;
		}

		if (result != Z_STREAM_END)
		{
			delete[] destBuffer;
			return NULL;
		}

		// Trim excess memory allocated & end inflation
		destBufferSize = stream.total_out;
		destBuffer = static_cast<unsigned char*>(std::realloc(destBuffer, destBufferSize));
		inflateEnd(&stream);

		*inflatedLength = destBufferSize;

		return destBuffer;
	}

	void Stream::Rewind()
	{
		if (streamMode == SmMemory)
			position = 0;
		else
		{
			fileStream.seekg(std::fstream::beg);
			fileStream.seekp(std::fstream::beg);
		}
	}

	void Stream::SetPosition(std::size_t value)
	{
		switch(streamMode)
		{
			case SmMemory:
				position = value;
				break;

			case SmRead:
				fileStream.seekg(value, std::fstream::beg);
				break;

			case SmWrite:
				fileStream.seekp(value, std::fstream::beg);
				break;
		}
	}

	std::size_t Stream::GetPosition()
	{
		switch(streamMode)
		{
			case SmMemory:
				return position;

			case SmRead:
				return static_cast<std::size_t>(fileStream.tellg());

			case SmWrite:
				return static_cast<std::size_t>(fileStream.tellp());
		}

		return 0;
	}

	std::size_t Stream::GetLength()
	{
		std::fstream::pos_type length = 0;

		if (streamMode == SmMemory)
			return buffer.size();

		if (streamMode == SmRead)
		{
			std::fstream::pos_type position = fileStream.tellg();
			fileStream.seekg(0, std::fstream::end);
			length = fileStream.tellg();
			fileStream.seekg(position, std::fstream::beg);
		}
		else
		{
			std::fstream::pos_type position = fileStream.tellp();
			fileStream.seekp(0, std::fstream::end);
			length = fileStream.tellp();
			fileStream.seekp(position, std::fstream::beg);
		}

		return static_cast<std::size_t>(length);
	}

	const Stream::StreamBuffer& Stream::GetMemoryBuffer() const
	{
		return buffer;
	}

	const unsigned char* Stream::GetRawMemoryBuffer() const
	{
		if (streamMode != SmMemory)
			return NULL;

		return buffer.data();
	}

	void Stream::ReadData(const StreamBuffer& value)
	{
		if (streamMode == SmMemory)
		{
			unsigned int size = std::max(static_cast<int>(value.size()), static_cast<int>(value.size() - buffer.size()));
			std::memcpy((void*)value.data(), buffer.data() + position, size);
			position += value.size();
		}
		else
			fileStream.read(reinterpret_cast<char*>(const_cast<unsigned char*>(value.data())), value.size());
	}

	bool Stream::ReadBoolean()
	{
		return ReadDword() >= 1;
	}
	
	unsigned char Stream::ReadByte()
	{
		unsigned char value = 0;
	
		if (streamMode == SmMemory)
		{
			value = *(unsigned char*)(buffer.data() + position);
			position += sizeof(unsigned char);
		}
		else
			fileStream.read(reinterpret_cast<char*>(&value), sizeof(unsigned char));

		return value;
	}

	unsigned short Stream::ReadWord()
	{
		unsigned short value = 0;

		if (streamMode == SmMemory)
		{
			value = *(unsigned short*)(buffer.data() + position);
			position += sizeof(unsigned short);
		}
		else
			fileStream.read(reinterpret_cast<char*>(&value), sizeof(unsigned short));

		return value;
	}

	unsigned int Stream::ReadDword()
	{
		unsigned int value = 0;

		if (streamMode == SmMemory)
		{
			value = *(unsigned int*)(buffer.data() + position);
			position += sizeof(unsigned int);
		}
		else
			fileStream.read(reinterpret_cast<char*>(&value), sizeof(unsigned int));

		return value;
	}

	unsigned long long Stream::ReadQword()
	{
		unsigned long long value = 0;

		if (streamMode == SmMemory)
		{
			value = *(unsigned long long*)(buffer.data() + position);
			position += sizeof(unsigned long long);
		}
		else
			fileStream.read(reinterpret_cast<char*>(&value), sizeof(unsigned long long));

		return value;
	}

	double Stream::ReadDouble()
	{
		double value = 0.0;

		if (streamMode == SmMemory)
		{
			value = *(double*)(buffer.data() + position);
			position += sizeof(double);
		}
		else
			fileStream.read(reinterpret_cast<char*>(&value), sizeof(double));

		return value;
	}

	float Stream::ReadFloat()
	{
		float value = 0.0f;

		if (streamMode == SmMemory)
		{
			value = *(float*)(buffer.data() + position);
			position += sizeof(float);
		}
		else
			fileStream.read(reinterpret_cast<char*>(&value), sizeof(float));

		return value;
	}

	std::string Stream::ReadString()
	{
		unsigned int length = ReadDword();

		if (streamMode == SmMemory)
		{
			std::string value(reinterpret_cast<const char*>(buffer.data() + position), length);
			position += length;

			return value;
		}
		else
		{
			std::string value(length, '\0');
			fileStream.read(const_cast<char*>(value.data()), length);

			return value;
		}

		return "";
	}

	time_t Stream::ReadTimestamp()
	{
		return (time_t)(86400.0 * ReadDouble() + GmTimestampEpoch);
	}

	Stream* Stream::ReadBitmap()
	{
		if (!ReadBoolean())
			return NULL;

		return Deserialize();
	}

	Stream* Stream::ReadBitmapOld()
	{
		if (ReadDword() == -1)
			return NULL;

		return Deserialize(false);
	}

	void Stream::WriteData(const StreamBuffer& value)
	{
		if (streamMode == SmMemory)
		{
			ExpandMemoryStream(value.size());
			std::memcpy(buffer.data() + position, value.data(), value.size());
			position += value.size();
		}
		else
			fileStream.write(reinterpret_cast<const char*>(value.data()), value.size());
	}

	void Stream::WriteBoolean(bool value)
	{
		WriteDword(value ? 1 : 0);
	}

	void Stream::WriteByte(unsigned char value)
	{
		if (streamMode == SmMemory)
		{
			ExpandMemoryStream(sizeof(unsigned char));
			*(unsigned char*)(buffer.data() + position) = value;
			position += sizeof(unsigned char);
		}
		else
			fileStream.write(reinterpret_cast<const char*>(&value), sizeof(unsigned char));
	}

	void Stream::WriteWord(unsigned short value)
	{
		if (streamMode == SmMemory)
		{
			ExpandMemoryStream(sizeof(unsigned short));
			*(unsigned short*)(buffer.data() + position) = value;
			position += sizeof(unsigned short);
		}
		else
			fileStream.write(reinterpret_cast<const char*>(&value), sizeof(unsigned short));
	}

	void Stream::WriteDword(unsigned int value)
	{
		if (streamMode == SmMemory)
		{
			ExpandMemoryStream(sizeof(unsigned int));
			*(unsigned int*)(buffer.data() + position) = value;
			position += sizeof(unsigned int);
		}
		else
			fileStream.write(reinterpret_cast<const char*>(&value), sizeof(unsigned int));
	}

	void Stream::WriteQword(unsigned long long value)
	{
		if (streamMode == SmMemory)
		{
			ExpandMemoryStream(sizeof(unsigned long long));
			*(unsigned long long*)(buffer.data() + position) = value;
			position += sizeof(unsigned long long);
		}
		else
			fileStream.write(reinterpret_cast<const char*>(&value), sizeof(unsigned long long));
	}

	void Stream::WriteDouble(double value)
	{
		if (streamMode == SmMemory)
		{
			ExpandMemoryStream(sizeof(double));
			*(double*)(buffer.data() + position) = value;
			position += sizeof(double);
		}
		else
			fileStream.write(reinterpret_cast<const char*>(&value), sizeof(double));
	}

	void Stream::WriteFloat(float value)
	{
		if (streamMode == SmMemory)
		{
			ExpandMemoryStream(sizeof(float));
			*(float*)(buffer.data() + position) = value;
			position += sizeof(float);
		}
		else
			fileStream.write(reinterpret_cast<const char*>(&value), sizeof(float));
	}

	void Stream::WriteString(const std::string& value)
	{
		WriteDword(value.length());

		for(std::size_t i = 0; i < value.length(); ++i)
			WriteByte(value[i]);
	}

	void Stream::WriteTimestamp()
	{
		WriteDouble((double)(time(NULL) - GmTimestampEpoch) / 86400.0);
	}

	void Stream::WriteBitmap(Stream* value)
	{
		WriteBoolean(value != NULL);
		
		if (value != NULL)
			Serialize(value);
	}

	void Stream::WriteBitmapOld(Stream* value)
	{
		WriteDword(value == NULL ? -1 : 0x10);
		
		if (value != NULL)
			Serialize(value, false);
	}

	void Stream::Deflate()
	{
		std::size_t deflatedLength = 0;

		unsigned char* deflatedBuffer = DeflateStream(buffer, &deflatedLength);
		if (deflatedLength == 0)
		{
			delete[] deflatedBuffer;
			return;
		}

		buffer.resize(deflatedLength);
		std::memcpy(buffer.data(), deflatedBuffer, deflatedLength);
		position = 0;

		delete[] deflatedBuffer;
	}
	
	void Stream::Inflate()
	{
		std::size_t inflatedLength = 0;

		unsigned char* inflatedBuffer = InflateStream(buffer, &inflatedLength);
		if (inflatedLength == 0)
		{
			delete[] inflatedBuffer;
			return;
		}

		buffer.resize(inflatedLength);
		std::memcpy(buffer.data(), inflatedBuffer, inflatedLength);
		position = 0;

		delete[] inflatedBuffer;
	}

	Stream* Stream::Deserialize(bool decompress)
	{
		Stream* value = new Stream();

		unsigned int length = ReadDword();
		StreamBuffer tmpBuffer(length, 0);
		ReadData(tmpBuffer);
		value->WriteData(tmpBuffer);
		value->Rewind();

		if (decompress)
			value->Inflate();

		return value;
	}

	void Stream::Serialize(Stream* stream, bool compress)
	{
		if (stream != NULL)
		{
			if (compress)
				stream->Deflate();

			WriteDword(stream->GetLength());
			WriteData(stream->GetMemoryBuffer());
		}
		else
			WriteDword(0);
	}

	StreamException::StreamException(const std::string& _message)
		: message(_message)
	{

	}

	StreamException::~StreamException() throw()
	{

	}

	const char* StreamException::what() const throw()
	{
		return message.c_str();
	}
}
