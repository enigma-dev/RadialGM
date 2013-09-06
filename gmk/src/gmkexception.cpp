/**
* @file  gmkexception.cpp
* @brief GMK Exception
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
