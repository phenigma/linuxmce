#include "WizardPageVideoRatio.h"

#include "Wizard.h"

WizardPageVideoRatio::WizardPageVideoRatio(SDLFrontEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{

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
	RatioMode = 1;
	WM_Event Event;
	Event.EnterKey();
	MainWizard->GenerateCustomEvent(Event);
}

void WizardPageVideoRatio::DoDecreaseSetting()
{
	RatioMode = 0;
	WM_Event Event;
	Event.EnterKey();
	MainWizard->GenerateCustomEvent(Event);
}
