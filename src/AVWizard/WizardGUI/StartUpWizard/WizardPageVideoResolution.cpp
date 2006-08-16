#include "WizardPageVideoResolution.h"

#include "ConfigureCommons.h"
#include "GUIWizardUtils.h"
#include "SettingsDictionaryTree.h"
#include "Wizard.h"
#include "SkinGenerator.h"
#include "WizardWidgetLabel.h"
#include <iostream>
#include "ThreadSleeper.h"

WizardPageVideoResolution::WizardPageVideoResolution(SDLFrontEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{
	Selected = NULL;
}

WizardPageVideoResolution::~WizardPageVideoResolution(void)
{
	ThreadSleeper::Instance()->Quit();
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

	std::string ResolutionValue;
	std::string RefreshValue;
	if(!AVWizardSettings->Exists("Resolution"))
	{
		std::cerr<<"Warning: Resolution not set at this step!"<<std::endl;
	}
	else
		ResolutionValue = AVWizardSettings->GetValue("Resolution");

	if(!AVWizardSettings->Exists("Refresh"))
	{
		std::cerr<<"Warning: Refresh not set at this step!"<<std::endl;
	}
	else
		RefreshValue = AVWizardSettings->GetValue("Refresh");

	WizardWidgetLabel* Label = dynamic_cast<WizardWidgetLabel*>
		(Page->GetChildRecursive("DescribeText0"));

	std::string LabelValue = "You chose the following resolution: ";

	Label->SetCaption(LabelValue + ResolutionValue+ "@" + RefreshValue + " Hz");


	ThreadSleeper::Instance()->Init(Seconds);
	Label = dynamic_cast<WizardWidgetLabel*>
		(Page->GetChildRecursive("CounterLabel"));
	ThreadSleeper::Instance()->SetLabel(Label);
	
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

