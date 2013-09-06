/*
 *	gmksprite.cpp
 *	GMK Sprite
 */

#include <gmksprite.hpp>
#include <gmk.hpp>

namespace Gmk
{
	Sprite::Sprite(GmkFile* gmk)
		: GmkResource(gmk),
		  width(0),
		  height(0),
		  bboxLeft(0),
		  bboxRight(0),
		  bboxBottom(0),
		  bboxTop(0),
		  originX(0),
		  originY(0),
		  subimages(),
		  maskShape(ShapePrecise),
		  alphaTolerance(0),
		  preciseCollisionChecking(true),
		  seperateMasks(false),
		  transparent(true),
		  smoothEdges(false),
		  boundingBox(BbAutomatic),
		  preload(true)
	{

	}

	Sprite::~Sprite()
	{
		for(std::size_t i = 0; i < subimages.size(); ++i)
		{
			if (subimages[i].data == NULL)
				continue;

			delete subimages[i].data;
		}
	}

	int Sprite::GetId() const
	{
		for(std::size_t i = 0; i < gmkHandle->sprites.size(); ++i)
			if (gmkHandle->sprites[i] == this)
				return i;

		return -1;
	}

	void Sprite::WriteVer81(Stream* stream)
	{
		Stream* spriteStream = new Stream();

		spriteStream->WriteBoolean(exists);
		if (exists)
		{
			spriteStream->WriteString(name);
			spriteStream->WriteTimestamp();
			spriteStream->WriteDword(800);
			spriteStream->WriteDword(originX);
			spriteStream->WriteDword(originY);
		
			spriteStream->WriteDword(subimages.size());
			for(std::size_t i = 0; i < subimages.size(); ++i)
			{
				spriteStream->WriteDword(800);
				spriteStream->WriteDword(subimages[i].width);
				spriteStream->WriteDword(subimages[i].height);

				if (subimages[i].width != 0 && subimages[i].height != 0)
					spriteStream->Serialize(subimages[i].data, false);
			}

			spriteStream->WriteDword(maskShape);
			spriteStream->WriteDword(alphaTolerance);
			spriteStream->WriteBoolean(seperateMasks);
			spriteStream->WriteDword(boundingBox);
			spriteStream->WriteDword(bboxLeft);
			spriteStream->WriteDword(bboxRight);
			spriteStream->WriteDword(bboxBottom);
			spriteStream->WriteDword(bboxTop);
		}

		stream->Serialize(spriteStream);
		delete spriteStream;
	}

	void Sprite::ReadVer81(Stream* stream)
	{
		Stream* spriteStream = stream->Deserialize();

		if (!spriteStream->ReadBoolean())
		{
			delete spriteStream;
			exists = false;
			return;
		}

		name					= spriteStream->ReadString();
		spriteStream->ReadTimestamp();
		spriteStream->ReadDword();
		originX					= spriteStream->ReadDword();
		originY					= spriteStream->ReadDword();

		unsigned int count = spriteStream->ReadDword();
		while(count--)
		{
			Subimage subimage;

			spriteStream->ReadDword();
			subimage.width = spriteStream->ReadDword();
			subimage.height = spriteStream->ReadDword();

			if (subimage.width != 0 && subimage.height != 0)
				subimage.data = spriteStream->Deserialize(false);
			else
				subimage.data = NULL;

			subimages.push_back(subimage);

			width = subimage.width;
			height = subimage.height;
		}

		maskShape				= spriteStream->ReadDword();
		alphaTolerance			= spriteStream->ReadDword();
		seperateMasks			= spriteStream->ReadBoolean();
		boundingBox				= spriteStream->ReadDword();
		bboxLeft				= spriteStream->ReadDword();
		bboxRight				= spriteStream->ReadDword();
		bboxBottom				= spriteStream->ReadDword();
		bboxTop					= spriteStream->ReadDword();

		delete spriteStream;
		exists = true;
	}

	void Sprite::WriteVer7(Stream* stream)
	{

	}

	void Sprite::ReadVer7(Stream* stream)
	{
		if (!stream->ReadBoolean())
		{
			exists = false;
			return;
		}

		name			= stream->ReadString();
		stream->ReadDword();
		width			= stream->ReadDword();
		height			= stream->ReadDword();
		bboxLeft		= stream->ReadDword();
		bboxRight		= stream->ReadDword();
		bboxBottom		= stream->ReadDword();
		bboxTop			= stream->ReadDword();
		transparent		= stream->ReadBoolean();
		smoothEdges		= stream->ReadBoolean();
		preload			= stream->ReadBoolean();
		boundingBox		= stream->ReadDword();
		preciseCollisionChecking = stream->ReadBoolean();
		originX			= stream->ReadDword();
		originY			= stream->ReadDword();

		unsigned int count = stream->ReadDword();
		while(count--)
		{
			Subimage subimage;
			subimage.width = width;
			subimage.height = height;
			subimage.data = stream->ReadBitmapOld();

			subimages.push_back(subimage);
		}

		exists = true;
	}
}
