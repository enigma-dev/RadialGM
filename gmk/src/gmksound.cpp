/**
* @file  gmksound.cpp
* @brief GMK Sound
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

#include <gmksound.hpp>
#include <gmk.hpp>

namespace Gmk
{
	Sound::Sound(GmkFile* gmk)
		: GmkResource(gmk),
		  kind(KindNormal),
		  extension(""),
		  filename(""),
		  data(NULL),
		  effects(EffectNone),
		  volume(1.0),
		  pan(0.0),
		  preload(true)
	{

	}

	Sound::~Sound()
	{
		if (data != NULL)
			delete data;
	}

	int Sound::GetId() const
	{
		for(std::size_t i = 0; i < gmkHandle->sounds.size(); ++i)
			if (gmkHandle->sounds[i] == this)
				return i;

		return -1;
	}

	void Sound::WriteVer81(Stream* stream)
	{
		Stream* soundStream = new Stream();

		soundStream->WriteBoolean(exists);
		if (exists)
		{
			soundStream->WriteString(name);
			soundStream->WriteTimestamp();
			soundStream->WriteDword(800);
			soundStream->WriteDword(kind);
			soundStream->WriteString(extension);
			soundStream->WriteString(filename);
		
			if (data != NULL)
			{
				soundStream->WriteBoolean(true);
				soundStream->Serialize(data, false);
			}
			else
				soundStream->WriteBoolean(false);

			soundStream->WriteDword(effects);
			soundStream->WriteDouble(volume);
			soundStream->WriteDouble(pan);
			soundStream->WriteBoolean(preload);
		}

		stream->Serialize(soundStream);
		delete soundStream;
	}

	void Sound::ReadVer81(Stream* stream)
	{
		Stream* soundStream = stream->Deserialize();

		if (!soundStream->ReadBoolean())
		{
			delete soundStream;
			exists = false;
			return;
		}

		name			= soundStream->ReadString();
		soundStream->ReadTimestamp();
		soundStream->ReadDword();
		kind			= soundStream->ReadDword();
		extension		= soundStream->ReadString();
		filename		= soundStream->ReadString();
		if (soundStream->ReadBoolean())
			data = soundStream->Deserialize(false);
		effects			= soundStream->ReadDword();
		volume			= soundStream->ReadDouble();
		pan				= soundStream->ReadDouble();
		preload			= soundStream->ReadBoolean();

		delete soundStream;
		exists = true;
	}

	void Sound::WriteVer7(Stream* stream)
	{

	}

	void Sound::ReadVer7(Stream* stream)
	{
		if (!stream->ReadBoolean())
		{
			exists = false;
			return;
		}

		name			= stream->ReadString();
		stream->ReadDword();
		kind			= stream->ReadDword();
		extension		= stream->ReadString();
		filename		= stream->ReadString();

		if (stream->ReadBoolean())
			data = stream->Deserialize(false);

		effects			= stream->ReadDword();
		volume			= stream->ReadDouble();
		pan				= stream->ReadDouble();
		preload			= stream->ReadBoolean();

		exists = true;
	}
}
