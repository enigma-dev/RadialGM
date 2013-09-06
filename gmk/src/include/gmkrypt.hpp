/*
 *	gmkrypt.hpp
 *	GMKrypt support
 */

#ifndef __GMK_GMKRYPT_HPP
#define __GMK_GMKRYPT_HPP

#include <stream.hpp>

namespace Gmk
{
	class Gmkrypt
	{
	private:
		int seed;
		int table[2][256];

		void GenerateSwapTable();

	public:
		Gmkrypt(int _seed);
		~Gmkrypt();

		Stream* Encrypt(Stream* stream);
		Stream* Decrypt(Stream* stream);

		static int ReadSeedFromJunkyard(Stream* stream);
	};
}

#endif
