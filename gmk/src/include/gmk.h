/**
* @file  gmk.h
* @brief Game Maker Kompressed and enKrypted format to piss of Klam
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

#ifndef __GMK_GMK_H
#define __GMK_GMK_H

#include <iostream>
#include <vector>
#include <gmkcommon.h>
#include <gmkresource.h>
#include <gmksettings.h>
#include <gmktrigger.h>
#include <gmksound.h>
#include <gmksprite.h>
#include <gmkbackground.h>
#include <gmkpath.h>
#include <gmkscript.h>
#include <gmkfont.h>
#include <gmktimeline.h>
#include <gmkobject.h>
#include <gmkroom.h>
#include <gmkincludefile.h>
#include <gmkgameinfo.h>
#include <gmktree.h>

namespace Gmk
{
	class GmkFile
	{
	public:
		static const unsigned int GMK_MAGIC					= 1234321;
		static const unsigned int GMK_GUID_LENGTH			= 16;
		static const unsigned int GMK_MAX_ID				= 100000000;
		static const unsigned int GMK_MIN_INSTANCE_LAST_ID	= 100000;
		static const unsigned int GMK_MIN_TILE_LAST_ID		= 1000000;

	private:
		float itemsProcessed, itemsToProcess;

		void CleanMemory();
		void Finalize();

		template <class T>
		void Defragment(std::vector<T*>& v);

		void SaveVer81(Stream* stream);
		void LoadVer81(Stream* stream);

		void SaveVer7(Stream* stream);
		void LoadVer7(Stream* stream);

		void SaveGmk(Stream* stream);
		void LoadGmk(Stream* stream);

	public:
		Version								version;
		unsigned int						gameId;
		unsigned char						guid[GMK_GUID_LENGTH];
		Settings*							settings;
		std::vector<Trigger*>				triggers;
		std::vector<std::pair<std::string, std::string> >
											constants;
		std::vector<Sound*>					sounds;
		std::vector<Sprite*>				sprites;
		std::vector<Background*>			backgrounds;
		std::vector<Path*>					paths;
		std::vector<Script*>				scripts;
		std::vector<Font*>					fonts;
		std::vector<Timeline*>				timelines;
		std::vector<Object*>				objects;
		std::vector<Room*>					rooms;
		unsigned int						lastInstancePlacedId;
		unsigned int						lastTilePlacedId;
		std::vector<IncludeFile*>			includeFiles;
		std::vector<std::string>			packages;
		GameInformation*					gameInformation;
		Tree*								resourceTree;

		GmkFile();
		~GmkFile();

		bool Save(const std::string& filename);
		bool Load(const std::string& filename);
		void DefragmentResources();

 		bool IsLoaded() const;
		float GetProgress() const;
	};
}

#endif
