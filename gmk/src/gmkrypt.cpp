/*
 *	gmkrypt.cpp
 *	GMKrypt
 */

#include <gmkrypt.hpp>

namespace Gmk
{
	Gmkrypt::Gmkrypt(int _seed)
		: seed(_seed)
	{
		GenerateSwapTable();
	}

	Gmkrypt::~Gmkrypt()
	{

	}

	void Gmkrypt::GenerateSwapTable()
	{
		int a = (seed % 250) + 6;
		int b = seed / 250;

		for(int i = 0; i < 256; i++)
			table[0][i] = i;

		for(int i = 1; i < 10001; i++)
		{
			int j = ((i * a + b) % 254) + 1;
			
			int t = table[0][j];
			table[0][j] = table[0][j + 1];
			table[0][j + 1] = t;
		}

		table[1][0] = 0;

		for(int i = 1; i < 256; ++i)
			table[1][table[0][i]] = i;
	}

	Stream* Gmkrypt::Encrypt(Stream* stream)
	{
		return NULL;
	}

	Stream* Gmkrypt::Decrypt(Stream* stream)
	{
		Stream* value = new Stream();

		unsigned int c = stream->GetPosition() + 1;

		value->WriteByte(stream->ReadByte());
		while(stream->GetPosition() < stream->GetLength())
		{
			Stream::StreamBuffer buffer(512);
			stream->ReadData(buffer);

			for(int i = 0; i < 512; ++i)
				buffer[i] = (table[1][(int)buffer[i]] - c++) & 0xFF;

			value->WriteData(buffer);
		}

		value->Rewind();

		return value;
	}

	int Gmkrypt::ReadSeedFromJunkyard(Stream* stream)
	{
		unsigned int bill = stream->ReadDword();
		unsigned int fred = stream->ReadDword();

		stream->SetPosition(stream->GetPosition() + bill * sizeof(unsigned int));
		int value = stream->ReadDword();
		stream->SetPosition(stream->GetPosition() + fred * sizeof(unsigned int));

		return value;
	}
}
