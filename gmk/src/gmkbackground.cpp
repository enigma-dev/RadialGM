/*
 *	gmkbackground.cpp
 *	GMK Background
 */

#include <gmkbackground.hpp>
#include <gmk.hpp>

namespace Gmk
{
	Background::Background(GmkFile* gmk)
		: GmkResource(gmk),
		  transparent(false),
		  smoothEdges(false),
		  preload(false),
		  useAsTileset(false),
		  tileWidth(16),
		  tileHeight(16),
		  tileHorizontalOffset(0),
		  tileVerticalOffset(0),
		  tileHorizontalSeperation(0),
		  tileVerticalSeperation(0),
		  width(0),
		  height(0),
		  data(NULL)
	{
		
	}

	Background::~Background()
	{
		if (data != NULL)
			delete data;
	}

	int Background::GetId() const
	{
		for(std::size_t i = 0; i < gmkHandle->backgrounds.size(); ++i)
			if (gmkHandle->backgrounds[i] == this)
				return i;

		return -1;
	}

	void Background::WriteVer81(Stream* stream)
	{
		Stream* backgroundStream = new Stream();

		backgroundStream->WriteBoolean(exists);
		if (exists)
		{
			backgroundStream->WriteString(name);
			backgroundStream->WriteTimestamp();
			backgroundStream->WriteDword(710);
			backgroundStream->WriteBoolean(useAsTileset);
			backgroundStream->WriteDword(tileWidth);
			backgroundStream->WriteDword(tileHeight);
			backgroundStream->WriteDword(tileHorizontalOffset);
			backgroundStream->WriteDword(tileVerticalOffset);
			backgroundStream->WriteDword(tileHorizontalSeperation);
			backgroundStream->WriteDword(tileVerticalSeperation);
			backgroundStream->WriteDword(800);
			backgroundStream->WriteDword(width);
			backgroundStream->WriteDword(height);

			if (width != 0 && height != 0)
				backgroundStream->Serialize(data, false);
		}

		stream->Serialize(backgroundStream);
		delete backgroundStream;
	}

	void Background::ReadVer81(Stream* stream)
	{
		Stream* backgroundStream = stream->Deserialize();

		if (!backgroundStream->ReadBoolean())
		{
			delete backgroundStream;
			exists = false;
			return;
		}

		name						= backgroundStream->ReadString();
		backgroundStream->ReadTimestamp();
		backgroundStream->ReadDword();
		useAsTileset				= backgroundStream->ReadBoolean();
		tileWidth					= backgroundStream->ReadDword();
		tileHeight					= backgroundStream->ReadDword();
		tileHorizontalOffset		= backgroundStream->ReadDword();
		tileVerticalOffset			= backgroundStream->ReadDword();
		tileHorizontalSeperation	= backgroundStream->ReadDword();
		tileVerticalSeperation		= backgroundStream->ReadDword();
		backgroundStream->ReadDword();
		width						= backgroundStream->ReadDword();
		height						= backgroundStream->ReadDword();

		if (width != 0 && height != 0)
			data = backgroundStream->Deserialize(false);

		delete backgroundStream;
		exists = true;
	}

	void Background::WriteVer7(Stream* stream)
	{

	}

	void Background::ReadVer7(Stream* stream)
	{
		if (!stream->ReadBoolean())
		{
			exists = false;
			return;
		}

		name						= stream->ReadString();
		stream->ReadDword();
		width						= stream->ReadDword();
		height						= stream->ReadDword();
		transparent					= stream->ReadBoolean();
		smoothEdges					= stream->ReadBoolean();
		preload						= stream->ReadBoolean();
		useAsTileset				= stream->ReadBoolean();
		tileWidth					= stream->ReadDword();
		tileHeight					= stream->ReadDword();
		tileHorizontalOffset		= stream->ReadDword();
		tileVerticalOffset			= stream->ReadDword();
		tileHorizontalSeperation	= stream->ReadDword();
		tileVerticalSeperation		= stream->ReadDword();

		if (stream->ReadBoolean())
			data = stream->ReadBitmapOld();

		exists = true;
	}
}
