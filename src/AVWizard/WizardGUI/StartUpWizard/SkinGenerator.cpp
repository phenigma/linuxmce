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
	StepFontColorDisable = "0A990A";
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
	CommandSetAudioConnector = "/usr/pluto/bin/AVWizard_AudioConnector.sh";
	
	BackgroundColor = "000000";
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
	Dictionary->Set("StepFontColorDisable", StepFontColorDisable);

	Dictionary->Set("SubStepFontColor", SubStepFontColor);
	Dictionary->Set("SubStepFontSize", SubStepFontSize);

	Dictionary->Set("TitleFontColor", TitleFontColor);
	Dictionary->Set("TitleFontSize", TitleFontSize);

	Dictionary->Set("MiniTitleFontColor", MiniTitleFontColor);
	Dictionary->Set("MiniTitleFontSize", MiniTitleFontSize);

	Dictionary->Set("ButtonTextFontColor", MiniTitleFontColor);
	Dictionary->Set("ButtonTextHighlightFontSize", MiniTitleFontSize);

	Dictionary->Set("BackgroundColor", BackgroundColor);

	Config.SaveToXMLFile(XMLFile);
}

void SkinGenerator::GetEnvironment()
{
	Config.LoadFromXMLFile("/usr/pluto/wiz_pixmaps/skin.xml");
	SettingsDictionary* Dictionary = Config.GetDictionary();

	if(Dictionary->Exists("DefaultFontStep"))
		DefaultFontStep = Dictionary->GetValue("DefaultFontStep");
	if(Dictionary->Exists("DefaultFontMiniTitle"))
		DefaultFontMiniTitle = Dictionary->GetValue("DefaultFontMiniTitle");
	if(Dictionary->Exists("DefaultFontText"))
		DefaultFontText = Dictionary->GetValue("DefaultFontText");

	if(Dictionary->Exists("CommandSetResolution"))
		CommandSetResolution = Dictionary->GetValue("CommandSetResolution");
	if(Dictionary->Exists("CommandSetResolutionPlus"))
		CommandSetResolutionPlus = Dictionary->GetValue("CommandSetResolutionPlus");
	if(Dictionary->Exists("CommandSetResolutionMinus"))
		CommandSetResolutionMinus = Dictionary->GetValue("CommandSetResolutionMinus");

	if(Dictionary->Exists("CommandSetRefresh"))
		CommandSetRefresh = Dictionary->GetValue("CommandSetRefresh");
	if(Dictionary->Exists("CommandSetRefreshPlus"))
		CommandSetRefreshPlus = Dictionary->GetValue("CommandSetRefreshPlus");
	if(Dictionary->Exists("CommandSetRefreshMinus"))
		CommandSetRefreshMinus = Dictionary->GetValue("CommandSetRefreshMinus");

	if(Dictionary->Exists("CommandSetRatio"))
		CommandSetRatio = Dictionary->GetValue("CommandSetRatio");
	if(Dictionary->Exists("CommandSetRatio4_3"))
		CommandSetRatio4_3 = Dictionary->GetValue("CommandSetRatio4_3");
	if(Dictionary->Exists("CommandSetRatio16_9"))
		CommandSetRatio16_9 = Dictionary->GetValue("CommandSetRatio16_9");

	if(Dictionary->Exists("PathPixmaps"))
		PathPixmaps = Dictionary->GetValue("PathPixmaps");

	if(Dictionary->Exists("StepFontColorHighlight"))
		StepFontColorHighlight = Dictionary->GetValue("StepFontColorHighlight");
	if(Dictionary->Exists("StepFontColorStandard"))
		StepFontColorHighlight = Dictionary->GetValue("StepFontColorStandard");
	if(Dictionary->Exists("StepFontColorPassed"))
		StepFontColorHighlight = Dictionary->GetValue("StepFontColorPassed");
	if(Dictionary->Exists("StepFontColorShadow"))
		StepFontColorShadow = Dictionary->GetValue("StepFontColorShadow");
	if(Dictionary->Exists("StepFontSize"))
		StepFontSize = Dictionary->GetValue("StepFontSize");
	if(Dictionary->Exists("StepFontColorDisable"))
		StepFontColorDisable = Dictionary->GetValue("StepFontColorDisable");

	if(Dictionary->Exists("SubStepFontColor"))
		SubStepFontColor = Dictionary->GetValue("SubStepFontColor");
	if(Dictionary->Exists("SubStepFontSize"))
		SubStepFontSize = Dictionary->GetValue("SubStepFontSize");


	if(Dictionary->Exists("TitleFontColor"))
		TitleFontColor = Dictionary->GetValue("TitleFontColor");
	if(Dictionary->Exists("TitleFontSize"))
		TitleFontSize = Dictionary->GetValue("TitleFontSize");

	if(Dictionary->Exists("MiniTitleFontColor"))
		MiniTitleFontColor = Dictionary->GetValue("MiniTitleFontColor");
	if(Dictionary->Exists("MiniTitleFontSize"))
		MiniTitleFontSize = Dictionary->GetValue("MiniTitleFontSize");

	if(Dictionary->Exists("BackgroundColor"))
		BackgroundColor = Dictionary->GetValue("BackgroundColor");
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

