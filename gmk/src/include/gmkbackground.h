/**
* @file  gmkbackground.h
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

#ifndef __GMK_BACKGROUND_H
#define __GMK_BACKGROUND_H

#include <gmkresource.h>

namespace Gmk
{
	class Background : public GmkResource
	{
	protected:
		void WriteVer81(Stream* stream);
		void ReadVer81(Stream* stream);

		void WriteVer7(Stream* stream);
		void ReadVer7(Stream* stream);

	public:
		bool					transparent;
		bool					smoothEdges;
		bool					preload;
		bool					useAsTileset;
		unsigned int			tileWidth;
		unsigned int			tileHeight;
		unsigned int			tileHorizontalOffset;
		unsigned int			tileVerticalOffset;
		unsigned int			tileHorizontalSeperation;
		unsigned int			tileVerticalSeperation;
		unsigned int			width;
		unsigned int			height;
		Stream*					data;

		Background(GmkFile* gmk);
		~Background();

		int GetId() const;
	};
}

#endif
