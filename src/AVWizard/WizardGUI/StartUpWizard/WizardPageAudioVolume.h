/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef WizardPageAudioVolume_H_ 
#define  WizardPageAudioVolume_H_

#include "WizardPage.h"

#include "WizardWidgetLabel.h"
#ifndef WIN32
#include "XinePlayer.h"
#endif

class WizardPageAudioVolume :
	public WizardPage
{
	int AudioVolumeMin, AudioVolumeMax, AudioVolumeCurrent, AudioVolumeIncrement;
	WizardWidgetLabel* VolumeLabel;

#ifndef WIN32
	XinePlayer* Player;
#endif
public:
	virtual void DefaultSetup(SettingsDictionary* AVWizardSettings);

	virtual int DoApplySetting(SettingsDictionary* Dictionary);

	virtual void DoIncreaseSetting();
	virtual void DoDecreaseSetting();
	virtual void DoClickWidget(WizardWidgetBase *pWidget);

	WizardPageAudioVolume(GenericBackEnd* FrontEnd, std::string Name);
	virtual ~WizardPageAudioVolume(void);
};

#endif
