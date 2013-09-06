/*
 *	gmksettings.cpp
 *	GMK Settings
 */

#include <gmksettings.hpp>
#include <gmk.hpp>

namespace Gmk
{
	Settings::Settings(GmkFile* gmk)
		: GmkResource(gmk),
		  fullscreen(false),
		  interpolatePixels(false),
		  noBorder(false),
		  showCursor(true),
		  scale(ScalingKeepAspectRatio),
		  sizeable(false),
		  stayOnTop(false),
		  windowColor(BuildColor(0, 0, 0)),
		  changeResolution(false),
		  colorDepth(CdNoChange),
		  resolution(ResolutionNoChange),
		  frequency(FrequencyNoChange),
		  noButtons(false),
		  vsync(false),
		  noScreenSaver(true),
		  fullscreenKey(true),
		  helpKey(true),
		  quitKey(true),
		  screenshotKey(true),
		  closeSecondary(true),
		  priority(PriorityNormal),
		  freeze(false),
		  showProgress(LpbtDefault),
		  frontImage(NULL),
		  backImage(NULL),
		  loadImage(NULL),
		  loadTransparent(false),
		  loadAlpha(255),
		  scaleProgress(true),
		  iconImage(NULL),
		  displayErrors(true),
		  writeErrors(false),
		  abortErrors(false),
		  treatUninitializedVariablesAsZero(false),
		  argumentError(true),
		  author(""),
		  versionString("100"),
		  information(""),
		  major(1),
		  minor(0),
		  release(0),
		  build(0),
		  company(""),
		  product(""),
		  copyright(""),
		  description("")
	{
		exists = true;

		// Create new icon in memory stream
		iconImage = new Stream();

		// Icon Header
		iconImage->WriteWord(0);						// Reserved (magic)
		iconImage->WriteWord(1);						// 1 = Icon, 2 = Cursor
		iconImage->WriteWord(1);						// 1 image in file

		// Image Header
		iconImage->WriteByte(32);						// 32px width
		iconImage->WriteByte(32);						// 32px height
		iconImage->WriteByte(0);						// Truecolor
		iconImage->WriteByte(0);						// Reserved
		iconImage->WriteWord(0);						// Color planes
		iconImage->WriteWord(32);						// 32bits per pixel
		iconImage->WriteDword(22486 - 16 - 6);			// Size of image
		iconImage->WriteDword(16 + 6);					// Offset to image

		// DIB header
		iconImage->WriteDword(40);						// Size of this header
		iconImage->WriteDword(32);						// Width
		iconImage->WriteDword(64);						// Height * 2
		iconImage->WriteWord(1);						// Color planes, must be 1
		iconImage->WriteDword(32);						// 32 bit color
		iconImage->WriteDword(0);						// No compression
		iconImage->WriteDword(22486 - 40 - 16 - 6);
		iconImage->WriteDword(1);						// Pixels per meter
		iconImage->WriteDword(1);						// Pixels per meter
		iconImage->WriteDword(0);						// Number of colors, set to 0 to default to 2 ^ n
		iconImage->WriteDword(0);						// Number of important colors

		// Make white pixels
		for(unsigned int i = 0; i < 22486 - 40 - 16 - 6; i++)
			iconImage->WriteByte(0xFF);
	}

	Settings::~Settings()
	{
		CleanMemory();
	}

	void Settings::CleanMemory()
	{
		if (frontImage != NULL)
			delete frontImage;

		if (backImage != NULL)
			delete backImage;

		if (loadImage != NULL)
			delete loadImage;

		if (iconImage != NULL)
			delete iconImage;
	}

