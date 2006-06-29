#include "WizardPageAudioConnector.h"

#include "GUIWizardUtils.h"
#include "ConfigureCommons.h"

#include "SkinGenerator.h"

WizardPageAudioConnector::WizardPageAudioConnector(SDLFrontEnd* FrontEnd, std::string Name)
: WizardPage(FrontEnd, Name)
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioConnector::WizardPageAudioConnector"<<std::endl;
#endif
	Buttons["Analog Stereo"] = 1;
	Buttons["SPDIF Coaxial"] = 2;
	Buttons["SPDIF Optical"] = 3;
	OutputValue = "SPDIF Optical";
	Selected = NULL;
}

WizardPageAudioConnector::~WizardPageAudioConnector(void)
{
}


/*virtual*/ int WizardPageAudioConnector::DoApplySetting(SettingsDictionary* Dictionary)
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioConnector::DoApplySetting"<<std::endl;
#endif
	if(Dictionary == NULL)
		return -1;
	OutputValue = Selected->GetCaption();
	Dictionary->Set("AudioConnector", OutputValue);
	std::string Command = SkinGenerator::Instance()->CommandSetAudioConnector;
	Command = Command + " '" + OutputValue + "'";
	system(Command.c_str());
	return 0;
}

/*virtual*/ void WizardPageAudioConnector::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioConnector::DefaultSetup"<<std::endl;
#endif
	if(!AVWizardSettings->Exists("AudioConnector"))
	{
		SearchSelectedItem();
		return;
	}
	OutputValue = AVWizardSettings->GetValue("AudioConnector");
	SearchSelectedItem();
}

/*virtual*/ void WizardPageAudioConnector::DoIncreaseSetting()
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioConnector::DoIncreaseSetting"<<std::endl;
#endif
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

/*virtual*/ void WizardPageAudioConnector::DoDecreaseSetting()
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioConnector::DoDecreaseSetting"<<std::endl;
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

/*virtual*/ void WizardPageAudioConnector::SearchSelectedItem()
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioConnector::SearchSelectedItem"<<std::endl;
#endif
	int ButtonIndex = Buttons[OutputValue];
	if(ButtonIndex == 0)
		return;
	std::string ButtonName = "Btn"+Utils::Int32ToString(ButtonIndex);
	Selected = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive(ButtonName));
	Selected->SetFocus(true);	
}
