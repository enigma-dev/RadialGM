/**
* @file  gmkaction.hpp
* @brief GMK Action
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
#ifndef __GMK_ACTION_HPP
#define __GMK_ACTION_HPP

#include <gmkresource.hpp>

namespace Gmk
{
	class Object;

	class Action : public GmkResource
	{
	public:
		static const unsigned int ARGUMENT_COUNT = 8;

		enum ActionKind
		{
			ActionKindNormal,
			ActionKindBeginGroup,
			ActionKindEndGroup,
			ActionKindElse,
			ActionKindExit,
			ActionKindRepeat,
			ActionKindVariable,
			ActionKindCode
		};

		enum ArgumentKind
		{
			ArgumentKindExpression,
			ArgumentKindString,
			ArgumentKindBoth,
			ArgumentKindBoolean,
			ArgumentKindMenu,
			ArgumentKindSprite,
			ArgumentKindSound,
			ArgumentKindBackground,
			ArgumentKindPath,
			ArgumentKindScript,
			ArgumentKindObject,
			ArgumentKindRoom,
			ArgumentKindFont,
			ArgumentKindColor,
			ArgumentKindTimeline,
			ArgumentKindFontString,
			ArgumentKindCount
		};

		enum ActionType
		{
			AtNothing,
			AtFunction,
			AtCode
		};

		enum AppliesTo
		{
			ApObject		= 0,												// >= 0 refers to an object index
			ApSelf			= -1,
			ApOther			= -2
		};

	protected:
		void WriteVer81(Stream* stream);
		void ReadVer81(Stream* stream);

		void WriteVer7(Stream* stream);
		void ReadVer7(Stream* stream);

	public:
		std::string				functionName;									// Function to execute (action_if_variable)
		std::string				functionCode;									// Code to execute (show_message(argument0))
		GmkResource*			argumentLink[ARGUMENT_COUNT];					// Logical link to objects, for runtime manipulation
		std::string				argumentValue[ARGUMENT_COUNT];					// Argument values
  		unsigned int			libraryId;										// Library ID (All official have ID 1)
		unsigned int			actionId;										// Action ID (Identifies D&D action)
		unsigned int			kind;											// Normal, Begin, End, Else, Exit, etc
		unsigned int			type;											// Executes function or code
		unsigned int			argumentsUsed;									// Arguments actually passed
		unsigned int			argumentKind[ARGUMENT_COUNT];					// Argument types (expression, sprite, background, object, etc)
		int						appliesToObject;								// "Applies to: self/other/object id"
		Object*					appliesObject;									// Object reference if appliesToObject >= ApObject
		bool					relative;										// Relative flag
		bool					appliesToSomething;								// "Applies to" visible?
		bool					question;										// Question flag
		bool					mayBeRelative;									// "Relative" visible?
		bool					notFlag;										// Not flag

		Action(GmkFile* gmk);
		~Action();

		void Finalize();

		void SetCode(const std::string& value);
		std::string GetCode() const;

		GmkResource* GetArgumentReference(unsigned int index) const;
	};
}

#endif