	void Settings::WriteVer81(Stream* stream)
	{
		Stream* settingsStream = new Stream();

		settingsStream->WriteBoolean(fullscreen);
		settingsStream->WriteBoolean(interpolatePixels);
		settingsStream->WriteBoolean(noBorder);
		settingsStream->WriteBoolean(showCursor);
		settingsStream->WriteDword(scale);
		settingsStream->WriteBoolean(sizeable);
		settingsStream->WriteBoolean(stayOnTop);
		settingsStream->WriteDword(windowColor);
		settingsStream->WriteBoolean(changeResolution);
		settingsStream->WriteDword(colorDepth);
		settingsStream->WriteDword(resolution);
		settingsStream->WriteDword(frequency);
		settingsStream->WriteBoolean(noButtons);
		settingsStream->WriteBoolean(vsync);
		settingsStream->WriteBoolean(noScreenSaver);
		settingsStream->WriteBoolean(fullscreenKey);
		settingsStream->WriteBoolean(helpKey);
		settingsStream->WriteBoolean(quitKey);
		settingsStream->WriteBoolean(saveKey);
		settingsStream->WriteBoolean(screenshotKey);
		settingsStream->WriteBoolean(closeSecondary);
		settingsStream->WriteDword(priority);
		settingsStream->WriteBoolean(freeze);
		settingsStream->WriteDword(showProgress);

		if (showProgress == LpbtCustom)
		{
			settingsStream->WriteBitmap(backImage);
			settingsStream->WriteBitmap(frontImage);
		}

		if (loadImage != NULL)
		{
			settingsStream->WriteBoolean(true);
			settingsStream->WriteBitmap(loadImage);
		}
		else
			settingsStream->WriteBoolean(false);

		settingsStream->WriteBoolean(loadTransparent);
		settingsStream->WriteDword(loadAlpha);
		settingsStream->WriteBoolean(scaleProgress);

		settingsStream->Serialize(iconImage, false);
		
		settingsStream->WriteBoolean(displayErrors);
		settingsStream->WriteBoolean(writeErrors);
		settingsStream->WriteBoolean(abortErrors);
		settingsStream->WriteDword(BuildByte(0, 0, 0, 0, 0, 0, argumentError, treatUninitializedVariablesAsZero));

		settingsStream->WriteString(author);
		settingsStream->WriteString(versionString);
		settingsStream->WriteTimestamp();
		settingsStream->WriteString(information);
		settingsStream->WriteDword(major);
		settingsStream->WriteDword(minor);
		settingsStream->WriteDword(release);
		settingsStream->WriteDword(build);
		settingsStream->WriteString(company);
		settingsStream->WriteString(product);
		settingsStream->WriteString(copyright);
		settingsStream->WriteString(description);
		settingsStream->WriteTimestamp();

		stream->Serialize(settingsStream);
		delete settingsStream;
	}

	void Settings::ReadVer81(Stream* stream)
	{
		CleanMemory();

		Stream* settingsStream = stream->Deserialize();

		fullscreen				= settingsStream->ReadBoolean();
		interpolatePixels		= settingsStream->ReadBoolean();
		noBorder				= settingsStream->ReadBoolean();
		showCursor				= settingsStream->ReadBoolean();
		scale					= settingsStream->ReadDword();
		sizeable				= settingsStream->ReadBoolean();
		stayOnTop				= settingsStream->ReadBoolean();
		windowColor				= settingsStream->ReadDword();
		changeResolution		= settingsStream->ReadBoolean();
		colorDepth				= settingsStream->ReadDword();
		resolution				= settingsStream->ReadDword();
		frequency				= settingsStream->ReadDword();
		noButtons				= settingsStream->ReadBoolean();
		vsync					= settingsStream->ReadBoolean();
		noScreenSaver			= settingsStream->ReadBoolean();
		fullscreenKey			= settingsStream->ReadBoolean();
		helpKey					= settingsStream->ReadBoolean();
		quitKey					= settingsStream->ReadBoolean();
		saveKey					= settingsStream->ReadBoolean();
		screenshotKey			= settingsStream->ReadBoolean();
		closeSecondary			= settingsStream->ReadBoolean();
		priority				= settingsStream->ReadDword();
		freeze					= settingsStream->ReadBoolean();
		showProgress			= settingsStream->ReadDword();

		if (showProgress == LpbtCustom)
		{
			backImage = settingsStream->ReadBitmap();
			frontImage = settingsStream->ReadBitmap();
		}

		if (settingsStream->ReadBoolean())
			loadImage = settingsStream->ReadBitmap();

		loadTransparent			= settingsStream->ReadBoolean();
		loadAlpha				= settingsStream->ReadDword();
		scaleProgress			= settingsStream->ReadBoolean();

		iconImage				= settingsStream->Deserialize(false);

		displayErrors			= settingsStream->ReadBoolean();
		writeErrors				= settingsStream->ReadBoolean();
		abortErrors				= settingsStream->ReadBoolean();
		
		unsigned int errorFlags = settingsStream->ReadDword();

		treatUninitializedVariablesAsZero = (errorFlags & 0x01) == 0x01;
		argumentError = (errorFlags & 0x02) == 0x02;

		author					= settingsStream->ReadString();
		versionString			= settingsStream->ReadString();
		settingsStream->ReadTimestamp();
		information				= settingsStream->ReadString();

		major					= settingsStream->ReadDword();
		minor					= settingsStream->ReadDword();
		release					= settingsStream->ReadDword();
		build					= settingsStream->ReadDword();
		company					= settingsStream->ReadString();
		product					= settingsStream->ReadString();
		copyright				= settingsStream->ReadString();
		description				= settingsStream->ReadString();
		settingsStream->ReadTimestamp();

		delete settingsStream;
	}

