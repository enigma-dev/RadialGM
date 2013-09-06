/*
 *	gmk.cpp
 *	GameMaker (special) K!
 */

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <stream.hpp>
#include <gmk.hpp>
#include <gmkrypt.hpp>

namespace Gmk
{
	GmkFile::GmkFile()
		: itemsProcessed(0.0f),
		  itemsToProcess(0.0f),
		  version(VerUnknown),
		  gameId(0),
		  settings(NULL),
		  triggers(),
		  constants(),
		  sounds(),
		  sprites(),
		  backgrounds(),
		  paths(),
		  scripts(),
		  fonts(),
		  timelines(),
		  objects(),
		  rooms(),
		  lastInstancePlacedId(GMK_MIN_INSTANCE_LAST_ID),
		  lastTilePlacedId(GMK_MIN_TILE_LAST_ID),
		  includeFiles(),
		  gameInformation(NULL),
		  resourceTree(NULL)
	{
		settings = new Settings(this);
		gameInformation = new GameInformation(this);
		resourceTree = new Tree(this);

		std::srand(static_cast<unsigned int>(time(NULL)));
		gameId = std::rand() % GMK_MAX_ID;
	}

	GmkFile::~GmkFile()
	{
		CleanMemory();
	}

	void GmkFile::CleanMemory()
	{
		if (settings != NULL)
		{
			delete settings;
			settings = NULL;
		}

		for(std::size_t i = 0; i < triggers.size(); ++i)
			delete triggers[i];

		triggers.clear();
		constants.clear();

		for(std::size_t i = 0; i < sounds.size(); ++i)
			delete sounds[i];

		sounds.clear();

		for(std::size_t i = 0; i < sprites.size(); ++i)
			delete sprites[i];

		sprites.clear();

		for(std::size_t i = 0; i < backgrounds.size(); ++i)
			delete backgrounds[i];

		backgrounds.clear();

		for(std::size_t i = 0; i < paths.size(); ++i)
			delete paths[i];

		paths.clear();

		for(std::size_t i = 0; i < scripts.size(); ++i)
			delete scripts[i];

		scripts.clear();

		for(std::size_t i = 0; i < fonts.size(); ++i)
			delete fonts[i];

		fonts.clear();

		for(std::size_t i = 0; i < timelines.size(); ++i)
			delete timelines[i];

		timelines.clear();

		for(std::size_t i = 0; i < objects.size(); ++i)
			delete objects[i];

		objects.clear();

		for(std::size_t i = 0; i < rooms.size(); ++i)
			delete rooms[i];

		rooms.clear();

		for(std::size_t i = 0; i < includeFiles.size(); ++i)
			delete includeFiles[i];

		includeFiles.clear();

		if (gameInformation != NULL)
		{
			delete gameInformation;
			gameInformation = NULL;
		}

		if (resourceTree != NULL)
		{
			delete resourceTree;
			resourceTree = NULL;
		}
	}

	void GmkFile::Finalize()
	{
		// Finalize paths
		for(std::size_t i = 0; i < paths.size(); ++i)
			paths[i]->Finalize();

		// Finalize timelines
		for(std::size_t i = 0; i < timelines.size(); ++i)
			timelines[i]->Finalize();

		// Finalize objects
		for(std::size_t i = 0; i < objects.size(); ++i)
			objects[i]->Finalize();

		// Finalize rooms
		for(std::size_t i = 0; i < rooms.size(); ++i)
			rooms[i]->Finalize();

		// Finalize resource tree
		if (resourceTree != NULL)
			resourceTree->Finalize();
	}

	bool GmkFile::Save(const std::string& filename)
	{
		Stream* stream = NULL;
		itemsProcessed = 0.0f;
		itemsToProcess = triggers.size() +
							constants.size() +
							sounds.size() +
							sprites.size() +
							backgrounds.size() +
							paths.size() +
							scripts.size() +
							fonts.size() +
							timelines.size() +
							objects.size() +
							rooms.size() +
							includeFiles.size() +
							packages.size() +
							5.0f;

		try
		{
			stream = new Stream(filename, Stream::SmWrite);

			SaveGmk(stream);

			delete stream;
		}
		catch(GmkException e)
		{
			std::cerr << "Gmk::Save Error: " << e.what() << std::endl;
			version = VerUnknown;

			if (stream != NULL)
				delete stream;

			return false;
		}

		return true;
	}

