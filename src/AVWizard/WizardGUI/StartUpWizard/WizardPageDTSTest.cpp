#include "WizardPageDTSTest.h"

#include "GUIWizardUtils.h"

WizardPageDTSTest::WizardPageDTSTest(SDLFrontEnd* FrontEnd, std::string Name)
: WizardPage(FrontEnd, Name)
{
	Buttons["No, I cannot hear it"] = 1;
	Buttons["Yes, I can hear it"] = 2;
	OutputValue = "Yes, I can hear it";
	Selected = NULL;
	Player = NULL;
}

WizardPageDTSTest::~WizardPageDTSTest(void)
{
	delete Player;
}

/*virtual*/ int WizardPageDTSTest::DoApplySetting(SettingsDictionary* Dictionary)
{
	if(Selected == NULL || Dictionary == NULL)
		return -1;
	OutputValue = Selected->GetCaption();
	Dictionary->Set("DTSTest", OutputValue!= "No, I cannot hear it");
	return 0;
}

/*virtual*/ void WizardPageDTSTest::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
	std::string FileName, ConfigName;
	Player = new XinePlayer();
	if(AVWizardSettings->Exists("XineConfigFile"))
	{
		ConfigName = AVWizardSettings->GetValue("XineConfigFile");

		if(AVWizardSettings->Exists("DTSTestFile"))
			FileName = AVWizardSettings->GetValue("DTSTestFile");
		Player->InitPlayerEngine(ConfigName, FileName);
	}
	

	if(!AVWizardSettings->Exists("DTSTest"))
	{
		SearchSelectedItem();
		return;
	}
	std::string Value = AVWizardSettings->GetValue("DTSTest");
	if (Utils::StringToInt32(Value.c_str()) != 0)
		OutputValue = "Yes, I can hear it";
	else
		OutputValue = "No, I cannot hear it";

	SearchSelectedItem();
}

/*virtual*/ void WizardPageDTSTest::DoIncreaseSetting()
{
	std::string IndexText = Utils::CopyStr(Selected->GetName().c_str(), 3, 1);
	int ButtonIndex = Utils::StringToInt32(IndexText);
	if(ButtonIndex == 3)
		return;
	Selected->SetFocus(false);
	ButtonIndex++;
	std::string ButtonName = "Btn"+Utils::Int32ToString(ButtonIndex);
	Selected = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive(ButtonName));
	Selected->SetFocus(true);	
}

/*virtual*/ void WizardPageDTSTest::DoDecreaseSetting()
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

/*virtual*/ void WizardPageDTSTest::SearchSelectedItem()
{
	int ButtonIndex = Buttons[OutputValue];
	if(ButtonIndex == 0)
		return;
	std::string ButtonName = "Btn"+Utils::Int32ToString(ButtonIndex);
	Selected = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive(ButtonName));
	Selected->SetFocus(true);	
}
