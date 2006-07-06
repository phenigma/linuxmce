#include "WizardPageDTSTest.h"

#include "GUIWizardUtils.h"

#include <iostream>

WizardPageDTSTest::WizardPageDTSTest(SDLFrontEnd* FrontEnd, std::string Name)
: WizardPage(FrontEnd, Name)
{
#ifdef DEBUG
	std::cout<<"WizardPageDTSTest::WizardPageDTSTest()"<<std::endl;
#endif
	Buttons["No, I cannot hear it"] = 1;
	Buttons["Yes, I can hear it"] = 2;
	OutputValue = "Yes, I can hear it";
	Selected = NULL;
	Player = NULL;
}

WizardPageDTSTest::~WizardPageDTSTest(void)
{
#ifdef DEBUG
	std::cout<<"WizardPageDTSTest::~WizardPageDTSTest()"<<std::endl;
#endif
	Player->StopPlayerEngine();
}

/*virtual*/ int WizardPageDTSTest::DoApplySetting(SettingsDictionary* Dictionary)
{
#ifdef DEBUG
	std::cout<<"WizardPageDTSTest::DoApplySetting()"<<std::endl;
#endif
	if(Selected == NULL || Dictionary == NULL)
		return -1;
	OutputValue = Selected->GetCaption();
	Dictionary->Set("DTSTest", OutputValue!= "No, I cannot hear it");
	return 0;
}

/*virtual*/ void WizardPageDTSTest::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
#ifdef DEBUG
	std::cout<<"WizardPageDTSTest::DefaultSetup()"<<std::endl;
#endif
	std::string FileName, ConfigName;
	Player = XinePlayer::GetInstance();
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
#ifdef DEBUG
	std::cout<<"WizardPageDTSTest::DoIncreaseSetting()"<<std::endl;
#endif
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

/*virtual*/ void WizardPageDTSTest::DoDecreaseSetting()
{
#ifdef DEBUG
	std::cout<<"WizardPageDTSTest::DoDecreaseSetting()"<<std::endl;
#endif
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
#ifdef DEBUG
	std::cout<<"WizardPageDTSTest::SearchSelectedItem()"<<std::endl;
#endif
	int ButtonIndex = Buttons[OutputValue];
	if(ButtonIndex == 0)
		return;
	std::string ButtonName = "Btn"+Utils::Int32ToString(ButtonIndex);
	Selected = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive(ButtonName));
	Selected->SetFocus(true);	
}

