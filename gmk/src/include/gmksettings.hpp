/*
 *	gmksettings.hpp
 *	GMK Settings
 */

#ifndef __GMK_SETTINGS_HPP
#define __GMK_SETTINGS_HPP

#include <gmkresource.hpp>

namespace Gmk
{
	class Settings : public GmkResource
	{
	public:
		enum Scaling
		{
			ScalingKeepAspectRatio = -1,
			ScalingFullScale
		};

		enum ColorDepth
		{
			CdNoChange,
			Cd16Bit,
			Cd32Bit
		};

		enum Resolution
		{
			ResolutionNoChange,
			Resolution320x240,
			Resolution640x480,
			Resolution800x600,
			Resolution1024x768,
			Resolution1280x1024,
			Resolution1600x1200
		};

		enum Frequency
		{
			FrequencyNoChange,
			Frequency60,
			Frequency70,
			Frequency85,
			Frequency100,
			Frequency120
		};

		enum Priority
		{
			PriorityNormal,
			PriorityHigh,
			PriorityHighest
		};

		enum LoadingProgressBarType
		{
			LpbtNone,
			LpbtDefault,
			LpbtCustom
		};

		void CleanMemory();

	protected:
		void WriteVer81(Stream* stream);
		void ReadVer81(Stream* stream);

		void WriteVer7(Stream* stream);
		void ReadVer7(Stream* stream);

	public:
		bool			fullscreen;
		bool			interpolatePixels;
		bool			noBorder;
		bool			showCursor;
		int				scale;
		bool			sizeable;
		bool			stayOnTop;
		unsigned int	windowColor;
		bool			changeResolution;
		unsigned int	colorDepth;
		unsigned int	resolution;
		unsigned int	frequency;
		bool			noButtons;
		bool			vsync;
		bool			noScreenSaver;
		bool			fullscreenKey;
		bool			helpKey;
		bool			quitKey;
		bool			saveKey;
		bool			screenshotKey;
		bool			closeSecondary;
		unsigned int	priority;
		bool			freeze;
		unsigned int	showProgress;
		Stream*			frontImage;
		Stream*			backImage;
		Stream*			loadImage;
		bool			loadTransparent;
		unsigned int	loadAlpha;
		bool			scaleProgress;
		Stream*			iconImage;
		bool			displayErrors;
		bool			writeErrors;
		bool			abortErrors;
		bool			treatUninitializedVariablesAsZero;
		bool			argumentError;
		std::string		author;
		std::string		versionString;
		std::string		information;
		unsigned int	major;
		unsigned int	minor;
		unsigned int	release;
		unsigned int	build;
		std::string		company;
		std::string		product;
		std::string		copyright;
		std::string		description;

		Settings(GmkFile* gmk);
		~Settings();
	};
}

#endif