	bool GmkFile::Load(const std::string& filename)
	{
		Stream* stream = NULL;
		itemsProcessed = 0.0f;
		itemsToProcess = 20.0f;

		CleanMemory();

		try
		{
			stream = new Stream(filename, Stream::SmRead);

			LoadGmk(stream);

			delete stream;
		}
		catch(GmkException e)
		{
			std::cerr << "Gmk::Load Error: " << e.what() << std::endl;
			version = VerUnknown;

			if (stream != NULL)
				delete stream;

			return false;
		}

		return true;
	}

	template <class T>
	void GmkFile::Defragment(std::vector<T*>& v)
	{
		for(typename std::vector<T*>::iterator itr = v.begin(); itr != v.end(); )
		{
			if (!(*itr)->GetExists())
			{
				delete *itr;
				itr = v.erase(itr);
				continue;
			}
				
			++itr;
		}
	}

	void GmkFile::DefragmentResources()
	{
		itemsProcessed = itemsToProcess = 0.0f;

		// Step 1: Delete non-existant resources
		Defragment(sprites);
		Defragment(sounds);
		Defragment(backgrounds);
		Defragment(paths);
		Defragment(scripts);
		Defragment(fonts);
		Defragment(timelines);
		Defragment(objects);
		Defragment(rooms);

		// Step 2: Defragment last placed IDs
		lastInstancePlacedId = GMK_MIN_INSTANCE_LAST_ID;
		lastTilePlacedId = GMK_MIN_TILE_LAST_ID;

		for(std::size_t i = 0; i < rooms.size(); ++i)
		{
			Room* room = rooms[i];

			for(std::size_t j = 0; j < room->instances.size(); ++j)
				room->instances[j]->id = lastInstancePlacedId++;

			for(std::size_t j = 0; j < room->tiles.size(); ++j)
				room->tiles[j]->id = lastTilePlacedId++;
		}
	}

#undef Defragment

	bool GmkFile::IsLoaded() const
	{
		return version != VerUnknown;
	}

	float GmkFile::GetProgress() const
	{
		return (itemsProcessed / itemsToProcess) * 100.0f;
	}

	void GmkFile::SaveGmk(Stream* stream)
	{
		const unsigned int Versions[6] = { 0, 530, 600, 701, 800, 810 };
		
		// Write header
		stream->WriteDword(GMK_MAGIC);
		stream->WriteDword(Versions[static_cast<int>(version)]);
		
		switch(version)
		{
			case Ver81:
				SaveVer81(stream);
				break;

			default:
				throw GmkException("Unsupported version");
		}
	}

	void GmkFile::LoadGmk(Stream* stream)
	{
		// Read header
		if (stream->ReadDword() != GMK_MAGIC)
			throw GmkException("Invalid magic!");

		switch(stream->ReadDword())
		{
			case 530:
				version = Ver53a;
				break;

			case 600:
				version = Ver61;
				break;

			case 701:
				version = Ver7;
				break;

			case 800:
				version = Ver8;
				break;

			case 810:
				version = Ver81;
				break;

			default:
				throw GmkException("Unknown or unsupported version!");
		}

		switch(version)
		{
			case Ver81:
				LoadVer81(stream);
				break;

			case Ver7:
				LoadVer7(stream);
				break;
		}

		Finalize();
	}