	void Settings::WriteVer7(Stream* stream)
	{

	}

	void Settings::ReadVer7(Stream* stream)
	{
		CleanMemory();

		fullscreen				= stream->ReadBoolean();
		interpolatePixels		= stream->ReadBoolean();
		noBorder				= stream->ReadBoolean();
		showCursor				= stream->ReadBoolean();
		scale					= stream->ReadDword();
		sizeable				= stream->ReadBoolean();
		stayOnTop				= stream->ReadBoolean();
		windowColor				= stream->ReadDword();
		changeResolution		= stream->ReadBoolean();
		colorDepth				= stream->ReadDword();
		resolution				= stream->ReadDword();
		frequency				= stream->ReadDword();
		noButtons				= stream->ReadBoolean();
		vsync					= stream->ReadBoolean();
		fullscreenKey			= stream->ReadBoolean();
		helpKey					= stream->ReadBoolean();
		quitKey					= stream->ReadBoolean();
		saveKey					= stream->ReadBoolean();
		screenshotKey			= stream->ReadBoolean();
		closeSecondary			= stream->ReadBoolean();
		priority				= stream->ReadDword();
		freeze					= stream->ReadBoolean();
		showProgress			= stream->ReadDword();

		if (showProgress == LpbtCustom)
		{
			backImage = stream->ReadBitmapOld();
			frontImage = stream->ReadBitmapOld();
		}

		if (stream->ReadBoolean())
			loadImage = stream->ReadBitmapOld();

		loadTransparent			= stream->ReadBoolean();
		loadAlpha				= stream->ReadDword();
		scaleProgress			= stream->ReadBoolean();

		iconImage				= stream->Deserialize(false);

		displayErrors			= stream->ReadBoolean();
		writeErrors				= stream->ReadBoolean();
		abortErrors				= stream->ReadBoolean();
		treatUninitializedVariablesAsZero = stream->ReadBoolean();

		author					= stream->ReadString();
		versionString			= stream->ReadString();
		stream->ReadTimestamp();
		information				= stream->ReadString();

		unsigned int count = stream->ReadDword();
		while(count--)
		{
			std::string name = stream->ReadString();
			std::string value = stream->ReadString();

			gmkHandle->constants.push_back(std::pair<std::string, std::string>(name, value));
		}

		major					= stream->ReadDword();
		minor					= stream->ReadDword();
		release					= stream->ReadDword();
		build					= stream->ReadDword();
		company					= stream->ReadString();
		product					= stream->ReadString();
		copyright				= stream->ReadString();
		description				= stream->ReadString();
	}
}
