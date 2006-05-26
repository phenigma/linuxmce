#include "WizardPageDolbyTest.h"

#include "GUIWizardUtils.h"

WizardPageDolbyTest::WizardPageDolbyTest(SDLFrontEnd* FrontEnd, std::string Name)
: WizardPage(FrontEnd, Name)
{
	Buttons["No, I cannot hear it"] = 1;
	Buttons["Yes, I can hear it"] = 2;
	OutputValue = "Yes, I can hear it";
	Selected = NULL;
	Player = NULL;
}

WizardPageDolbyTest::~WizardPageDolbyTest(void)
{
	Player->StopPlayerEngine();
}

/*virtual*/ int WizardPageDolbyTest::DoApplySetting(SettingsDictionary* Dictionary)
{
	if((Dictionary == NULL)|| (Selected == NULL))
		return -1;
	OutputValue = Selected->GetCaption();
	Dictionary->Set("DolbyTest", OutputValue!= "No, I cannot hear it");
	return 0;
}

/*virtual*/ void WizardPageDolbyTest::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
	std::string FileName, ConfigName;
	Player = XinePlayer::GetInstance();
	if(AVWizardSettings->Exists("XineConfigFile"))
	{
		ConfigName = AVWizardSettings->GetValue("XineConfigFile");

		if(AVWizardSettings->Exists("DolbyTestFile"))
			FileName = AVWizardSettings->GetValue("DolbyTestFile");
		Player->InitPlayerEngine(ConfigName, FileName);
	}

	if(!AVWizardSettings->Exists("DolbyTest"))
	{
		SearchSelectedItem();
		return;
	}
	std::string Value = AVWizardSettings->GetValue("DolbyTest");
	if (Utils::StringToInt32(Value.c_str()) != 0)
		OutputValue = "Yes, I can hear it";
	else
		OutputValue = "No, I cannot hear it";

	SearchSelectedItem();
}

/*virtual*/ void WizardPageDolbyTest::DoIncreaseSetting()
{
	std::string IndexText = Utils::CopyStr(Selected->GetName().c_str(), 3, 1);
	int ButtonIndex = Utils::StringToInt32(IndexText.c_str());
	if(ButtonIndex == 2)
		return;
	Selected->SetFocus(false);
	ButtonIndex++;
	std::string ButtonName = "Btn"+Utils::Int32ToString(ButtonIndex);
	Selected = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive(ButtonName));
	Selected->SetFocus(true);	
}

/*virtual*/ void WizardPageDolbyTest::DoDecreaseSetting()
{
	std::string IndexText = Utils::CopyStr(Selected->GetName().c_str(), 3, 1);
	int ButtonIndex = Utils::StringToInt32(IndexText.c_str());
	if(ButtonIndex == 1)
		return;
	Selected->SetFocus(false);
	ButtonIndex--;
	std::string ButtonName = "Btn"+Utils::Int32ToString(ButtonIndex);
	Selected = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive(ButtonName));
	Selected->SetFocus(true);	
}

/*virtual*/ void WizardPageDolbyTest::SearchSelectedItem()
{
	int ButtonIndex = Buttons[OutputValue];
	if(ButtonIndex == 0)
		return;
	std::string ButtonName = "Btn"+Utils::Int32ToString(ButtonIndex);
	Selected = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive(ButtonName));
	Selected->SetFocus(true);	
}