	void GmkFile::SaveVer81(Stream* stream)
	{
		// Write header
		stream->WriteDword(gameId);
		for(unsigned int i = 0; i < GMK_GUID_LENGTH; ++i)
		{
			unsigned char guidByte = gameId >> i / 4;
			guidByte %= ((i >> 6) + guidByte & 0x7F) + 0xFF;
			guidByte ^= (i * guidByte >> 3) & 0xAB;

			stream->WriteByte(guidByte);
		}

		++itemsProcessed;

		// Write settings
		stream->WriteDword(800);
		if (settings == NULL)
			throw GmkException("Settings are not declared");

		settings->Write(stream);

		++itemsProcessed;

		// Write triggers
		stream->WriteDword(800);
		stream->WriteDword(triggers.size());
		for(std::size_t i = 0; i < triggers.size(); ++i, ++itemsProcessed)
			triggers[i]->Write(stream);

		stream->WriteTimestamp();

		// Write constants
		stream->WriteDword(800);
		stream->WriteDword(constants.size());
		for(std::size_t i = 0; i < constants.size(); ++i, ++itemsProcessed)
		{
			stream->WriteString(constants[i].first);
			stream->WriteString(constants[i].second);
		}

		stream->WriteTimestamp();

		// Write sounds
		stream->WriteDword(800);
		stream->WriteDword(sounds.size());
		for(std::size_t i = 0; i < sounds.size(); ++i, ++itemsProcessed)
			sounds[i]->Write(stream);

		// Write sprites
		stream->WriteDword(800);
		stream->WriteDword(sprites.size());
		for(std::size_t i = 0; i < sprites.size(); ++i, ++itemsProcessed)
			sprites[i]->Write(stream);

		// Write backgrounds
		stream->WriteDword(800);
		stream->WriteDword(backgrounds.size());
		for(std::size_t i = 0; i < backgrounds.size(); ++i, ++itemsProcessed)
			backgrounds[i]->Write(stream);

		// Write paths
		stream->WriteDword(800);
		stream->WriteDword(paths.size());
		for(std::size_t i = 0; i < paths.size(); ++i, ++itemsProcessed)
			paths[i]->Write(stream);

		// Write scripts
		stream->WriteDword(800);
		stream->WriteDword(scripts.size());
		for(std::size_t i = 0; i < scripts.size(); ++i, ++itemsProcessed)
			scripts[i]->Write(stream);

		// Write fonts
		stream->WriteDword(800);
		stream->WriteDword(fonts.size());
		for(std::size_t i = 0; i < fonts.size(); ++i, ++itemsProcessed)
			fonts[i]->Write(stream);

		// Write timelines
		stream->WriteDword(800);
		stream->WriteDword(timelines.size());
		for(std::size_t i = 0; i < timelines.size(); ++i, ++itemsProcessed)
			timelines[i]->Write(stream);

		// Write objects
		stream->WriteDword(800);
		stream->WriteDword(objects.size());
		for(std::size_t i = 0; i < objects.size(); ++i, ++itemsProcessed)
			objects[i]->Write(stream);

		// Write rooms
		stream->WriteDword(800);
		stream->WriteDword(rooms.size());
		for(std::size_t i = 0; i < rooms.size(); ++i, ++itemsProcessed)
			rooms[i]->Write(stream);

		// Write last ids
		stream->WriteDword(lastInstancePlacedId);
		stream->WriteDword(lastTilePlacedId);
		++itemsProcessed;

		// Write include files
		stream->WriteDword(800);
		stream->WriteDword(includeFiles.size());
		for(std::size_t i = 0; i < includeFiles.size(); ++i, ++itemsProcessed)
			includeFiles[i]->Write(stream);
		
		// Write packages
		stream->WriteDword(700);
		stream->WriteDword(packages.size());
		for(std::size_t i = 0; i < packages.size(); ++i, ++itemsProcessed)
			stream->WriteString(packages[i]);

		// Write game information
		stream->WriteDword(800);
		gameInformation->Write(stream);
		++itemsProcessed;

		// Ignore library creation code (safe)
		stream->WriteDword(500);
		stream->WriteDword(0);

		// Ignore room execution order (safe)
		stream->WriteDword(700);
		stream->WriteDword(0);

		// Write resource tree
		resourceTree->Write(stream);
		++itemsProcessed;
	}

