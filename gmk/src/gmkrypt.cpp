/**
* @file gmkrypt.cpp
* @brief GMKrypt
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

#include <gmkrypt.h>

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
