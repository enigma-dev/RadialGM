/**
* @file  gmkgameinfo.cpp
* @brief GMK Game Information
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

#include <gmkgameinfo.hpp>

namespace Gmk
{
	GameInformation::GameInformation(GmkFile* gmk)
		: GmkResource(gmk),
		  backgroundColor(BuildColor(255, 255, 225)),
		  showInSeperateWindow(true),
		  caption("Game Information"),
		  left(-1),
		  top(-1),
		  width(600),
		  height(400),
		  showBorder(true),
		  sizeable(true),
		  alwaysOnTop(false),
		  freeze(true),
		  information("{\\rtf1\\ansi\\ansicpg1252\\deff0\\deflang1033"
					  "{\\fonttbl{\\f0\\fnil Arial;}}{\\colortbl ;\\red0\\green0\\blue0;}"
					  "\\viewkind4\\uc1\\pard\\cf1\\f0\\fs24}")
	{
		exists = true;
	}

	GameInformation::~GameInformation()
	{

	}

	void GameInformation::WriteVer81(Stream* stream)
	{
		Stream* gameInfoStream = new Stream();

		gameInfoStream->WriteDword(backgroundColor);
		gameInfoStream->WriteBoolean(showInSeperateWindow);
		gameInfoStream->WriteString(caption);
		gameInfoStream->WriteDword(left);
		gameInfoStream->WriteDword(top);
		gameInfoStream->WriteDword(width);
		gameInfoStream->WriteDword(height);
		gameInfoStream->WriteBoolean(showBorder);
		gameInfoStream->WriteBoolean(sizeable);
		gameInfoStream->WriteBoolean(alwaysOnTop);
		gameInfoStream->WriteBoolean(freeze);
		gameInfoStream->WriteTimestamp();
		gameInfoStream->WriteString(information);

		stream->Serialize(gameInfoStream);
		delete gameInfoStream;
	}

	void GameInformation::ReadVer81(Stream* stream)
	{
		Stream* gameInfoStream = stream->Deserialize();

		backgroundColor				= gameInfoStream->ReadDword();
		showInSeperateWindow		= gameInfoStream->ReadBoolean();
		caption						= gameInfoStream->ReadString();
		left						= gameInfoStream->ReadDword();
		top							= gameInfoStream->ReadDword();
		width						= gameInfoStream->ReadDword();
		height						= gameInfoStream->ReadDword();
		showBorder					= gameInfoStream->ReadBoolean();
		sizeable					= gameInfoStream->ReadBoolean();
		alwaysOnTop					= gameInfoStream->ReadBoolean();
		freeze						= gameInfoStream->ReadBoolean();
		gameInfoStream->ReadTimestamp();
		information					= gameInfoStream->ReadString();

		delete gameInfoStream;
	}

	void GameInformation::WriteVer7(Stream* stream)
	{

	}

	void GameInformation::ReadVer7(Stream* stream)
	{

	}
}
