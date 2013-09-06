/*
 *	gmkobject.cpp
 *	GMK Object
 */

#include <gmkobject.hpp>
#include <gmk.hpp>

namespace Gmk
{
	Object::Object(GmkFile* gmk)
		: GmkResource(gmk),
		  spriteIndex(SpriteIndexNone),
		  solid(false),
		  visible(true),
		  depth(0),
		  persistent(false),
		  parentIndex(ParentIndexNone),
		  maskIndex(MaskIndexNone),
		  events(),
		  sprite(NULL),
		  mask(NULL),
		  parent(NULL)
	{
		
	}

	Object::~Object()
	{
		for(std::size_t i = 0; i < events.size(); ++i)
			delete events[i];
	}

	int Object::GetId() const
	{
		for(std::size_t i = 0; i < gmkHandle->objects.size(); ++i)
			if (gmkHandle->objects[i] == this)
				return i;

		return -1;
	}

	void Object::WriteVer81(Stream* stream)
	{
		Stream* objectStream = new Stream();

		objectStream->WriteBoolean(exists);
		if (exists)
		{
			objectStream->WriteString(name);
			objectStream->WriteTimestamp();
			objectStream->WriteDword(430);
			objectStream->WriteDword(sprite != NULL ? sprite->GetId() : SpriteIndexNone);
			objectStream->WriteBoolean(solid);
			objectStream->WriteBoolean(visible);
			objectStream->WriteDword(depth);
			objectStream->WriteBoolean(persistent);
			objectStream->WriteDword(parent != NULL ? parent->GetId() : ParentIndexNone);
			objectStream->WriteDword(mask != NULL ? mask->GetId() : MaskIndexNone);

			objectStream->WriteDword(11);
			for(std::size_t i = 0; i < 12; ++i)
			{
				for(std::size_t j = 0; j < events.size(); ++j)
				{
					if (events[j]->eventNumber == i)
					{
						objectStream->WriteDword(events[j]->eventKind);
						objectStream->WriteDword(400);

						objectStream->WriteDword(events[j]->actions.size());
						for(std::size_t k = 0; k < events[j]->actions.size(); ++k)
							events[j]->actions[k]->Write(objectStream);
					}
				}

				objectStream->WriteDword(-1);
			}
		}

		stream->Serialize(objectStream);
		delete objectStream;
	}

	void Object::ReadVer81(Stream* stream)
	{
		Stream* objectStream = stream->Deserialize();

		if (!objectStream->ReadBoolean())
		{
			delete objectStream;
			exists = false;
			return;
		}

		name				= objectStream->ReadString();
		objectStream->ReadTimestamp();
		objectStream->ReadDword();
		spriteIndex			= objectStream->ReadDword();
		solid				= objectStream->ReadBoolean();
		visible				= objectStream->ReadBoolean();
		depth				= objectStream->ReadDword();
		persistent			= objectStream->ReadBoolean();
		parentIndex			= objectStream->ReadDword();
		maskIndex			= objectStream->ReadDword();

		unsigned int count = objectStream->ReadDword() + 1;
		for(unsigned int i = 0; i < count; ++i)
		{
			for(;;)
			{
				int first = objectStream->ReadDword();
				if (first == -1)
					break;

				Event* event = new Event();
				event->eventNumber = i;
				event->eventKind = first;

				objectStream->ReadDword();

				unsigned int actionCount = objectStream->ReadDword();
				while(actionCount--)
				{
					Action* action = new Action(gmkHandle);
					action->Read(objectStream);

					event->actions.push_back(action);
				}

				events.push_back(event);
			}
		}

		delete objectStream;
		exists = true;
	}

	void Object::WriteVer7(Stream* stream)
	{

	}

	void Object::ReadVer7(Stream* stream)
	{

	}

	void Object::Finalize()
	{
		sprite = (spriteIndex != SpriteIndexNone) ? gmkHandle->sprites[spriteIndex] : NULL;
		parent = (parentIndex != ParentIndexNone) ? gmkHandle->objects[parentIndex] : NULL;
		mask = (maskIndex != MaskIndexNone) ? gmkHandle->sprites[maskIndex] : NULL;

		for(std::size_t i = 0; i < events.size(); ++i)
		{
			for(std::size_t j = 0; j < events[i]->actions.size(); ++j)
				events[i]->actions[j]->Finalize();
		}
	}

	Object::Event::Event()
		: eventNumber(0),
		  eventKind(0),
		  actions()
	{

	}

	Object::Event::~Event()
	{
		for(std::size_t i = 0; i < actions.size(); ++i)
			delete actions[i];
	}
}
