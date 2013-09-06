/*
 *	gmkexception.cpp
 *	GMK Exception
 */

#include <gmkexception.hpp>

namespace Gmk
{
	GmkException::GmkException(const std::string& _message)
		: message(_message)
	{

	}

	GmkException::~GmkException() throw()
	{

	}

	const char* GmkException::what() const throw()
	{
		return message.c_str();
	}
}
