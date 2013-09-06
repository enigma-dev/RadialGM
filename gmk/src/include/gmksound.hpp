/*
 *	gmksound.hpp
 *	GMK Sound
 */

#ifndef __GMK_SOUND_HPP
#define __GMK_SOUND_HPP

#include <gmkresource.hpp>

namespace Gmk
{
	class Sound : public GmkResource
	{
	public:
		enum Kind
		{
			KindNormal,
			KindBackground,
			Kind3D,
			KindMultimedia
		};

		enum Effect
		{
			EffectNone				= 0x00,
			EffectChorus			= 0x01,
			EffectEcho				= 0x02,
			EffectFlanger			= 0x04,
			EffectGargle			= 0x08,
			EffectReverb			= 0x10
		};

	protected:
		void WriteVer81(Stream* stream);
		void ReadVer81(Stream* stream);

		void WriteVer7(Stream* stream);
		void ReadVer7(Stream* stream);

	public:
		unsigned int			kind;
		std::string				extension;
		std::string				filename;
		Stream*					data;
		unsigned int			effects;
		double					volume;
		double					pan;
		bool					preload;

		Sound(GmkFile* gmk);
		~Sound();

		int GetId() const;
	};
}

#endif
