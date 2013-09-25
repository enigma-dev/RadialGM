/**
* @file  gmkpath.cpp
* @brief GMK Path
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

#include <gmkpath.h>
#include <gmk.h>

namespace Gmk
{
	Path::Path(GmkFile* gmk)
		: GmkResource(gmk),
		  connectionKind(KindStraight),
		  closed(true),
		  precision(4),
		  roomIndex(-1),
		  snapX(16),
		  snapY(16),
		  points(),
		  room(NULL)
	{
		
	}

	Path::~Path()
	{
		
	}

	int Path::GetId() const
	{
		for(std::size_t i = 0; i < gmkHandle->paths.size(); ++i)
			if (gmkHandle->paths[i] == this)
				return i;

		return -1;
	}

	void Path::WriteVer81(Stream* stream)
	{
		Stream* pathStream = new Stream();

		pathStream->WriteBoolean(exists);
		if (exists)
		{
			pathStream->WriteString(name);
			pathStream->WriteTimestamp();
			pathStream->WriteDword(530);
			pathStream->WriteDword(connectionKind);
			pathStream->WriteBoolean(closed);
			pathStream->WriteDword(precision);
			pathStream->WriteDword(room != NULL ? room->GetId() : RoomIndexNone);
			pathStream->WriteDword(snapX);
			pathStream->WriteDword(snapY);

			pathStream->WriteDword(points.size());
			for(std::size_t i = 0; i < points.size(); ++i)
			{
				pathStream->WriteDouble(points[i].x);
				pathStream->WriteDouble(points[i].y);
				pathStream->WriteDouble(points[i].speed);
			}
		}

		stream->Serialize(pathStream);
		delete pathStream;
	}

	void Path::ReadVer81(Stream* stream)
	{
		Stream* pathStream = stream->Deserialize();

		if (!pathStream->ReadBoolean())
		{
			delete pathStream;
			exists = false;
			return;
		}

		name				= pathStream->ReadString();
		pathStream->ReadTimestamp();
		pathStream->ReadDword();
		connectionKind		= pathStream->ReadDword();
		closed				= pathStream->ReadBoolean();
		precision			= pathStream->ReadDword();
		roomIndex			= pathStream->ReadDword();
		snapX				= pathStream->ReadDword();
		snapY				= pathStream->ReadDword();

		unsigned int count = pathStream->ReadDword();
		while(count--)
		{
			Point point;

			point.x = pathStream->ReadDouble();
			point.y = pathStream->ReadDouble();
			point.speed = pathStream->ReadDouble();

			points.push_back(point);
		}

		delete pathStream;
		exists = true;
	}

	void Path::WriteVer7(Stream* stream)
	{

	}

	void Path::ReadVer7(Stream* stream)
	{

	}

	void Path::Finalize()
	{
		room = (roomIndex != RoomIndexNone) ? gmkHandle->rooms[roomIndex] : NULL;
	}
}
