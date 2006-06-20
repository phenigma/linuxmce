//
// C++ Implementation: SkinGenerator
//
// Description: 
//
//
// Author: Ciprian Mustiata <ciprian.m@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "SkinGenerator.h"

#include "ConfigureCommons.h"

void SkinGenerator::SetSkinEnvironment(std::string SkinName)
{
	DefaultFontStep = "/usr/share/fonts/truetype/msttcorefonts/Arial.ttf";
	DefaultFontMiniTitle = "/usr/share/fonts/truetype/msttcorefonts/Trebuchet_MS.ttf";
	DefaultFontText = "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf";

	CommandSetResolution = "/usr/pluto/bin/AVWizard_UpdateResolution.sh";
	CommandSetResolutionPlus = CommandSetResolution  + " resolution+";
	CommandSetResolutionMinus = CommandSetResolution  + " resolution-";

	CommandSetRefresh = "/usr/pluto/bin/AVWizard_UpdateResolution.sh";
	CommandSetRefreshPlus = CommandSetRefresh + " refresh+";
	CommandSetRefreshMinus = CommandSetRefresh + " refresh-";

	CommandSetRatio = "/usr/pluto/bin/AVWizard_UpdateResolution.sh";
	CommandSetRatio4_3 = CommandSetRatio + " 4_3";
	CommandSetRatio16_9 = CommandSetRatio + " 16_9";

	StepFontColorHighlight = "FFFFFF";
	StepFontColorStandard = "000000";
	StepFontColorPassed = "B0B0B0";
	StepFontColorShadow = "002900";
	StepFontSize = "18";

	SubStepFontColor = "FFFFFF";
	SubStepFontSize = "14";

	TitleFontColor = "000000";
	TitleFontSize = "24";

	MiniTitleFontColor = "7F0000";
	MiniTitleFontSize = "22";

	ButtonTextFontColor = "000000";
	ButtonTextHighlightFontSize = "22";

	DefaultFontColor = "000000";
	DefaultFontSize = "13";

	PathPixmaps = "/usr/pluto/wiz_pixmaps";
}

void SkinGenerator::SaveSkin(std::string XMLFile)
{
	SettingsDictionary* Dictionary = Config.GetDictionary();

	Dictionary->Set("DefaultFontStep", DefaultFontStep);
	Dictionary->Set("DefaultFontMiniTitle", DefaultFontMiniTitle);
	Dictionary->Set("DefaultFontText", DefaultFontText);

	Dictionary->Set("CommandSetResolution", CommandSetResolution);
	Dictionary->Set("CommandSetResolutionPlus", CommandSetResolutionPlus);
	Dictionary->Set("CommandSetResolutionMinus", CommandSetResolutionMinus);

	Dictionary->Set("CommandSetRefresh", CommandSetRefresh);
	Dictionary->Set("CommandSetRefreshPlus", CommandSetRefreshPlus);
	Dictionary->Set("CommandSetRefreshMinus", CommandSetRefreshMinus);

	Dictionary->Set("CommandSetRatio", CommandSetRatio);
	Dictionary->Set("CommandSetRatio4_3", CommandSetRatio4_3);
	Dictionary->Set("CommandSetRatio16_9", CommandSetRatio16_9);

	Dictionary->Set("PathPixmaps", PathPixmaps);

	Dictionary->Set("StepFontColorHighlight", StepFontColorHighlight);
	Dictionary->Set("StepFontColorStandard", StepFontColorHighlight);
	Dictionary->Set("StepFontColorPassed", StepFontColorHighlight);
	Dictionary->Set("StepFontColorShadow", StepFontColorShadow);
	Dictionary->Set("StepFontSize", StepFontSize);

	Dictionary->Set("SubStepFontColor", SubStepFontColor);
	Dictionary->Set("SubStepFontSize", SubStepFontSize);

	Dictionary->Set("TitleFontColor", TitleFontColor);
	Dictionary->Set("TitleFontSize", TitleFontSize);

	Dictionary->Set("MiniTitleFontColor", MiniTitleFontColor);
	Dictionary->Set("MiniTitleFontSize", MiniTitleFontSize);

	Dictionary->Set("ButtonTextFontColor", MiniTitleFontColor);
	Dictionary->Set("ButtonTextHighlightFontSize", MiniTitleFontSize);

	Config.SaveToXMLFile(XMLFile);
}

void SkinGenerator::GetEnvironment()
{
	Config.LoadFromXMLFile("/usr/pluto/wiz_pixmaps/skin.xml");
	SettingsDictionary* Dictionary = Config.GetDictionary();

	DefaultFontStep = Dictionary->GetValue("DefaultFontStep");
	DefaultFontMiniTitle = Dictionary->GetValue("DefaultFontMiniTitle");
	DefaultFontText = Dictionary->GetValue("DefaultFontText");

	CommandSetResolution = Dictionary->GetValue("CommandSetResolution");
	CommandSetResolutionPlus = Dictionary->GetValue("CommandSetResolutionPlus");
	CommandSetResolutionMinus = Dictionary->GetValue("CommandSetResolutionMinus");

	CommandSetRefresh = Dictionary->GetValue("CommandSetRefresh");
	CommandSetRefreshPlus = Dictionary->GetValue("CommandSetRefreshPlus");
	CommandSetRefreshMinus = Dictionary->GetValue("CommandSetRefreshMinus");

	CommandSetRatio = Dictionary->GetValue("CommandSetRatio");
	CommandSetRatio4_3 = Dictionary->GetValue("CommandSetRatio4_3");
	CommandSetRatio16_9 = Dictionary->GetValue("CommandSetRatio16_9");

	PathPixmaps = Dictionary->GetValue("PathPixmaps");

	StepFontColorHighlight = Dictionary->GetValue("StepFontColorHighlight");
	StepFontColorHighlight = Dictionary->GetValue("StepFontColorStandard");
	StepFontColorHighlight = Dictionary->GetValue("StepFontColorPassed");
	StepFontColorShadow = Dictionary->GetValue("StepFontColorShadow");
	StepFontSize = Dictionary->GetValue("StepFontSize");

	SubStepFontColor = Dictionary->GetValue("SubStepFontColor");
	SubStepFontSize = Dictionary->GetValue("SubStepFontSize");


	TitleFontColor = Dictionary->GetValue("TitleFontColor");
	TitleFontSize = Dictionary->GetValue("TitleFontSize");

	MiniTitleFontColor = Dictionary->GetValue("MiniTitleFontColor");
	MiniTitleFontSize = Dictionary->GetValue("MiniTitleFontSize");

}

/*static*/ SkinGenerator* SkinGenerator::Instance_ = NULL;

SkinGenerator* SkinGenerator::Instance()
{
	if(Instance_ == NULL)
		Instance_ = new SkinGenerator();

//	Instance_->GetEnvironment();
	return Instance_;
}

SkinGenerator::SkinGenerator()
{
	SetSkinEnvironment("Basic");	
}

