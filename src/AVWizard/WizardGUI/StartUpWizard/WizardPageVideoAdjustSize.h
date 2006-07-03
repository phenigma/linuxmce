#ifndef WizardPageVideoAdjustSize_H_
#define WizardPageVideoAdjustSize_H_

#include "WizardPage.h"

#include "WizardWidgetButton.h"

class WizardPageVideoAdjustSize :
	public WizardPage
{
	SettingsDictionary* GlobalSettings;

	std::string PreviousResolution;
public:

	virtual void DefaultSetup(SettingsDictionary* AVWizardSettings);

	virtual int DoApplySetting(SettingsDictionary* Dictionary);

	virtual void DoIncreaseSetting();
	virtual void DoDecreaseSetting();
	virtual void DoNextFocusItem();
	virtual void DoPreviousFocusItem();

	WizardPageVideoAdjustSize(SDLFrontEnd* FrontEnd, std::string Name);
	virtual ~WizardPageVideoAdjustSize();

	virtual void UpdateSelected();
};

#endif
