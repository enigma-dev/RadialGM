/**
* @file  gmktrigger.h
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

#ifndef __GMK_TRIGGER_H
#define __GMK_TRIGGER_H

#include <gmkresource.h>

namespace Gmk
{
	class Trigger : public GmkResource
	{
	public:
		enum Moment
		{
			MomentMiddle,
			MomentBegin,
			MomentEnd
		};

	protected:
		void WriteVer81(Stream* stream);
		void ReadVer81(Stream* stream);

		void WriteVer7(Stream* stream);
		void ReadVer7(Stream* stream);

	public:
		std::string			condition;
		unsigned int		momentOfChecking;
		std::string			constantName;

		Trigger(GmkFile* gmk);
		~Trigger();
	};
}

#endif
