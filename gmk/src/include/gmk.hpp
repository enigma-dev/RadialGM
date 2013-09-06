/*
 *	gmk.hpp
 *	Game Maker Kompressed and enKrypted format to piss of Klam
 */

#ifndef __GMK_GMK_HPP
#define __GMK_GMK_HPP

#include <iostream>
#include <vector>
#include <gmkcommon.hpp>
#include <gmkresource.hpp>
#include <gmksettings.hpp>
#include <gmktrigger.hpp>
#include <gmksound.hpp>
#include <gmksprite.hpp>
#include <gmkbackground.hpp>
#include <gmkpath.hpp>
#include <gmkscript.hpp>
#include <gmkfont.hpp>
#include <gmktimeline.hpp>
#include <gmkobject.hpp>
#include <gmkroom.hpp>
#include <gmkincludefile.hpp>
#include <gmkgameinfo.hpp>
#include <gmktree.hpp>

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