	void GmkFile::LoadVer81(Stream* stream)
	{
		unsigned int count = 0;

		// Read header
		gameId = stream->ReadDword();
		for(unsigned int i = 0; i < GMK_GUID_LENGTH; ++i)
			guid[i] = stream->ReadByte();

		++itemsProcessed;
		
		// Load settings
		stream->ReadDword();
		settings = new Settings(this);
		settings->Read(stream);

		++itemsProcessed;

		// Load triggers
		stream->ReadDword();
		count = stream->ReadDword();
		while(count--)
		{
			Trigger* trigger = new Trigger(this);
			trigger->Read(stream);
			triggers.push_back(trigger);
		}

		stream->ReadTimestamp();

		++itemsProcessed;

		// Load constants
		stream->ReadDword();
		count = stream->ReadDword();
		while(count--)
		{
			std::string name = stream->ReadString();
			std::string value = stream->ReadString();

			constants.push_back(std::pair<std::string, std::string>(name, value));
		}

		stream->ReadTimestamp();

		++itemsProcessed;

		// Load sounds
		stream->ReadDword();
		count = stream->ReadDword();
		while(count--)
		{
			Sound* sound = new Sound(this);
			sound->Read(stream);
			sounds.push_back(sound);
		}

		++itemsProcessed;

		// Load sprites
		stream->ReadDword();
		count = stream->ReadDword();
		while(count--)
		{
			Sprite* sprite = new Sprite(this);
			sprite->Read(stream);
			sprites.push_back(sprite);
		}

		++itemsProcessed;

		// Load backgrounds
		stream->ReadDword();
		count = stream->ReadDword();
		while(count--)
		{
			Background* background = new Background(this);
			background->Read(stream);
			backgrounds.push_back(background);
		}

		++itemsProcessed;

		// Load paths
		stream->ReadDword();
		count = stream->ReadDword();
		while(count--)
		{
			Path* path = new Path(this);
			path->Read(stream);
			paths.push_back(path);
		}

		++itemsProcessed;

		// Load scripts
		stream->ReadDword();
		count = stream->ReadDword();
		while(count--)
		{
			Script* script = new Script(this);
			script->Read(stream);
			scripts.push_back(script);
		}

		++itemsProcessed;

		// Load fonts
		stream->ReadDword();
		count = stream->ReadDword();
		while(count--)
		{
			Font* font = new Font(this);
			font->Read(stream);
			fonts.push_back(font);
		}

		++itemsProcessed;
	
		// Load timelines
		stream->ReadDword();
		count = stream->ReadDword();
		while(count--)
		{
			Timeline* timeline = new Timeline(this);
			timeline->Read(stream);
			timelines.push_back(timeline);
		}

		++itemsProcessed;

		// Load objects
		stream->ReadDword();
		count = stream->ReadDword();
		while(count--)
		{
			Object* object = new Object(this);
			object->Read(stream);
			objects.push_back(object);
		}

		++itemsProcessed;

		// Load rooms
		stream->ReadDword();
		count = stream->ReadDword();
		while(count--)
		{
			Room* room = new Room(this);
			room->Read(stream);
			rooms.push_back(room);
		}

		++itemsProcessed;

		// Load last ids
		lastInstancePlacedId = stream->ReadDword();
		lastTilePlacedId = stream->ReadDword();

		++itemsProcessed;

		// Load include files
		stream->ReadDword();
		count = stream->ReadDword();
		while(count--)
		{
			IncludeFile* includeFile = new IncludeFile(this);
			includeFile->Read(stream);
			includeFiles.push_back(includeFile);
		}

		++itemsProcessed;

		// Load packages
		stream->ReadDword();
		count = stream->ReadDword();
		while(count--)
			packages.push_back(stream->ReadString());

		++itemsProcessed;

		// Read game information
		stream->ReadDword();
		gameInformation = new GameInformation(this);
		gameInformation->Read(stream);

		++itemsProcessed;

		// Read library creation code -- we should be able to safely ignore this
		stream->ReadDword();
		count = stream->ReadDword();
		while(count--)
			stream->ReadString();

		++itemsProcessed;

		// Read room execution order -- this too
		stream->ReadDword();
		count = stream->ReadDword();
		while(count--)
			stream->ReadDword();

		++itemsProcessed;

		// Read resource tree
		resourceTree = new Tree(this);
		resourceTree->Read(stream);

		++itemsProcessed;
	}

	void GmkFile::SaveVer7(Stream* stream)
	{

	}

	void GmkFile::LoadVer7(Stream* stream)
	{
		unsigned int count = 0;

		// Decrypt GMK
		Gmkrypt gmkrypt(Gmkrypt::ReadSeedFromJunkyard(stream));
		Stream* decryptedStream = gmkrypt.Decrypt(stream);

		// New stream
		stream = decryptedStream;

		// Read header
		gameId = stream->ReadDword();
		for(unsigned int i = 0; i < GMK_GUID_LENGTH; ++i)
			guid[i] = stream->ReadByte();

		// Load settings
		stream->ReadDword();
		settings = new Settings(this);
		settings->Read(stream);

		// Load sounds
		stream->ReadDword();
		count = stream->ReadDword();
		while(count--)
		{
			Sound* sound = new Sound(this);
			sound->Read(stream);
			sounds.push_back(sound);
		}

		// Load sprites
		stream->ReadDword();
		count = stream->ReadDword();
		while(count--)
		{
			Sprite* sprite = new Sprite(this);
			sprite->Read(stream);
			sprites.push_back(sprite);
		}

		// Load backgrounds
		stream->ReadDword();
		count = stream->ReadDword();
		while(count--)
		{
			Background* background = new Background(this);
			background->Read(stream);
			backgrounds.push_back(background);
		}
		
		delete decryptedStream;
	}
}
