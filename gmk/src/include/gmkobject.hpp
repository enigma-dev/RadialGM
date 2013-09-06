/*
 *	gmkobject.hpp
 *	GMK Object
 */

#ifndef __GMK_OBJECT_HPP
#define __GMK_OBJECT_HPP

#include <gmkresource.hpp>
#include <gmkaction.hpp>
#include <gmksprite.hpp>

namespace Gmk
{
	class Object : public GmkResource
	{
	public:
		static const int SpriteIndexNone		= -1;
		static const int ParentIndexNone		= -100;
		static const int MaskIndexNone			= -1;

		class Event
		{
		public:
			enum EventNumber
			{
				EnNormal = 0,
				EnStepNormal = 0,
				EnStepBegin = 1,
				EnStepEnd = 2,
				EnDrawNormal = 0,
				EnDrawGUI = 64,
				EnDrawResize = 64,
				EnOtherOutsideRoom = 0,
				EnOtherIntersectBoundary = 1,
				EnOtherGameStart = 2,
				EnOtherGameEnd = 3,
				EnOtherRoomStart = 4,
				EnOtherRoomEnd = 5,
				EnOtherNoMoreLives = 6,
				EnOtherAnimationEnd = 7,
				EnOtherEndOfPath = 8,
				EnOtherNoMoreHealth = 9,
				EnOtherUser0 = 10,
				EnOtherUser1 = 11,
				EnOtherUser2 = 12,
				EnOtherUser3 = 13,
				EnOtherUser4 = 14,
				EnOtherUser5 = 15,
				EnOtherUser6 = 16,
				EnOtherUser7 = 17,
				EnOtherUser8 = 18,
				EnOtherUser9 = 19,
				EnOtherUser10 = 20,
				EnOtherUser11 = 21,
				EnOtherUser12 = 22,
				EnOtherUser13 = 23,
				EnOtherUser14 = 24,
				EnOtherUser15 = 25,
				EnOtherOutsideView0 = 40,
				EnOtherOutsideView1 = 41,
				EnOtherOutsideView2 = 42,
				EnOtherOutsideView3 = 43,
				EnOtherOutsideView4 = 44,
				EnOtherOutsideView5 = 45,
				EnOtherOutsideView6 = 46,
				EnOtherOutsideView7 = 47,
				EnOtherBoundaryView0 = 50,
				EnOtherBoundaryViewView1 = 51,
				EnOtherBoundaryViewView2 = 52,
				EnOtherBoundaryViewView3 = 53,
				EnOtherBoundaryViewView4 = 54,
				EnOtherBoundaryViewView5 = 55,
				EnOtherBoundaryViewView6 = 56,
				EnOtherBoundaryViewView7 = 57,
				EnOtherAyscDialog = 63,
				EnOtherAyscIAP = 66,
				EnOtherAyscCloud = 67,
				EnOtherAyscNetworking = 68,
				EnKeyboardBackspace = 8,
				EnKeyboardEnter = 13,
				EnKeyboardShift = 16,
				EnKeyboardControl = 17,
				EnKeyboardAlt = 18,
				EnKeyboardEscape = 27,
				EnKeyboardSpace = 32,
				EnKeyboardPageUp = 33,
				EnKeyboardPageDown = 34,
				EnKeyboardEnd = 35,
				EnKeyboardHome = 36,
				EnKeyboardArrowLeft = 37,
				EnKeyboardArrowUp = 38,
				EnKeyboardArrowRight = 39,
				EnKeyboardArrowDown = 40,
				EnKeyboardInsert = 45,
				EnKeyboardDelete = 46,
				EnKeyboard0 = 48,
				EnKeyboard1 = 49,
				EnKeyboard2 = 50,
				EnKeyboard3 = 51,
				EnKeyboard4 = 52,
				EnKeyboard5 = 53,
				EnKeyboard6 = 54,
				EnKeyboard7 = 55,
				EnKeyboard8 = 56,
				EnKeyboard9 = 57,
				EnKeyboardA = 65,
				EnKeyboardB = 66,
				EnKeyboardC = 67,
				EnKeyboardD = 68,
				EnKeyboardE = 69,
				EnKeyboardF = 70,
				EnKeyboardG = 71,
				EnKeyboardH = 72,
				EnKeyboardI = 73,
				EnKeyboardJ = 74,
				EnKeyboardK = 75,
				EnKeyboardL = 76,
				EnKeyboardM = 77,
				EnKeyboardN = 78,
				EnKeyboardO = 79,
				EnKeyboardP = 80,
				EnKeyboardQ = 81,
				EnKeyboardR = 82,
				EnKeyboardS = 83,
				EnKeyboardT = 84,
				EnKeyboardU = 85,
				EnKeyboardV = 86,
				EnKeyboardW = 87,
				EnKeyboardX = 88,
				EnKeyboardY = 89,
				EnKeyboardZ = 90,
				EnKeyboardKeyPad0 = 96,
				EnKeyboardKeyPad1 = 97,
				EnKeyboardKeyPad2 = 98,
				EnKeyboardKeyPad3 = 99,
				EnKeyboardKeyPad4 = 100,
				EnKeyboardKeyPad5 = 101,
				EnKeyboardKeyPad6 = 102,
				EnKeyboardKeyPad7 = 103,
				EnKeyboardKeyPad8 = 104,
				EnKeyboardKeyPad9 = 105,
				EnKeyboardKeyPadMultiply = 106,
				EnKeyboardKeyPadAdd = 107,
				EnKeyboardKeyPadSubstract = 109,
				EnKeyboardKeyPadPeriod = 110,
				EnKeyboardKeyPadSlash = 111,
				EnKeyboardF1 = 112,
				EnKeyboardF2 = 113,
				EnKeyboardF3 = 114,
				EnKeyboardF4 = 115,
				EnKeyboardF5 = 116,
				EnKeyboardF6 = 117,
				EnKeyboardF7 = 118,
				EnKeyboardF8 = 119,
				EnKeyboardF9 = 120,
				EnKeyboardF10 = 121,
				EnKeyboardF11 = 122,
				EnKeyboardF12 = 123
			};

			enum EventKind
			{
				EkCreate,
				EkDestroy,
				EkAlarm,
				EkStep,
				EkCollision,
				EkKeyboard = 5,
				EkMouse,
				EkOther = 7,
				EkDraw,
				EkPress,
				EkRelease,
				EkAsyncronous,
				EkUnknown
			};

		public:
			unsigned int			eventNumber;
			unsigned int			eventKind;
			std::vector<Action*>	actions;

			Event();
			~Event();
		};

	private:
		int							spriteIndex;
		int							parentIndex;
		int							maskIndex;

	protected:
		void WriteVer81(Stream* stream);
		void ReadVer81(Stream* stream);

		void WriteVer7(Stream* stream);
		void ReadVer7(Stream* stream);

	public:
		bool						solid;
		bool						visible;
		int							depth;
		bool						persistent;
		std::vector<Event*>			events;

		Sprite* sprite;
		Sprite* mask;
		Object* parent;

		Object(GmkFile* gmk);
		~Object();

		int GetId() const;
		void Finalize();
	};
}

#endif
