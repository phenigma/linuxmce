#include "WizardPageVideoRatio.h"

#include "Wizard.h"

#include "GUIWizardUtils.h"

WizardPageVideoRatio::WizardPageVideoRatio(SDLFrontEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{
	Buttons["16_9"] = 1;
	Buttons["4_3"] = 2;
	OutputValue = "4_3";
}

WizardPageVideoRatio::~WizardPageVideoRatio(void)
{
}

/*virtual*/ int WizardPageVideoRatio::DoApplySetting(SettingsDictionary* Dictionary)
{
	if(Dictionary == NULL)
		return -1;
	std::string RatioValue = "16_9";
	if(RatioMode != 0)
		RatioValue = "4_3";

	Dictionary->Set("VideoRatio", RatioValue);
	return 0;
}

/*virtual*/ void WizardPageVideoRatio::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
	if(!AVWizardSettings->Exists("VideoRatio"))
	{
		RatioMode = 0;
		return;
	}
	std::string RatioValue = AVWizardSettings->GetValue("VideoRatio");
	RatioMode = (RatioValue == "4_3");

	RatioValue = "16_9";
	if(RatioMode != 0)
		RatioValue = "4_3";

	AVWizardSettings->Set("VideoRatio", RatioValue);

	if(!AVWizardSettings->Exists("VideoRatio"))
	{
		RatioMode = 0;
		return;
	}
}

void WizardPageVideoRatio::DoIncreaseSetting()
{
	std::string IndexText = Utils::CopyStr(Selected->GetName().c_str(), 3, 1);
	int ButtonIndex = Utils::StringToInt32(IndexText);
	if(ButtonIndex == 2)
		return;
	Selected->SetFocus(false);
	ButtonIndex++;
	std::string ButtonName = "Btn"+Utils::Int32ToString(ButtonIndex);
	Selected = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive(ButtonName));
	Selected->SetFocus(true);	
}

void WizardPageVideoRatio::DoDecreaseSetting()
{
	std::string IndexText = Utils::CopyStr(Selected->GetName().c_str(), 3, 1);
	int ButtonIndex = Utils::StringToInt32(IndexText);
	if(ButtonIndex == 1)
		return;
	Selected->SetFocus(false);
	ButtonIndex--;
	std::string ButtonName = "Btn"+Utils::Int32ToString(ButtonIndex);
	Selected = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive(ButtonName));
	Selected->SetFocus(true);
}
