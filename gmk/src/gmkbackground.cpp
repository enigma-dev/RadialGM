/**
* @file  gmkbackground.cpp
* @brief GMK Background
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

#include <gmkbackground.hpp>
#include <gmk.hpp>

namespace Gmk
{
	Background::Background(GmkFile* gmk)
		: GmkResource(gmk),
		  transparent(false),
		  smoothEdges(false),
		  preload(false),
		  useAsTileset(false),
		  tileWidth(16),
		  tileHeight(16),
		  tileHorizontalOffset(0),
		  tileVerticalOffset(0),
		  tileHorizontalSeperation(0),
		  tileVerticalSeperation(0),
		  width(0),
		  height(0),
		  data(NULL)
	{
		
	}

	Background::~Background()
	{
		if (data != NULL)
			delete data;
	}

	int Background::GetId() const
	{
		for(std::size_t i = 0; i < gmkHandle->backgrounds.size(); ++i)
			if (gmkHandle->backgrounds[i] == this)
				return i;

		return -1;
	}

	void Background::WriteVer81(Stream* stream)
	{
		Stream* backgroundStream = new Stream();

		backgroundStream->WriteBoolean(exists);
		if (exists)
		{
			backgroundStream->WriteString(name);
			backgroundStream->WriteTimestamp();
			backgroundStream->WriteDword(710);
			backgroundStream->WriteBoolean(useAsTileset);
			backgroundStream->WriteDword(tileWidth);
			backgroundStream->WriteDword(tileHeight);
			backgroundStream->WriteDword(tileHorizontalOffset);
			backgroundStream->WriteDword(tileVerticalOffset);
			backgroundStream->WriteDword(tileHorizontalSeperation);
			backgroundStream->WriteDword(tileVerticalSeperation);
			backgroundStream->WriteDword(800);
			backgroundStream->WriteDword(width);
			backgroundStream->WriteDword(height);

			if (width != 0 && height != 0)
				backgroundStream->Serialize(data, false);
		}

		stream->Serialize(backgroundStream);
		delete backgroundStream;
	}

	void Background::ReadVer81(Stream* stream)
	{
		Stream* backgroundStream = stream->Deserialize();

		if (!backgroundStream->ReadBoolean())
		{
			delete backgroundStream;
			exists = false;
			return;
		}

		name						= backgroundStream->ReadString();
		backgroundStream->ReadTimestamp();
		backgroundStream->ReadDword();
		useAsTileset				= backgroundStream->ReadBoolean();
		tileWidth					= backgroundStream->ReadDword();
		tileHeight					= backgroundStream->ReadDword();
		tileHorizontalOffset		= backgroundStream->ReadDword();
		tileVerticalOffset			= backgroundStream->ReadDword();
		tileHorizontalSeperation	= backgroundStream->ReadDword();
		tileVerticalSeperation		= backgroundStream->ReadDword();
		backgroundStream->ReadDword();
		width						= backgroundStream->ReadDword();
		height						= backgroundStream->ReadDword();

		if (width != 0 && height != 0)
			data = backgroundStream->Deserialize(false);

		delete backgroundStream;
		exists = true;
	}

	void Background::WriteVer7(Stream* stream)
	{

	}

	void Background::ReadVer7(Stream* stream)
	{
		if (!stream->ReadBoolean())
		{
			exists = false;
			return;
		}

		name						= stream->ReadString();
		stream->ReadDword();
		width						= stream->ReadDword();
		height						= stream->ReadDword();
		transparent					= stream->ReadBoolean();
		smoothEdges					= stream->ReadBoolean();
		preload						= stream->ReadBoolean();
		useAsTileset				= stream->ReadBoolean();
		tileWidth					= stream->ReadDword();
		tileHeight					= stream->ReadDword();
		tileHorizontalOffset		= stream->ReadDword();
		tileVerticalOffset			= stream->ReadDword();
		tileHorizontalSeperation	= stream->ReadDword();
		tileVerticalSeperation		= stream->ReadDword();

		if (stream->ReadBoolean())
			data = stream->ReadBitmapOld();

		exists = true;
	}
}
