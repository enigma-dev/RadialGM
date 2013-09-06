/**
* @file  gmksound.hpp
* @brief GMK Sound
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

#ifndef __GMK_SOUND_HPP
#define __GMK_SOUND_HPP

#include <gmkresource.hpp>

namespace Gmk
{
	class Sound : public GmkResource
	{
	public:
		enum Kind
		{
			KindNormal,
			KindBackground,
			Kind3D,
			KindMultimedia
		};

		enum Effect
		{
			EffectNone				= 0x00,
			EffectChorus			= 0x01,
			EffectEcho				= 0x02,
			EffectFlanger			= 0x04,
			EffectGargle			= 0x08,
			EffectReverb			= 0x10
		};

	protected:
		void WriteVer81(Stream* stream);
		void ReadVer81(Stream* stream);

		void WriteVer7(Stream* stream);
		void ReadVer7(Stream* stream);

	public:
		unsigned int			kind;
		std::string				extension;
		std::string				filename;
		Stream*					data;
		unsigned int			effects;
		double					volume;
		double					pan;
		bool					preload;

		Sound(GmkFile* gmk);
		~Sound();

		int GetId() const;
	};
}

#endif
