/*
 *	gmkroom.cpp
 *	GMK Room
 */

#include <gmkroom.hpp>
#include <gmk.hpp>

namespace Gmk
{
	Room::Room(GmkFile* gmk)
		: GmkResource(gmk),
		  caption(""),
		  width(640),
		  height(480),
		  snapY(16),
		  snapX(16),
		  isometricGrid(false),
		  speed(30),
		  persistent(false),
		  backgroundColor(BuildColor(192, 192, 192)),
		  drawBackgroundColor(true),
		  clearBackgroundWithWindowColor(true),
		  creationCode(""),
		  backgrounds(),
		  viewsEnabled(false),
		  views(),
		  instances(),
		  tiles(),
		  rememberRoomEditorInfo(true),
		  roomEditorWidth(646),
		  roomEditorHeight(488),
		  showGrid(true),
		  showObjects(true),
		  showTiles(true),
		  showBackgrounds(true),
		  showForegrounds(true),
		  showViews(false),
		  deleteUnderlyingObjects(true),
		  deleteUnderlyingTiles(true),
		  tabIndex(0),
		  scrollbarX(0),
		  scrollbarY(0)
	{

	}

	Room::~Room()
	{
		for(std::size_t i = 0; i < backgrounds.size(); ++i)
			delete backgrounds[i];

		for(std::size_t i = 0; i < views.size(); ++i)
			delete views[i];

		for(std::size_t i = 0; i < instances.size(); ++i)
			delete instances[i];

		for(std::size_t i = 0; i < tiles.size(); ++i)
			delete tiles[i];
	}

	int Room::GetId() const
	{
		for(std::size_t i = 0; i < gmkHandle->rooms.size(); ++i)
			if (gmkHandle->rooms[i] == this)
				return i;

		return -1;
	}

	void Room::WriteVer81(Stream* stream)
	{
		Stream* roomStream = new Stream();

		roomStream->WriteBoolean(exists);
		if (exists)
		{
			roomStream->WriteString(name);
			roomStream->WriteTimestamp();
			roomStream->WriteDword(541);
			roomStream->WriteString(caption);
			roomStream->WriteDword(width);
			roomStream->WriteDword(height);
			roomStream->WriteDword(snapY);
			roomStream->WriteDword(snapX);
			roomStream->WriteBoolean(isometricGrid);
			roomStream->WriteDword(speed);
			roomStream->WriteBoolean(persistent);
			roomStream->WriteDword(backgroundColor);
			roomStream->WriteDword(BuildByte(0, 0, 0, 0, 0, 0, !clearBackgroundWithWindowColor, drawBackgroundColor));
			roomStream->WriteString(creationCode);

			roomStream->WriteDword(backgrounds.size());
			for(std::size_t i = 0; i < backgrounds.size(); ++i)
				backgrounds[i]->Write(roomStream);

			roomStream->WriteBoolean(viewsEnabled);
			roomStream->WriteDword(views.size());
			for(std::size_t i = 0; i < views.size(); ++i)
				views[i]->Write(roomStream);

			roomStream->WriteDword(instances.size());
			for(std::size_t i = 0; i < instances.size(); ++i)
				instances[i]->Write(roomStream);

			roomStream->WriteDword(tiles.size());
			for(std::size_t i = 0; i < tiles.size(); ++i)
				tiles[i]->Write(roomStream);

			roomStream->WriteBoolean(rememberRoomEditorInfo);
			roomStream->WriteDword(roomEditorWidth);
			roomStream->WriteDword(roomEditorHeight);
			roomStream->WriteBoolean(showGrid);
			roomStream->WriteBoolean(showObjects);
			roomStream->WriteBoolean(showTiles);
			roomStream->WriteBoolean(showBackgrounds);
			roomStream->WriteBoolean(showForegrounds);
			roomStream->WriteBoolean(showViews);
			roomStream->WriteBoolean(deleteUnderlyingObjects);
			roomStream->WriteBoolean(deleteUnderlyingTiles);
			roomStream->WriteDword(tabIndex);
			roomStream->WriteDword(scrollbarX);
			roomStream->WriteDword(scrollbarY);
		}

		stream->Serialize(roomStream);
		delete roomStream;
	}

