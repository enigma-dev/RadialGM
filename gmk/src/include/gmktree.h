/**
* @file  gmktree.h
* @brief GMK Resource Tree
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

#ifndef __GMK_TREE_H
#define __GMK_TREE_H

#include <gmkresource.h>

namespace Gmk
{
	class Tree : public GmkResource
	{
	public:
		class Node
		{
		public:
			enum Status
			{
				StatusPrimary = 1,
				StatusGroup,
				StatusSecondary
			};

			enum Group
			{
				GroupObjects = 1,
				GroupSprites,
				GroupSounds,
				GroupRooms,
				GroupBackgrounds = 6,
				GroupScripts,
				GroupPaths,
				GroupDataFiles,
				GroupFonts = GroupDataFiles,
				GroupGameInformation,
				GroupGameOptions,
				GroupGlobalGameOptions = GroupGameOptions,
				GroupTimelines,
				GroupExtensionPackages,
				GroupCount
			};

		private:
			unsigned int index;

		public:
			std::string			name;
			unsigned int		status;
			unsigned int		group;
			GmkResource*		resource;
			std::vector<Node*>	contents;

			Node(unsigned int _status, unsigned int _group, unsigned int _index, const std::string& _name);
			~Node();

			void Finalize(GmkResource* parent);

			void AddResource(GmkResource* resource);
			Node* AddFilter(const std::string& value);
		};
	
	private:
		void CleanMemory();
		GmkResource* GetResource(unsigned int id, unsigned int kind) const;

		void ReadRecursiveTree(Stream* stream, Node* parent, unsigned int count);
		void WriteRecursiveTree(Stream* stream, Node* parent, unsigned int count);

	protected:
		void WriteVer81(Stream* stream);
		void ReadVer81(Stream* stream);

		void WriteVer7(Stream* stream);
		void ReadVer7(Stream* stream);

	public:
		std::vector<Node*>			contents;

		Tree(GmkFile* gmk);
		~Tree();

		void Finalize();

		Node* GetBranch(Node::Group group) const;
	};
}

#endif
