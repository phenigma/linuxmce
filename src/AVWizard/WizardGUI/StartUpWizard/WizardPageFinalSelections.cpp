#include "WizardPageFinalSelections.h"

#include "WizardWidgetLabel.h"
#include "GUIWizardUtils.h"

#include "Wizard.h"

WizardPageFinalSelections::WizardPageFinalSelections(SDLFrontEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{
	Buttons["I do not agree"] = 1;
	Buttons["I agree"] = 2;
	OutputValue = "I agree";
	Selected = NULL;
}

WizardPageFinalSelections::~WizardPageFinalSelections(void)
{
}

/*virtual*/ void WizardPageFinalSelections::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
	WizardWidgetLabel*Selected;
	std::string LabelName;
	std::string CaptionValue;
	for(int i = 1; i<=8; i++)
	{
		LabelName = "OptionText"+Utils::Int32ToString(i);
		Selected = dynamic_cast<WizardWidgetLabel*> (Page->GetChildRecursive(LabelName));

		CaptionValue = Utils::Int32ToString(i) + ". ";
		switch(i) {
		case 1:
			CaptionValue = CaptionValue +"Screen ratio: "+ GetVideoRatio(AVWizardSettings);
			break;
		case 2:
			CaptionValue = CaptionValue +"Video output: "+ GetVideoOutput(AVWizardSettings);
			break;
		case 3:
			CaptionValue = CaptionValue +"Video resolution: "+ GetVideoResolution(AVWizardSettings);			
			break;
		case 4:
			CaptionValue = CaptionValue +"Video refresh: "+ GetVideoRefresh(AVWizardSettings);			
			break;
		case 5: 
			CaptionValue = CaptionValue +"Audio connector: "+ GetAudioConnector(AVWizardSettings);
			break;
		case 6:
			CaptionValue = CaptionValue +"Audio volume: "+ GetVolume(AVWizardSettings);
			break;
		case 7: CaptionValue = CaptionValue +"Dolby sound test passed: "
					+ GetDolbyTestStatus(AVWizardSettings);
			break;
		case 8: CaptionValue = CaptionValue +"DTS sound test passed: "
				   + GetDTSTestStatus(AVWizardSettings);
			break;
		default:;
		}
		Selected->SetCaption(CaptionValue);
	}
	SearchSelectedItem();
}

std::string WizardPageFinalSelections::GetVideoRatio(SettingsDictionary* AVWizardSettings)
{
	int RatioMode = 0;
	if(!AVWizardSettings->Exists("VideoRatio"))
	{
		RatioMode = 0;
		return "4:3";
	}
	std::string RatioValue = AVWizardSettings->GetValue("VideoRatio");
	RatioMode = (RatioValue == "4_3");

	RatioValue = "16:9";
	if(RatioMode != 0)
		RatioValue = "4:3";
	return RatioValue;
}

std::string WizardPageFinalSelections::GetVideoOutput(SettingsDictionary* AVWizardSettings)
{
	if(!AVWizardSettings->Exists("VideoOutput"))
	{
		return "VGA";
	}
	std::string OutputValue = AVWizardSettings->GetValue("VideoOutput");

	return OutputValue;
}

std::string WizardPageFinalSelections::GetVideoResolution(SettingsDictionary* AVWizardSettings)
{
	return "";
}

std::string WizardPageFinalSelections::GetVideoRefresh(SettingsDictionary* AVWizardSettings)
{
	return "";
}

std::string WizardPageFinalSelections::GetAudioConnector(SettingsDictionary* AVWizardSettings)
{
	if(!AVWizardSettings->Exists("AudioConnector"))
	{
		return "Toslink";
	}
	std::string OutputValue = AVWizardSettings->GetValue("AudioConnector");
	return OutputValue;
}

std::string WizardPageFinalSelections::GetVolume(SettingsDictionary* AVWizardSettings)
{
	int AudioVolumeMin, AudioVolumeMax, AudioVolumeCurrent;
	AudioVolumeMin = 0;
	AudioVolumeMax = 31;
	AudioVolumeCurrent = 15;

	std::string OptionName = "AudioVolumeMin";
	if(!AVWizardSettings->Exists(OptionName))
		AudioVolumeMin = Utils::StringToInt32(AVWizardSettings->GetValue("AudioVolumeMin"));

	if(AVWizardSettings->Exists("AudioVolumeMax"))
		AudioVolumeMax = Utils::StringToInt32(AVWizardSettings->GetValue("AudioVolumeMax"));

	if(AVWizardSettings->Exists("AudioVolumeCurrent"))
		AudioVolumeCurrent = Utils::StringToInt32(AVWizardSettings->GetValue("AudioVolumeCurrent"));

	return Utils::Int32ToString((AudioVolumeCurrent - AudioVolumeMin)*100/(AudioVolumeMax - AudioVolumeMin)) + "%";;
}

std::string WizardPageFinalSelections::GetDolbyTestStatus(SettingsDictionary* AVWizardSettings)
{
	std::string Value = AVWizardSettings->GetValue("DolbyTest");
	int PassDolbyTest = Utils::StringToInt32(Value) != 0;
	if (PassDolbyTest)
		return "true";
	
	return "false";
}

std::string WizardPageFinalSelections::GetDTSTestStatus(SettingsDictionary* AVWizardSettings)
{
	std::string Value = AVWizardSettings->GetValue("DTSTest");
	int PassDolbyTest = Utils::StringToInt32(Value) != 0;
	if (PassDolbyTest)
		return "true";
	

	return "false";
}

/*virtual*/ void WizardPageFinalSelections::DoIncreaseSetting()
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

/*virtual*/ void WizardPageFinalSelections::DoDecreaseSetting()
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

/*virtual*/ void WizardPageFinalSelections::SearchSelectedItem()
{
	int ButtonIndex = Buttons[OutputValue];
	if(ButtonIndex == 0)
		return;
	std::string ButtonName = "Btn"+Utils::Int32ToString(ButtonIndex);
	Selected = dynamic_cast<WizardWidgetButton*> (Page->GetChildRecursive(ButtonName));
	Selected->SetFocus(true);	
}

/*virtual*/ int WizardPageFinalSelections::DoApplySetting(SettingsDictionary* Dictionary)
{
	if(Selected == NULL)
		return 0;

	if(Buttons[Selected->GetCaption()] == 1)
	{
		Wizard::GetInstance()->SetExitWithCode(1);
	}
	return 0;
}
