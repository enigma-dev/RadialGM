/**
* @file gmksprite.h
* @brief GMK Sprite
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

#ifndef __GMK_SPRITE_H
#define __GMK_SPRITE_H

#include <gmkresource.h>

namespace Gmk
{
	class Sprite : public GmkResource
	{
	public:
		enum Shape
		{
			ShapePrecise,
			ShapeRectangle,
			ShapeDisc,
			ShapeDiamond
		};

		enum BoundingBox
 		{
			BbAutomatic,
			BbFull,
			BbManual
		};

		typedef struct _Subimage
		{
			unsigned int width, height;
			Stream* data;
		} Subimage;

	protected:
		void WriteVer81(Stream* stream);
		void ReadVer81(Stream* stream);

		void WriteVer7(Stream* stream);
		void ReadVer7(Stream* stream);

	public:
		unsigned int			width;
		unsigned int			height;
		int						bboxLeft;
		int						bboxRight;
		int						bboxBottom;
		int						bboxTop;
		unsigned int			originX;
		unsigned int			originY;
		std::vector<Subimage>	subimages;
		unsigned int			maskShape;
		unsigned int			alphaTolerance;
		bool					preciseCollisionChecking;
		bool					seperateMasks;
		bool					transparent;
		bool					smoothEdges;
		unsigned int			boundingBox;
		bool					preload;

		Sprite(GmkFile* gmk);
		~Sprite();

		int GetId() const;
	};
}

#endif
