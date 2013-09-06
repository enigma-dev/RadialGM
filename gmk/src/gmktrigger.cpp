/*
 *	gmktrigger.cpp
 *	GMK Trigger
 */

#include <gmktrigger.hpp>

namespace Gmk
{
	Trigger::Trigger(GmkFile* gmk)
		: GmkResource(gmk),
		  condition(""),
		  momentOfChecking(MomentBegin),
		  constantName("")
	{
		
	}

	Trigger::~Trigger()
	{
		
	}

	void Trigger::WriteVer81(Stream* stream)
	{
		Stream* writeStream = new Stream();

		writeStream->WriteBoolean(exists);
		if (exists)
		{
			writeStream->WriteDword(800);

			writeStream->WriteString(name);
			writeStream->WriteString(condition);
			writeStream->WriteDword(momentOfChecking);
			writeStream->WriteString(constantName);
		}
		
		stream->Serialize(writeStream);
		delete writeStream;
	}

	void Trigger::ReadVer81(Stream* stream)
	{
		Stream* triggerStream = stream->Deserialize();

		if (!triggerStream->ReadBoolean())
		{
			delete triggerStream;
			exists = false;
			return;
		}

		triggerStream->ReadDword();
		name				= triggerStream->ReadString();
		condition			= triggerStream->ReadString();
		momentOfChecking	= triggerStream->ReadDword();
		constantName		= triggerStream->ReadString();

		delete triggerStream;
		exists = true;
	}

	void Trigger::WriteVer7(Stream* stream)
	{

	}

	void Trigger::ReadVer7(Stream* stream)
	{

	}
}
