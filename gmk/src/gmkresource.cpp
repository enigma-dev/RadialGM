/**
* @file  gmkresource.cpp
* @brief GMK Resource
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

#include <gmkresource.hpp>
#include <gmk.hpp>

namespace Gmk
{
	GmkResource::GmkResource(GmkFile* gmk)
		: gmkHandle(gmk),
		  exists(true),
		  name("")
	{

	}

	GmkResource* GmkResource::GetResource(unsigned int type, unsigned int index) const
	{
		switch(type)
		{
			case RtSprite:
				return index < gmkHandle->sprites.size() ? gmkHandle->sprites[index] : NULL;

			case RtSound:
				return index < gmkHandle->sounds.size() ? gmkHandle->sounds[index] : NULL;

			case RtBackground:
				return index < gmkHandle->backgrounds.size() ? gmkHandle->backgrounds[index] : NULL;

			case RtPath:
				return index < gmkHandle->paths.size() ? gmkHandle->paths[index] : NULL;

			case RtScript:
				return index < gmkHandle->scripts.size() ? gmkHandle->scripts[index] : NULL;

			case RtFont:
				return index < gmkHandle->fonts.size() ? gmkHandle->fonts[index] : NULL;

			case RtTimeline:
				return index < gmkHandle->timelines.size() ? gmkHandle->timelines[index] : NULL;

			case RtObject:
				return index < gmkHandle->objects.size() ? gmkHandle->objects[index] : NULL;

			case RtRoom:
				return index < gmkHandle->rooms.size() ? gmkHandle->rooms[index] : NULL;
		}

		return NULL;
	}

	void GmkResource::Write(Stream* stream)
	{
		switch(gmkHandle->version)
		{
			case Ver81:
				WriteVer81(stream);
				break;

			case Ver7:
				WriteVer7(stream);
				break;

			default:
				throw GmkException("No rule to Write resource");
		}
	}

	void GmkResource::Read(Stream* stream)
	{
		switch(gmkHandle->version)
		{
			case Ver81:
				ReadVer81(stream);
				break;

			case Ver7:
				ReadVer7(stream);
				break;

			default:
				throw GmkException("No rule to Read resource");
		}
	}

	void GmkResource::Finalize()
	{

	}

	int GmkResource::GetId() const
	{
		return -1;
	}

	bool GmkResource::GetExists() const
	{
		return exists;
	}
}
