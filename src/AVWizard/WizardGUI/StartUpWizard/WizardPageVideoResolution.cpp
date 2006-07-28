#include "WizardPageVideoResolution.h"

#include "ConfigureCommons.h"

#include "GUIWizardUtils.h"

#include <iostream>

#include "SettingsDictionaryTree.h"

#include "Wizard.h"

#include "SkinGenerator.h"



WizardPageVideoResolution::WizardPageVideoResolution(SDLFrontEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{
	Selected = NULL;
	Sleeper = NULL;
}

WizardPageVideoResolution::~WizardPageVideoResolution(void)
{
	delete Sleeper;
}


/*virtual*/ int WizardPageVideoResolution::DoApplySetting(SettingsDictionary* Dictionary)
{
	if(Dictionary == NULL)
		return -1;
	if (Selected->GetName() == "BackBtn")
	{
		return 1;
	}

	return 0;
}

/*virtual*/ void WizardPageVideoResolution::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
	Selected = dynamic_cast<WizardWidgetButton*>
		(Page->GetChildRecursive("BackBtn"));
	if(Selected == NULL)
	{
		std::cout<<"Warning, no back button!";
		return;
	}
	Selected->SetFocus(true);
	int Seconds = Utils::StringToInt32(SkinGenerator::Instance()->WaitForAcceptResolution);
	if(!Seconds)
		Seconds = 3;

	Sleeper = new VideoResolutionSecondSleeper(Seconds);
	WizardWidgetLabel* Label = dynamic_cast<WizardWidgetLabel*>
		(Page->GetChildRecursive("CounterLabel"));
	Sleeper->SetLabel(Label);
	
}

/*virtual*/ void WizardPageVideoResolution::DoIncreaseSetting()
{
	if(Selected)
		Selected->SetFocus(false);
	Selected = dynamic_cast<WizardWidgetButton*>
		(Page->GetChildRecursive("ContinueBtn"));
	if(Selected == NULL)
	{
		std::cout<<"Warning, no continue button!";
		return;
	}
	Selected->SetFocus(true);
}

/*virtual*/ void WizardPageVideoResolution::DoDecreaseSetting()
{
	if(Selected)
		Selected->SetFocus(false);
	Selected = dynamic_cast<WizardWidgetButton*>
		(Page->GetChildRecursive("BackBtn"));
	if(Selected == NULL)
	{
		std::cout<<"Warning, no back button!";
		return;
	}
	Selected->SetFocus(true);

}

VideoResolutionSecondSleeper::VideoResolutionSecondSleeper(int Seconds)
	: ThreadSleeper(Seconds)
{
}

void VideoResolutionSecondSleeper::SecondTick()
{
	if(!Label)
	{
		std::cout<<"VideoResolutionSecondSleeper::SecondTick Warning! No label = nothing to draw";
		return;
	}
	int Seconds = GetSecondRemaining();
	std::string LabelCaption = Utils::Int32ToString(Seconds);
	Label->SetCaption(LabelCaption);
	WM_Event Event;
	if(Seconds)
		Event.DownKey();
	else
		Event.EscapeKey();
	Wizard::GetInstance()->GenerateCustomEvent(Event);
}

/*virtual*/ void VideoResolutionSecondSleeper::SetLabel(WizardWidgetLabel* Label)
{
	this->Label = Label;
}
