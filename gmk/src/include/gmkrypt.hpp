/**
* @file  gmkrypt.hpp
* @brief GMKrypt support
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
