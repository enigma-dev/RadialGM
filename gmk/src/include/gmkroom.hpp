/*
 *	gmkroom.hpp
 *	GMK Room
 */

#ifndef __GMK_ROOM_HPP
#define __GMK_ROOM_HPP

#include <gmkresource.hpp>
#include <gmkbackground.hpp>
#include <gmkobject.hpp>

namespace Gmk
{
	class Room : public GmkResource
	{
	public:
		class Background : public GmkResource
		{
		private:
			int					imageIndex;

		protected:
			void WriteVer81(Stream* stream);
			void ReadVer81(Stream* stream);

			void WriteVer7(Stream* stream);
			void ReadVer7(Stream* stream);

		public:
			bool				visible;
			bool				foreground;
			::Gmk::Background*	image;
			unsigned int		x;
			unsigned int		y;
			bool				tileHorizontal;
			bool				tileVertical;
			int					speedHorizontal;
			int					speedVertical;
			bool				stretch;

			Background(GmkFile* gmk);
			~Background();

			void Finalize();
		};

		class View : public GmkResource
		{
		private:
			int					objectFollowingIndex;

		protected:
			void WriteVer81(Stream* stream);
			void ReadVer81(Stream* stream);

			void WriteVer7(Stream* stream);
			void ReadVer7(Stream* stream);

		public:
			bool				visible;
			unsigned int		viewX;
			unsigned int		viewY;
			unsigned int		viewW;
			unsigned int		viewH;
			unsigned int		portX;
			unsigned int		portY;
			unsigned int		portW;
			unsigned int		portH;
			unsigned int		horizontalBorder;
			unsigned int		verticalBorder;
			int					horizontalSpeed;
			int					verticalSpeed;
			Object*				objectFollowing;

			View(GmkFile* gmk);
			~View();

			void Finalize();
		};

		class Instance : public GmkResource
		{
		private:
			unsigned int		objectIndex;

		protected:
			void WriteVer81(Stream* stream);
			void ReadVer81(Stream* stream);

			void WriteVer7(Stream* stream);
			void ReadVer7(Stream* stream);

		public:
			unsigned int		x;
			unsigned int		y;
			Object*				object;
			unsigned int		id;
			std::string			creationCode;
			bool				locked;

			Instance(GmkFile* gmk);
			~Instance();

			void Finalize();
		};

		class Tile : public GmkResource
		{
		private:
			int					backgroundIndex;

		protected:
			void WriteVer81(Stream* stream);
			void ReadVer81(Stream* stream);

			void WriteVer7(Stream* stream);
			void ReadVer7(Stream* stream);

		public:
			unsigned int		x;
			unsigned int		y;
			::Gmk::Background*	background;
			unsigned int		tileX;
			unsigned int		tileY;
			unsigned int		width;
			unsigned int		height;
			unsigned int		layer;
			unsigned int		id;
			bool				locked;

			Tile(GmkFile* gmk);
			~Tile();

			void Finalize();
		};

	protected:
		void WriteVer81(Stream* stream);
		void ReadVer81(Stream* stream);

		void WriteVer7(Stream* stream);
		void ReadVer7(Stream* stream);

	public:
		std::string					caption;
		unsigned int				width;
		unsigned int				height;
		unsigned int				snapY;
		unsigned int				snapX;
		bool						isometricGrid;
		unsigned int				speed;
		bool						persistent;
		unsigned int				backgroundColor;
		bool						drawBackgroundColor;
		bool						clearBackgroundWithWindowColor;
		std::string					creationCode;
		std::vector<Background*>	backgrounds;
		bool						viewsEnabled;
		std::vector<View*>			views;
		std::vector<Instance*>		instances;
		std::vector<Tile*>			tiles;
		bool						rememberRoomEditorInfo;
		unsigned int				roomEditorWidth;
		unsigned int				roomEditorHeight;
		bool						showGrid;
		bool						showObjects;
		bool						showTiles;
		bool						showBackgrounds;
		bool						showForegrounds;
		bool						showViews;
		bool						deleteUnderlyingObjects;
		bool						deleteUnderlyingTiles;
		unsigned int				tabIndex;
		unsigned int				scrollbarX;
		unsigned int				scrollbarY;
		
		Room(GmkFile* gmk);
		~Room();

		int GetId() const;
		void Finalize();
	};
}

#endif
