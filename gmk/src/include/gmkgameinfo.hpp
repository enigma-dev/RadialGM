/*
 *	gmkgameinfo.hpp
 *	GMK Game Information
 */

#ifndef __GMK_GAME_INFORMATION_HPP
#define __GMK_GAME_INFORMATION_HPP

#include <gmkresource.hpp>

namespace Gmk
{
	class GameInformation : public GmkResource
	{
	protected:
		void WriteVer81(Stream* stream);
		void ReadVer81(Stream* stream);

		void WriteVer7(Stream* stream);
		void ReadVer7(Stream* stream);

	public:
		unsigned int			backgroundColor;
		bool					showInSeperateWindow;
		std::string				caption;
		int						left;
		int						top;
		int						width;
		int						height;
		bool					showBorder;
		bool					sizeable;
		bool					alwaysOnTop;
		bool					freeze;
		std::string				information;

		GameInformation(GmkFile* gmk);
		~GameInformation();
	};
}

#endif
