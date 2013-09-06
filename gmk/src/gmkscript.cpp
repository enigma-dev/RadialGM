/*
 *	gmkscript.cpp
 *	GMK Script
 */

#include <gmkscript.hpp>
#include <gmk.hpp>

namespace Gmk
{
	Script::Script(GmkFile* gmk)
		: GmkResource(gmk),
		  value("")
	{
		
	}

	Script::~Script()
	{

	}

	int Script::GetId() const
	{
		for(std::size_t i = 0; i < gmkHandle->scripts.size(); ++i)
			if (gmkHandle->scripts[i] == this)
				return i;

		return -1;
	}

	void Script::WriteVer81(Stream* stream)
	{
		Stream* scriptStream = new Stream();

		scriptStream->WriteBoolean(exists);
		if (exists)
		{
			scriptStream->WriteString(name);
			scriptStream->WriteTimestamp();
			scriptStream->WriteDword(800);
			scriptStream->WriteString(value);
		}

		stream->Serialize(scriptStream);
		delete scriptStream;
	}

	void Script::ReadVer81(Stream* stream)
	{
		Stream* scriptStream = stream->Deserialize();

		if (!scriptStream->ReadBoolean())
		{
			delete scriptStream;
			exists = false;
			return;
		}
	
		name = scriptStream->ReadString();
		scriptStream->ReadTimestamp();
		scriptStream->ReadDword();
		value = scriptStream->ReadString();

		delete scriptStream;
		exists = true;
	}

	void Script::WriteVer7(Stream* stream)
	{

	}

	void Script::ReadVer7(Stream* stream)
	{

	}
}
