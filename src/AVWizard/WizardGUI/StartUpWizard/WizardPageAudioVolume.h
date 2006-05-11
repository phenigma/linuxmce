#ifndef WizardPageAudioVolume_H_ 
#define  WizardPageAudioVolume_H_

#include "WizardPage.h"

#include "WizardWidgetLabel.h"

#include "XinePlayer.h"

class WizardPageAudioVolume :
	public WizardPage
{
	int AudioVolumeMin, AudioVolumeMax, AudioVolumeCurrent, AudioVolumeIncrement;
	WizardWidgetLabel* VolumeLabel;

	XinePlayer* Player;
public:
	virtual void DefaultSetup(SettingsDictionary* AVWizardSettings);

	virtual int DoApplySetting(SettingsDictionary* Dictionary);

	virtual void DoIncreaseSetting();
	virtual void DoDecreaseSetting();

	WizardPageAudioVolume(SDLFrontEnd* FrontEnd, std::string Name);
	virtual ~WizardPageAudioVolume(void);
};

#endif
