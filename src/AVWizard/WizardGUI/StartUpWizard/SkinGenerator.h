/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */

#ifndef SkinGenerator_H_
#define SkinGenerator_H_

#include <string>

#include "SettingsDictionaryTree.h"

#define USE_UI2


class SkinGenerator
{
	SettingsDictionaryTree Config;

public:
	std::string DefaultFontStep; // "/usr/share/fonts/truetype/msttcorefonts/Arial.ttf"
	std::string DefaultFontMiniTitle; // "/usr/share/fonts/truetype/msttcorefonts/Trebuchet_MS.ttf"
	std::string DefaultFontText; // "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf"

	std::string CommandSetResolution; // "/usr/pluto/bin/AVWizard_UpdateResolution.sh"
	std::string CommandResetResolution; // COMMAND_SET_RATIO + " reset"
	std::string CommandChangeResolution; // COMMAND_SET_RATIO + " set_resolution"
	
	std::string CommandSetAudioConnector; // "/usr/pluto/bin/AVWizard_AudioConnector.sh"
	std::string PathPixmaps; // "/usr/pluto/wiz_pixmaps"

	std::string StepFontColorHighlight;
	std::string StepFontColorStandard;
	std::string StepFontColorPassed;
	std::string StepFontColorShadow;
	std::string StepFontColorDisable;

	std::string StepFontSize;

	std::string SubStepFontColor;
	std::string SubStepFontSize;


	std::string DefaultFontColor;
	std::string DefaultFontSize;

	std::string TitleFontColor;
	std::string TitleFontSize;

	std::string MiniTitleFontColor;
	std::string MiniTitleFontSize;

	std::string ButtonTextFontColor;
	std::string ButtonTextHighlightFontSize;

	std::string ScrollListBackColor;
	std::string ScrollListBackFocusedColor;
	std::string ScrollListFocusedBackColor;
	std::string ScrollListFocusedBackFocusedColor;

	std::string BackgroundColor;

	std::string CounterFontSize;

	std::string WaitForAcceptResolution;

	std::string ScrollBackColor;
	std::string ScrollBackFocusedColor;
	std::string HighScrollBackColor;
	std::string HighScrollBackFocusedColor;

	void SetSkinEnvironment(std::string SkinName);
	void SaveSkin(std::string XMLFile);

	static SkinGenerator* Instance();

	void GetEnvironment();
private:
	SkinGenerator();

	static SkinGenerator* Instance_;
};

#endif //SkinGenerator_H_
