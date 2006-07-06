/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */

#ifndef SkinGenerator_H_
#define SkinGenerator_H_

#include <string>

#include "SettingsDictionaryTree.h"

class SkinGenerator
{
	SettingsDictionaryTree Config;

public:
	std::string DefaultFontStep; // "/usr/share/fonts/truetype/msttcorefonts/Arial.ttf"
	std::string DefaultFontMiniTitle; // "/usr/share/fonts/truetype/msttcorefonts/Trebuchet_MS.ttf"
	std::string DefaultFontText; // "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf"

	std::string CommandSetResolution; // "/usr/pluto/bin/AVWizard_UpdateResolution.sh"
	std::string CommandSetResolutionPlus; //COMMAND_SET_RESOLUTION + " resolution+"
	std::string CommandSetResolutionMinus; // COMMAND_SET_RESOLUTION + " resolution-"

	std::string CommandSetRefresh; // "/usr/pluto/bin/AVWizard_UpdateResolution.sh"
	std::string CommandSetRefreshPlus; // COMMAND_SET_REFRESH + " refresh+"
	std::string CommandSetRefreshMinus; // COMMAND_SET_REFRESH + " refresh-"

	std::string CommandSetRatio; // "/usr/pluto/bin/AVWizard_UpdateResolution.sh"
	std::string CommandSetRatio4_3;  // COMMAND_SET_RATIO + " 4_3"
	std::string CommandSetRatio16_9; // COMMAND_SET_RATIO " 16_9"
	
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

	std::string BackgroundColor;

	void SetSkinEnvironment(std::string SkinName);
	void SaveSkin(std::string XMLFile);

	static SkinGenerator* Instance();

	void GetEnvironment();
private:
	SkinGenerator();

	static SkinGenerator* Instance_;
};

#endif //SkinGenerator_H_
