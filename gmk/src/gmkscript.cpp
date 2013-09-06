/**
* @file  gmkscript.cpp
* @brief GMK Script
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

#include <gmkscript.hpp>
#include <gmk.hpp>

namespace Gmk
{
	Script::Script(GmkFile* gmk)
		: GmkResource(gmk),
		  value("")
	{
		
	}

	Script::~Script()
	{

	}

	int Script::GetId() const
	{
		for(std::size_t i = 0; i < gmkHandle->scripts.size(); ++i)
			if (gmkHandle->scripts[i] == this)
				return i;

		return -1;
	}

	void Script::WriteVer81(Stream* stream)
	{
		Stream* scriptStream = new Stream();

		scriptStream->WriteBoolean(exists);
		if (exists)
		{
			scriptStream->WriteString(name);
			scriptStream->WriteTimestamp();
			scriptStream->WriteDword(800);
			scriptStream->WriteString(value);
		}

		stream->Serialize(scriptStream);
		delete scriptStream;
	}

	void Script::ReadVer81(Stream* stream)
	{
		Stream* scriptStream = stream->Deserialize();

		if (!scriptStream->ReadBoolean())
		{
			delete scriptStream;
			exists = false;
			return;
		}
	
		name = scriptStream->ReadString();
		scriptStream->ReadTimestamp();
		scriptStream->ReadDword();
		value = scriptStream->ReadString();

		delete scriptStream;
		exists = true;
	}

	void Script::WriteVer7(Stream* stream)
	{

	}

	void Script::ReadVer7(Stream* stream)
	{

	}
}
