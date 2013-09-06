/*
 *	gmkincludefile.hpp
 *	GMK Include File
 */

#ifndef __GMK_INCLUDE_FILE_HPP
#define __GMK_INCLUDE_FILE_HPP

#include <gmkresource.hpp>

namespace Gmk
{
	class IncludeFile : public GmkResource
	{
	public:
		enum ExportKind
		{
			EkDont,
			EkTempDirectory,
			EkWorkingDirectory,
			EkFollowingFolder
		};

	protected:
		void WriteVer81(Stream* stream);
		void ReadVer81(Stream* stream);

		void WriteVer7(Stream* stream);
		void ReadVer7(Stream* stream);

	public:
		std::string				filename;
		std::string				filepath;
		bool					originalFile;
		unsigned int			originalFileSize;
		Stream*					data;
		unsigned int			exportKind;
		std::string				exportPath;
		bool					overwrite;
		bool					freeMemory;
		bool					removeAtEndOfGame;

		IncludeFile(GmkFile* gmk);
		~IncludeFile();
	};
}

#endif
