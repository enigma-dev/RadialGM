/**
* @file  gmkresource.h
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

#ifndef __GMK_RESOURCE_H
#define __GMK_RESOURCE_H

#include <iostream>
#include <vector>
#include <string>
#include <stream.h>
#include <gmkcommon.h>

namespace Gmk
{
	class GmkFile;

	class GmkResource
	{
	public:
		enum ResourceType
		{
			RtSprite,
			RtSound,
			RtBackground,
			RtPath,
			RtScript,
			RtFont,
			RtTimeline,
			RtObject,
			RtRoom,
			RtUnknown,
			RtCount
		};

		GmkResource* GetResource(unsigned int type, unsigned int index) const;

	protected:
		GmkFile* gmkHandle;
		bool exists;

		virtual void WriteVer81(Stream* stream) = 0;
		virtual void ReadVer81(Stream* stream) = 0;

		virtual void WriteVer7(Stream* stream) = 0;
		virtual void ReadVer7(Stream* stream) = 0;

	public:
		std::string name;

		GmkResource(GmkFile* gmk);

		void Write(Stream* stream);
		void Read(Stream* stream);
		virtual void Finalize();

		virtual int GetId() const;
		bool GetExists() const;
	};
}

#endif