	void Room::ReadVer81(Stream* stream)
	{
		unsigned int count;
		Stream* roomStream = stream->Deserialize();

		if (!roomStream->ReadBoolean())
		{
			delete roomStream;
			exists = false;
			return;
		}

		name					= roomStream->ReadString();
		roomStream->ReadTimestamp();
		roomStream->ReadDword();
		caption					= roomStream->ReadString();
		width					= roomStream->ReadDword();
		height					= roomStream->ReadDword();
		snapY					= roomStream->ReadDword();
		snapX					= roomStream->ReadDword();
		isometricGrid			= roomStream->ReadBoolean();
		speed					= roomStream->ReadDword();
		persistent				= roomStream->ReadBoolean();
		backgroundColor			= roomStream->ReadDword();

		unsigned int bgFlags = roomStream->ReadDword();
		drawBackgroundColor = GetBit(bgFlags, 0);
		clearBackgroundWithWindowColor = !GetBit(bgFlags, 1);

		creationCode = roomStream->ReadString();

		count = roomStream->ReadDword();
		while(count--)
		{
			Background* background = new Background(gmkHandle);
			background->Read(roomStream);
			backgrounds.push_back(background);
		}

		viewsEnabled = roomStream->ReadBoolean();

		count = roomStream->ReadDword();
		while(count--)
		{
			View* view = new View(gmkHandle);
			view->Read(roomStream);
			views.push_back(view);
		}

		count = roomStream->ReadDword();
		while(count--)
		{
			Instance* instance = new Instance(gmkHandle);
			instance->Read(roomStream);
			instances.push_back(instance);
		}

		count = roomStream->ReadDword();
		while(count--)
		{
			Tile* tile = new Tile(gmkHandle);
			tile->Read(roomStream);
			tiles.push_back(tile);
		}

		rememberRoomEditorInfo			= roomStream->ReadBoolean();
		roomEditorWidth					= roomStream->ReadDword();
		roomEditorHeight				= roomStream->ReadDword();
		showGrid						= roomStream->ReadBoolean();
		showObjects						= roomStream->ReadBoolean();
		showTiles						= roomStream->ReadBoolean();
		showBackgrounds					= roomStream->ReadBoolean();
		showForegrounds					= roomStream->ReadBoolean();
		showViews						= roomStream->ReadBoolean();
		deleteUnderlyingObjects			= roomStream->ReadBoolean();
		deleteUnderlyingTiles			= roomStream->ReadBoolean();
		tabIndex						= roomStream->ReadDword();
		scrollbarX						= roomStream->ReadDword();
		scrollbarY						= roomStream->ReadDword();

		delete roomStream;
		exists = true;
	}

	void Room::WriteVer7(Stream* stream)
	{

	}

	void Room::ReadVer7(Stream* stream)
	{

	}

	void Room::Finalize()
	{
		for(std::size_t i = 0; i < backgrounds.size(); ++i)
			backgrounds[i]->Finalize();

		for(std::size_t i = 0; i < views.size(); ++i)
			views[i]->Finalize();

		for(std::size_t i = 0; i < instances.size(); ++i)
			instances[i]->Finalize();

		for(std::size_t i = 0; i < tiles.size(); ++i)
			tiles[i]->Finalize();
	}

	Room::Background::Background(GmkFile* gmk)
		: GmkResource(gmk),
		  visible(false),
		  foreground(false),
		  image(NULL),
		  imageIndex(-1),
		  x(0),
		  y(0),
		  tileHorizontal(true),
		  tileVertical(true),
		  speedHorizontal(0),
		  speedVertical(0),
		  stretch(false)
	{

	}

	Room::Background::~Background()
	{

	}

	void Room::Background::WriteVer81(Stream* stream)
	{
		stream->WriteBoolean(visible);
		stream->WriteBoolean(foreground);
		stream->WriteDword(image != NULL ? image->GetId() : -1);
		stream->WriteDword(x);
		stream->WriteDword(y);
		stream->WriteBoolean(tileHorizontal);
		stream->WriteBoolean(tileVertical);
		stream->WriteDword(speedHorizontal);
		stream->WriteDword(speedVertical);
		stream->WriteBoolean(stretch);
	}

	void Room::Background::ReadVer81(Stream* stream)
	{
		visible			= stream->ReadBoolean();
		foreground		= stream->ReadBoolean();
		imageIndex		= stream->ReadDword();
		x				= stream->ReadDword();
		y				= stream->ReadDword();
		tileHorizontal	= stream->ReadBoolean();
		tileVertical	= stream->ReadBoolean();
		speedHorizontal	= stream->ReadDword();
		speedVertical	= stream->ReadDword();
		stretch			= stream->ReadBoolean();
	}

	void Room::Background::WriteVer7(Stream* stream)
	{

	}

	void Room::Background::ReadVer7(Stream* stream)
	{

	}

	void Room::Background::Finalize()
	{
		image = (imageIndex != -1) ? gmkHandle->backgrounds[imageIndex] : NULL;
	}

	Room::View::View(GmkFile* gmk)
		: GmkResource(gmk),
		  visible(false),
		  viewX(0),
		  viewY(0),
		  viewW(640),
		  viewH(480),
		  portX(0),
		  portY(0),
		  portW(640),
		  portH(640),
		  horizontalBorder(32),
		  verticalBorder(32),
		  horizontalSpeed(-1),
		  verticalSpeed(-1),
		  objectFollowing(NULL),
		  objectFollowingIndex(-1)
	{

	}

