/*
 *	gmkfont.hpp
 *	GMK Font
 */

#ifndef __GMK_FONT_HPP
#define __GMK_FONT_HPP

#include <gmkresource.hpp>

namespace Gmk
{
	class Font : public GmkResource
	{
	public:
		enum AntiAliasingLevel
		{
			AaOff = 1,
			Aa1,
			Aa2,
			Aa3
		};

		// TODO Sort of a hack
#ifndef ANSI_CHARSET
		enum CharacterSet
		{
			ANSI_CHARSET		= 0x00,
			DEFAULT_CHARSET		= 0x00,
			EASTEUROPE_CHARSET	= 0xEE,
			RUSSIAN_CHARSET		= 0xCC,
			SYMBOL_CHARSET		= 0x02,
			SHIFTJIS_CHARSET	= 0x80,
			HANGEUL_CHARSET		= 0x81,
			GB2312_CHARSET		= 0x86,
			CHINESEBIG5_CHARSET	= 0x88,
			JOHAB_CHARSET		= 0x82,
			HEWBREW_CHARSET		= 0xB1,
			ARABIC_CHARSET		= 0xB2,
			GREEK_CHARSET		= 0xA1,
			TURKISH_CHARSET		= 0xA2,
			VIETNAMESE_CHARSET	= 0xA3,
			THAI_CHARSET		= 0xDE,
			MAC_CHARSET			= 0x4D,
			BALTIC_CHARSET		= 0xBA,
			OEM_CHARSET			= 0xFF
		};
#endif

	protected:
		void WriteVer81(Stream* stream);
		void ReadVer81(Stream* stream);

		void WriteVer7(Stream* stream);
		void ReadVer7(Stream* stream);

	public:
		std::string			fontName;
		unsigned int		size;
		bool				bold;
		bool				italic;
		unsigned int		characterRangeBegin;
		unsigned int		characterRangeEnd;
		unsigned int		characterSet;
		unsigned int		antiAliasing;

		Font(GmkFile* gmk);
		~Font();

		int GetId() const;
	};
}

#endif
