#include "WizardPageVideoRatio.h"

#include "Wizard.h"

#include "GUIWizardUtils.h"

#include "ConfigureCommons.h"

WizardPageVideoRatio::WizardPageVideoRatio(SDLFrontEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{
	Buttons["4_3"] = 1;
	Buttons["16_9"] = 2;
	OutputValue = "4_3";
	Selected = NULL;
}

WizardPageVideoRatio::~WizardPageVideoRatio(void)
{
}

/*virtual*/ int WizardPageVideoRatio::DoApplySetting(SettingsDictionary* Dictionary)
{
	if(Dictionary == NULL)
		return -1;
	if(Selected->GetName() == "Btn1")
	{
		OutputValue = "4_3";
		system(COMMAND_SET_RATIO_4_3);
	}	
	else
	{
		OutputValue = "16_9";
		system(COMMAND_SET_RATIO_16_9);
	}
	Dictionary->Set("Video_Ratio", OutputValue);
	return 0;
}

/*virtual*/ void WizardPageVideoRatio::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
	if(!AVWizardSettings->Exists("Video_Ratio"))
	{
		RatioMode = 0;
	}
	else
	{
		OutputValue = AVWizardSettings->GetValue("Video_Ratio");
		RatioMode = (OutputValue == "4_3");
	}

	Wizard::GetInstance()->Resize(640, 480, false);

	SearchSelectedItem();
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


/*virtual*/ void WizardPageVideoRatio::SearchSelectedItem()
{
	int ButtonIndex = Buttons[OutputValue];
	if(ButtonIndex == 0)
		return;
	std::string ButtonName = "Btn"+Utils::Int32ToString(ButtonIndex);
	WizardWidgetButton* Btn43, *Btn169;
	Btn43 = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive("Btn1"));
	Btn169 = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive("Btn2"));
	Btn43->SetFocus(false);
	Btn169->SetFocus(false);
	if(ButtonIndex == 1)
		Selected = Btn43;
	else
		Selected = Btn169;
	Selected->SetFocus(true);	
}