	Room::View::~View()
	{

	}

	void Room::View::WriteVer81(Stream* stream)
	{
		stream->WriteBoolean(visible);
		stream->WriteDword(viewX);
		stream->WriteDword(viewY);
		stream->WriteDword(viewW);
		stream->WriteDword(viewH);
		stream->WriteDword(portX);
		stream->WriteDword(portY);
		stream->WriteDword(portW);
		stream->WriteDword(portH);
		stream->WriteDword(horizontalBorder);
		stream->WriteDword(verticalBorder);
		stream->WriteDword(horizontalSpeed);
		stream->WriteDword(verticalSpeed);
		stream->WriteDword(objectFollowing != NULL ? objectFollowing->GetId() : -1); // -100?
	}

	void Room::View::ReadVer81(Stream* stream)
	{
		visible					= stream->ReadBoolean();
		viewX					= stream->ReadDword();
		viewY					= stream->ReadDword();
		viewW					= stream->ReadDword();
		viewH					= stream->ReadDword();
		portX					= stream->ReadDword();
		portY					= stream->ReadDword();
		portW					= stream->ReadDword();
		portH					= stream->ReadDword();
		horizontalBorder		= stream->ReadDword();
		verticalBorder			= stream->ReadDword();
		horizontalSpeed			= stream->ReadDword();
		verticalSpeed			= stream->ReadDword();
		objectFollowingIndex	= stream->ReadDword();
	}

	void Room::View::WriteVer7(Stream* stream)
	{

	}

	void Room::View::ReadVer7(Stream* stream)
	{

	}

	void Room::View::Finalize()
	{
		objectFollowing = (objectFollowingIndex != -1) ? gmkHandle->objects[objectFollowingIndex] : NULL;
	}

	Room::Instance::Instance(GmkFile* gmk)
		: GmkResource(gmk),
		  x(0),
		  y(0),
		  object(NULL),
		  objectIndex(-1),
		  id(0),
		  creationCode(""),
		  locked(false)
	{

	}

	Room::Instance::~Instance()
	{

	}

	void Room::Instance::WriteVer81(Stream* stream)
	{
		stream->WriteDword(x);
		stream->WriteDword(y);
		stream->WriteDword(object != NULL ? object->GetId() : -1);
		stream->WriteDword(id);
		stream->WriteString(creationCode);
		stream->WriteBoolean(locked);
	}

	void Room::Instance::ReadVer81(Stream* stream)
	{
		x				= stream->ReadDword();
		y				= stream->ReadDword();
		objectIndex		= stream->ReadDword();
		id				= stream->ReadDword();
		creationCode	= stream->ReadString();
		locked			= stream->ReadBoolean();
	}

	void Room::Instance::WriteVer7(Stream* stream)
	{

	}

	void Room::Instance::ReadVer7(Stream* stream)
	{

	}

	void Room::Instance::Finalize()
	{
		object = (objectIndex != -1) ? gmkHandle->objects[objectIndex] : NULL;
	}

	Room::Tile::Tile(GmkFile* gmk)
		: GmkResource(gmk),
		  x(0),
		  y(0),
		  background(NULL),
		  backgroundIndex(-1),
		  tileX(0),
		  tileY(0),
		  width(0),
		  height(0),
		  layer(1000000),
		  id(0),
		  locked(false)
	{

	}

	Room::Tile::~Tile()
	{

	}

	void Room::Tile::WriteVer81(Stream* stream)
	{
		stream->WriteDword(x);
		stream->WriteDword(y);
		stream->WriteDword(background != NULL ? background->GetId() : -1);
		stream->WriteDword(tileX);
		stream->WriteDword(tileY);
		stream->WriteDword(width);
		stream->WriteDword(height);
		stream->WriteDword(layer);
		stream->WriteDword(id);
		stream->WriteBoolean(locked);
	}

	void Room::Tile::ReadVer81(Stream* stream)
	{
		x					= stream->ReadDword();
		y					= stream->ReadDword();
		backgroundIndex		= stream->ReadDword();
		tileX				= stream->ReadDword();
		tileY				= stream->ReadDword();
		width				= stream->ReadDword();
		height				= stream->ReadDword();
		layer				= stream->ReadDword();
		id					= stream->ReadDword();
		locked				= stream->ReadBoolean();
	}

	void Room::Tile::WriteVer7(Stream* stream)
	{

	}

	void Room::Tile::ReadVer7(Stream* stream)
	{

	}

	void Room::Tile::Finalize()
	{
		background = (backgroundIndex != -1) ? gmkHandle->backgrounds[backgroundIndex] : NULL;
	}
}
