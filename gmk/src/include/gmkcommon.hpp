/*
 *	gmkcommon.hpp
 *	GMK Common Definitions
 */

#ifndef __GMK_COMMON_HPP
#define __GMK_COMMON_HPP

#include <gmkexception.hpp>

namespace Gmk
{
	typedef enum _Version
	{
		VerUnknown,
		Ver53a,
		Ver61,
		Ver7,
		Ver8,
		Ver81								// GM 8.1.141 (r11549)
	} Version;
}

#endif
