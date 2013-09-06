/**
* @file  gmktimeline.cpp
* @brief GMK Timeline
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

#include <gmktimeline.hpp>
#include <gmk.hpp>

namespace Gmk
{
	Timeline::Timeline(GmkFile* gmk)
		: GmkResource(gmk),
		  moments()
	{

	}

	Timeline::~Timeline()
	{
		for(std::size_t i = 0; i < moments.size(); ++i)
		{
			for(std::size_t j = 0; j < moments[i].actions.size(); ++j)
				delete moments[i].actions[j];
		}
	}

	int Timeline::GetId() const
	{
		for(std::size_t i = 0; i < gmkHandle->timelines.size(); ++i)
			if (gmkHandle->timelines[i] == this)
				return i;

		return -1;
	}

	void Timeline::WriteVer81(Stream* stream)
	{
		Stream* timelineStream = new Stream();

		timelineStream->WriteBoolean(exists);
		if (exists)
		{
			timelineStream->WriteString(name);
			timelineStream->WriteTimestamp();
			timelineStream->WriteDword(500);

			timelineStream->WriteDword(moments.size());
			for(std::size_t i = 0; i < moments.size(); ++i)
			{
				timelineStream->WriteDword(moments[i].position);

				timelineStream->WriteDword(400);
				timelineStream->WriteDword(moments[i].actions.size());
				for(std::size_t j = 0; j < moments[i].actions.size(); ++j)
					moments[i].actions[j]->Write(timelineStream);
			}
		}

		stream->Serialize(timelineStream);
		delete timelineStream;
	}

	void Timeline::ReadVer81(Stream* stream)
	{
		Stream* timelineStream = stream->Deserialize();

		if (!timelineStream->ReadBoolean())
		{
			delete timelineStream;
			exists = false;
			return;
		}

		name				= timelineStream->ReadString();
		timelineStream->ReadTimestamp();
		timelineStream->ReadDword();

		unsigned int count = timelineStream->ReadDword();
		while(count--)
		{
			Moment moment;
			moment.position = timelineStream->ReadDword();
			
			timelineStream->ReadDword();
			unsigned int actionCount = timelineStream->ReadDword();
			while(actionCount--)
			{
				Action* action = new Action(gmkHandle);
				action->Read(timelineStream);

				moment.actions.push_back(action);
			}

			moments.push_back(moment);
		}

		delete timelineStream;
		exists = true;
	}

	void Timeline::WriteVer7(Stream* stream)
	{

	}

	void Timeline::ReadVer7(Stream* stream)
	{

	}

	void Timeline::Finalize()
	{
		for(std::size_t i = 0; i < moments.size(); ++i)
		{
			for(std::size_t j = 0; j < moments[i].actions.size(); ++j)
				moments[i].actions[j]->Finalize();
		}
	}
}
