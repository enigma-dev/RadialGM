/**
* @file  gmkfont.cpp
* @brief GMK Font
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

#include <gmkfont.hpp>
#include <gmk.hpp>

namespace Gmk
{
	Font::Font(GmkFile* gmk)
		: GmkResource(gmk),
		  fontName(""),
		  size(12),
		  bold(false),
		  italic(false),
		  characterRangeBegin(32),
		  characterRangeEnd(127),
		  characterSet(DEFAULT_CHARSET),
		  antiAliasing(Aa3)
	{

	}

	Font::~Font()
	{

	}

	int Font::GetId() const
	{
		for(std::size_t i = 0; i < gmkHandle->fonts.size(); ++i)
			if (gmkHandle->fonts[i] == this)
				return i;

		return -1;
	}

	void Font::WriteVer81(Stream* stream)
	{
		Stream* fontStream = new Stream();

		fontStream->WriteBoolean(exists);
		if (exists)
		{
			fontStream->WriteString(name);
			fontStream->WriteTimestamp();
			fontStream->WriteDword(800);
			fontStream->WriteString(fontName);
			fontStream->WriteDword(size);
			fontStream->WriteBoolean(bold);
			fontStream->WriteBoolean(italic);
			
			unsigned int value = 0;
			value |= (characterSet & 0xFF) << 16;
			value |= (antiAliasing & 0xFF) << 24;
			value |= characterRangeBegin & 0xFFFF;
			
			fontStream->WriteDword(value);
			fontStream->WriteDword(characterRangeEnd);
		}

		stream->Serialize(fontStream);
		delete fontStream;
	}

	void Font::ReadVer81(Stream* stream)
	{
		Stream* fontStream = stream->Deserialize();

		if (!fontStream->ReadBoolean())
		{
			delete fontStream;
			exists = false;
			return;
		}

		name				= fontStream->ReadString();
		fontStream->ReadTimestamp();
		fontStream->ReadDword();
		fontName			= fontStream->ReadString();
		size				= fontStream->ReadDword();
		bold				= fontStream->ReadBoolean();
		italic				= fontStream->ReadBoolean();

		unsigned int value	= fontStream->ReadDword();
		characterSet		= (value >> 16) & 0xFF;
		antiAliasing		= (value >> 24) & 0xFF;
		characterRangeBegin	= value & 0xFFFF;

		characterRangeEnd	= fontStream->ReadDword();

		delete fontStream;
		exists = true;
	}

	void Font::WriteVer7(Stream* stream)
	{

	}

	void Font::ReadVer7(Stream* stream)
	{

	}
}
