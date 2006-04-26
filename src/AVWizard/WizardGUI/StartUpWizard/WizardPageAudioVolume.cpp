#include "WizardPageAudioVolume.h"

#include "GUIWizardUtils.h"

WizardPageAudioVolume::WizardPageAudioVolume(SDLFrontEnd* FrontEnd, std::string Name)
: WizardPage(FrontEnd, Name)
{
	AudioVolumeMin = 0;
	AudioVolumeMax = 31;
	AudioVolumeCurrent = 15;
	AudioVolumeIncrement = 1;
}

WizardPageAudioVolume::~WizardPageAudioVolume()
{
}


int WizardPageAudioVolume::DoApplySetting(SettingsDictionary* Dictionary)
{
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
	if(!AVWizardSettings->Exists("AudioVolumeMin"))
		AudioVolumeMin = Utils::StringToInt32(AVWizardSettings->GetValue("AudioVolumeMin"));

	if(AVWizardSettings->Exists("AudioVolumeMax"))
		AudioVolumeMax = Utils::StringToInt32(AVWizardSettings->GetValue("AudioVolumeMax"));

	if(AVWizardSettings->Exists("AudioVolumeCurrent"))
		AudioVolumeCurrent = Utils::StringToInt32(AVWizardSettings->GetValue("AudioVolumeCurrent"));

	if(AVWizardSettings->Exists("AudioVolumeIncrement"))
		AudioVolumeIncrement = Utils::StringToInt32(AVWizardSettings->GetValue("AudioVolumeIncrement"));

	VolumeLabel = dynamic_cast<WizardWidgetLabel*> (Page->GetChildRecursive("SpeakerVolumeText"));
	std::string VolumeLabelCaption = Utils::Int32ToString((AudioVolumeCurrent - AudioVolumeMin)*100/(AudioVolumeMax - AudioVolumeMin)) + "%";
	VolumeLabel->SetCaption(VolumeLabelCaption);
}

void WizardPageAudioVolume::DoIncreaseSetting()
{
	int NewVolume = AudioVolumeCurrent + AudioVolumeIncrement;
	if (NewVolume > AudioVolumeMax)
		NewVolume = AudioVolumeMax;

	AudioVolumeCurrent = NewVolume;

	VolumeLabel = dynamic_cast<WizardWidgetLabel*> (Page->GetChildRecursive("SpeakerVolumeText"));
	std::string VolumeLabelCaption = Utils::Int32ToString((AudioVolumeCurrent - AudioVolumeMin)*100/(AudioVolumeMax - AudioVolumeMin)) + "%";
	VolumeLabel->SetCaption(VolumeLabelCaption);

}

void WizardPageAudioVolume::DoDecreaseSetting()
{
	int NewVolume = AudioVolumeCurrent - AudioVolumeIncrement;
	if (NewVolume < AudioVolumeMin)
		NewVolume = AudioVolumeMin;
	AudioVolumeCurrent = NewVolume;

	VolumeLabel = dynamic_cast<WizardWidgetLabel*> (Page->GetChildRecursive("SpeakerVolumeText"));
	std::string VolumeLabelCaption = Utils::Int32ToString((AudioVolumeCurrent - AudioVolumeMin)*100/(AudioVolumeMax - AudioVolumeMin)) + "%";
	VolumeLabel->SetCaption(VolumeLabelCaption);
}
