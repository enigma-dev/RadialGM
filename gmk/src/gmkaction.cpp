/*
 *	gmkaction.cpp
 *	GMK Action
 */

#include <cstdlib>
#include <sstream>
#include <gmkaction.hpp>
#include <gmk.hpp>

namespace Gmk
{
	Action::Action(GmkFile* gmk)
		: GmkResource(gmk),
		  functionName(""),
		  functionCode(""),
		  argumentValue(),
		  libraryId(0),
		  actionId(0),
		  kind(0),
		  type(0),
		  argumentsUsed(0),
		  appliesToObject(ApSelf),
		  appliesObject(NULL),
		  relative(false),
		  appliesToSomething(false),
		  question(false),
		  mayBeRelative(false),
		  notFlag(false)
	{
		for(unsigned int i = 0; i < ARGUMENT_COUNT; ++i)
		{
			argumentValue[i] = "";
			argumentLink[i] = NULL;
			argumentKind[i] = ArgumentKindExpression;
		}
	}

	Action::~Action()
	{

	}

	void Action::SetCode(const std::string& value)
	{
		if (kind != ActionKindCode)
			return;

		argumentValue[0] = value;
	}

	std::string Action::GetCode() const
	{
		return kind == ActionKindCode ? argumentValue[0] : "";
	}

	GmkResource* Action::GetArgumentReference(unsigned int index) const
	{
		// More magic!
		static const unsigned int akKinds[ArgumentKindCount] =
		{
			RtUnknown,
			RtUnknown,
			RtUnknown,
			RtUnknown,
			RtUnknown,
			RtSprite,
			RtSound,
			RtBackground,
			RtPath,
			RtScript,
			RtObject,
			RtRoom,
			RtFont,
			RtUnknown,
			RtTimeline
		};

		return (index < ARGUMENT_COUNT) ? GetResource(akKinds[argumentKind[index]], std::atoi(argumentValue[index].c_str())) : NULL;
	}

	void Action::WriteVer81(Stream* stream)
	{
		stream->WriteDword(440);

		stream->WriteDword(libraryId);
		stream->WriteDword(actionId);
		stream->WriteDword(kind);
		stream->WriteBoolean(mayBeRelative);
		stream->WriteBoolean(question);
		stream->WriteBoolean(appliesToSomething);
		stream->WriteDword(type);
		stream->WriteString(functionName);
		stream->WriteString(functionCode);
		stream->WriteDword(argumentsUsed);

		stream->WriteDword(ARGUMENT_COUNT);
		for(unsigned int i = 0; i < ARGUMENT_COUNT; ++i)
			stream->WriteDword(argumentKind[i]);

		if (appliesObject == NULL)
			stream->WriteDword(appliesToObject);
		else
			stream->WriteDword(appliesObject->GetId());

		stream->WriteBoolean(relative);

		stream->WriteDword(ARGUMENT_COUNT);
		for(unsigned int i = 0; i < ARGUMENT_COUNT; ++i)
		{
			if (argumentLink[i] != NULL)
			{
				std::stringstream ss;
				ss << std::dec << argumentLink[i]->GetId();
				stream->WriteString(ss.str());
			}
			else
				stream->WriteString(argumentValue[i]);
		}

		stream->WriteBoolean(notFlag);
	}

	void Action::ReadVer81(Stream* stream)
	{
		unsigned int count;

		// Skip version
		stream->ReadDword();

		libraryId				= stream->ReadDword();
		actionId				= stream->ReadDword();
		kind					= stream->ReadDword();
		mayBeRelative			= stream->ReadBoolean();
		question				= stream->ReadBoolean();
		appliesToSomething		= stream->ReadBoolean();
		type					= stream->ReadDword();
		functionName			= stream->ReadString();
		functionCode			= stream->ReadString();
		argumentsUsed			= stream->ReadDword();

		count = stream->ReadDword();
		for(unsigned int i = 0; i < count; ++i)
			argumentKind[i] = stream->ReadDword();

		appliesToObject			= stream->ReadDword();
		relative				= stream->ReadBoolean();

		count = stream->ReadDword();
		for(unsigned int i = 0; i < count; ++i)
			argumentValue[i] = stream->ReadString();

		notFlag					= stream->ReadBoolean();
	}

	void Action::WriteVer7(Stream* stream)
	{

	}

	void Action::ReadVer7(Stream* stream)
	{

	}

	void Action::Finalize()
	{
		appliesObject = (appliesToObject >= ApObject) ? static_cast<Object*>(GetResource(RtObject, appliesToObject)) : NULL;

		for(int i = 0; i < ARGUMENT_COUNT; ++i)
			argumentLink[i] = GetArgumentReference(i);
	}
}
