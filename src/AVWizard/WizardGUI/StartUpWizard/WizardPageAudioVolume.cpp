#include "WizardPageAudioVolume.h"

#include "GUIWizardUtils.h"

#include <stdlib.h>
#include <iostream>

WizardPageAudioVolume::WizardPageAudioVolume(SDLFrontEnd* FrontEnd, std::string Name)
: WizardPage(FrontEnd, Name)
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioVolume::WizardPageAudioVolume"<<std::endl;
#endif
	AudioVolumeMin = 0;
	AudioVolumeMax = 31;
	AudioVolumeCurrent = 15;
	AudioVolumeIncrement = 1;
	Player = NULL;
}

WizardPageAudioVolume::~WizardPageAudioVolume()
{
#ifdef DEBUG
	std::cout<<"dtor WizardPageAudioVolume"<<std::endl;
#endif
	Player->StopPlayerEngine();
}


int WizardPageAudioVolume::DoApplySetting(SettingsDictionary* Dictionary)
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioVolume::WizardPageAudioVolume"<<std::endl;
#endif
	if(Dictionary == NULL)
		return -1;
	Dictionary->Set("AudioVolumeMin", AudioVolumeMin);
	Dictionary->Set("AudioVolumeMax", AudioVolumeMax);
	Dictionary->Set("AudioVolumeCurrent", AudioVolumeCurrent);
	Dictionary->Set("AudioVolumeIncrement", AudioVolumeIncrement);
	return 0;
}

/*virtual*/ void WizardPageAudioVolume::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioVolume::DefaultSetup"<<std::endl;
#endif
	std::string FileName, ConfigName;
	Player = XinePlayer::GetInstance();
	if(AVWizardSettings->Exists("XineConfigFile"))
	{
		ConfigName = AVWizardSettings->GetValue("XineConfigFile");

	}
	
	if(AVWizardSettings->Exists("SoundTestFile"))
		FileName = AVWizardSettings->GetValue("SoundTestFile");
	if(FileName != "")
		Player->InitPlayerEngine(ConfigName, FileName);

	if(FileName != "")
	{
		FILE* f = fopen(FileName.c_str(), "r");
		if(NULL != f)
		{
			fclose(f);
			WizardWidgetLabel* Label = NULL;
			Label = dynamic_cast<WizardWidgetLabel*>(Page->GetChildRecursive("DescribeText2"));
			Label->SetUpAttribute( "Visible", "0");
			Label = (WizardWidgetLabel*)Page->GetChildRecursive("DescribeText3");
			Label->SetUpAttribute( "Visible", "0");
		}
	}
	if(!AVWizardSettings->Exists("AudioVolumeMin"))
		AudioVolumeMin = Utils::StringToInt32(AVWizardSettings->GetValue("AudioVolumeMin"));

	if(AVWizardSettings->Exists("AudioVolumeMax"))
		AudioVolumeMax = Utils::StringToInt32(AVWizardSettings->GetValue("AudioVolumeMax"));

	if(AVWizardSettings->Exists("AudioVolumeCurrent"))
		AudioVolumeCurrent = Utils::StringToInt32(AVWizardSettings->GetValue("AudioVolumeCurrent"));

	if(AVWizardSettings->Exists("AudioVolumeIncrement"))
		AudioVolumeIncrement = Utils::StringToInt32(AVWizardSettings->GetValue("AudioVolumeIncrement"));

	std::string VolumeLabelCaption = Utils::Int32ToString((AudioVolumeCurrent - AudioVolumeMin)*100/(AudioVolumeMax - AudioVolumeMin)) + "%";

	VolumeLabel = dynamic_cast<WizardWidgetLabel*> (Page->GetChildRecursive("SpeakerVolumeTextShadow"));
	VolumeLabel->SetCaption(VolumeLabelCaption);
	VolumeLabel = dynamic_cast<WizardWidgetLabel*> (Page->GetChildRecursive("SpeakerVolumeText"));
	VolumeLabel->SetCaption(VolumeLabelCaption);
}

void WizardPageAudioVolume::DoIncreaseSetting()
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioVolume::DoIncreaseSetting"<<std::endl;
#endif
	int NewVolume = AudioVolumeCurrent + AudioVolumeIncrement;
	if (NewVolume > AudioVolumeMax)
		NewVolume = AudioVolumeMax;

	AudioVolumeCurrent = NewVolume;

	std::string VolumeLabelCaption = Utils::Int32ToString((AudioVolumeCurrent - AudioVolumeMin) 
		* 100 / (AudioVolumeMax - AudioVolumeMin)) + "%";

	VolumeLabel = dynamic_cast<WizardWidgetLabel*> (Page->GetChildRecursive("SpeakerVolumeTextShadow"));
	VolumeLabel->SetCaption(VolumeLabelCaption);

	VolumeLabel = dynamic_cast<WizardWidgetLabel*> (Page->GetChildRecursive("SpeakerVolumeText"));
	VolumeLabel->SetCaption(VolumeLabelCaption);

	std::string Command = "amixer sset Master "+Utils::Int32ToString(AudioVolumeCurrent) +
		" unmute >/dev/null";
	system(Command.c_str());

}

void WizardPageAudioVolume::DoDecreaseSetting()
{
#ifdef DEBUG
	std::cout<<"WizardPageAudioVolume::DoDecreaseSetting"<<std::endl;
#endif
	int NewVolume = AudioVolumeCurrent - AudioVolumeIncrement;
	if (NewVolume < AudioVolumeMin)
		NewVolume = AudioVolumeMin;
	AudioVolumeCurrent = NewVolume;


	std::string VolumeLabelCaption = Utils::Int32ToString((AudioVolumeCurrent - AudioVolumeMin)*100/(AudioVolumeMax - AudioVolumeMin)) + "%";
	VolumeLabel = dynamic_cast<WizardWidgetLabel*> (Page->GetChildRecursive("SpeakerVolumeTextShadow"));
	VolumeLabel->SetCaption(VolumeLabelCaption);
	VolumeLabel = dynamic_cast<WizardWidgetLabel*> (Page->GetChildRecursive("SpeakerVolumeText"));
	VolumeLabel->SetCaption(VolumeLabelCaption);
	
	std::string Command = "amixer sset Master "+Utils::Int32ToString(AudioVolumeCurrent) +
		" unmute >/dev/null";
	system(Command.c_str());
}
