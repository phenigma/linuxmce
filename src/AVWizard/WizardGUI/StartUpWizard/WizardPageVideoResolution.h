/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef WizardPageVideoResolution_H_
#define WizardPageVideoResolution_H_

#include "WizardPage.h"

#include "WizardWidgetLabel.h"
#include "WizardWidgetButton.h"
#include "ThreadSleeper.h"

class VideoResolutionSecondSleeper : public ThreadSleeper
{
	WizardWidgetLabel* Label;
public:
	VideoResolutionSecondSleeper(int Seconds);
	virtual void SecondTick();
	virtual void SetLabel(WizardWidgetLabel* Label);
};

class WizardPageVideoResolution :
	public WizardPage
{
	WizardWidgetButton* Selected;
	SettingsDictionary* GlobalSettings;
	VideoResolutionSecondSleeper* Sleeper;

	std::string PreviousResolution;
public:

	virtual void DefaultSetup(SettingsDictionary* AVWizardSettings);

	virtual int DoApplySetting(SettingsDictionary* Dictionary);

	virtual void DoIncreaseSetting();
	virtual void DoDecreaseSetting();

	WizardPageVideoResolution(SDLFrontEnd* FrontEnd, std::string Name);
	virtual ~WizardPageVideoResolution();
};

#endif
