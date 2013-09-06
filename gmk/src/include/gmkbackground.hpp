/*
 *	gmkbackground.hpp
 *	GMK Background
 */

#ifndef __GMK_BACKGROUND_HPP
#define __GMK_BACKGROUND_HPP

#include <gmkresource.hpp>

namespace Gmk
{
	class Background : public GmkResource
	{
	protected:
		void WriteVer81(Stream* stream);
		void ReadVer81(Stream* stream);

		void WriteVer7(Stream* stream);
		void ReadVer7(Stream* stream);

	public:
		bool					transparent;
		bool					smoothEdges;
		bool					preload;
		bool					useAsTileset;
		unsigned int			tileWidth;
		unsigned int			tileHeight;
		unsigned int			tileHorizontalOffset;
		unsigned int			tileVerticalOffset;
		unsigned int			tileHorizontalSeperation;
		unsigned int			tileVerticalSeperation;
		unsigned int			width;
		unsigned int			height;
		Stream*					data;

		Background(GmkFile* gmk);
		~Background();

		int GetId() const;
	};
}

#endif
