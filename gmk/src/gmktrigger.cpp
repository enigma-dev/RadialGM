/**
* @file  gmktrigger.cpp
* @brief GMK Trigger
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

#include <gmktrigger.h>

namespace Gmk
{
	Trigger::Trigger(GmkFile* gmk)
		: GmkResource(gmk),
		  condition(""),
		  momentOfChecking(MomentBegin),
		  constantName("")
	{
		
	}

	Trigger::~Trigger()
	{
		
	}

	void Trigger::WriteVer81(Stream* stream)
	{
		Stream* writeStream = new Stream();

		writeStream->WriteBoolean(exists);
		if (exists)
		{
			writeStream->WriteDword(800);

			writeStream->WriteString(name);
			writeStream->WriteString(condition);
			writeStream->WriteDword(momentOfChecking);
			writeStream->WriteString(constantName);
		}
		
		stream->Serialize(writeStream);
		delete writeStream;
	}

	void Trigger::ReadVer81(Stream* stream)
	{
		Stream* triggerStream = stream->Deserialize();

		if (!triggerStream->ReadBoolean())
		{
			delete triggerStream;
			exists = false;
			return;
		}

		triggerStream->ReadDword();
		name				= triggerStream->ReadString();
		condition			= triggerStream->ReadString();
		momentOfChecking	= triggerStream->ReadDword();
		constantName		= triggerStream->ReadString();

		delete triggerStream;
		exists = true;
	}

	void Trigger::WriteVer7(Stream* stream)
	{

	}

	void Trigger::ReadVer7(Stream* stream)
	{

	}
}
