/**
* @file  gmkincludefile.cpp
* @brief GMK Include File
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

#include <gmkincludefile.h>

namespace Gmk
{
	IncludeFile::IncludeFile(GmkFile* gmk)
		: GmkResource(gmk),
		  filename(""),
		  filepath(""),
		  originalFile(false),
		  originalFileSize(0),
		  data(NULL),
		  exportKind(EkDont),
		  exportPath(""),
		  overwrite(false),
		  freeMemory(true),
		  removeAtEndOfGame(true)
	{
		exists = true;
	}

	IncludeFile::~IncludeFile()
	{
		if (data != NULL)
			delete data;
	}

	void IncludeFile::WriteVer81(Stream* stream)
	{
		Stream* includeFileStream = new Stream();

		includeFileStream->WriteTimestamp();
		includeFileStream->WriteDword(800);
		includeFileStream->WriteString(filename);
		includeFileStream->WriteString(filepath);
		includeFileStream->WriteBoolean(originalFile);
		includeFileStream->WriteDword(originalFileSize);

		if (data != NULL)
		{
			includeFileStream->WriteBoolean(true);
			includeFileStream->Serialize(data, false);
		}
		else
			includeFileStream->WriteBoolean(false);

		includeFileStream->WriteDword(exportKind);
		includeFileStream->WriteString(exportPath);
		includeFileStream->WriteBoolean(overwrite);
		includeFileStream->WriteBoolean(freeMemory);
		includeFileStream->WriteBoolean(removeAtEndOfGame);

		stream->Serialize(includeFileStream);
		delete includeFileStream;
	}

	void IncludeFile::ReadVer81(Stream* stream)
	{
		Stream* includeFileStream = stream->Deserialize();

		includeFileStream->ReadTimestamp();
		includeFileStream->ReadDword();
		filename				= includeFileStream->ReadString();
		filepath				= includeFileStream->ReadString();
		originalFile			= includeFileStream->ReadBoolean();
		originalFileSize		= includeFileStream->ReadDword();
		
		if (includeFileStream->ReadBoolean())
			data = includeFileStream->Deserialize(false);
		else
			data = NULL;

		exportKind				= includeFileStream->ReadDword();
		exportPath				= includeFileStream->ReadString();
		overwrite				= includeFileStream->ReadBoolean();
		freeMemory				= includeFileStream->ReadBoolean();
		removeAtEndOfGame		= includeFileStream->ReadBoolean();

		delete includeFileStream;
	}

	void IncludeFile::WriteVer7(Stream* stream)
	{

	}

	void IncludeFile::ReadVer7(Stream* stream)
	{

	}
}
