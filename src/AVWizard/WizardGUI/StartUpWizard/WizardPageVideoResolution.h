#ifndef WizardPageVideoResolution_H_
#define WizardPageVideoResolution_H_

#include "WizardPage.h"

#include "WizardWidgetListBox.h"

class WizardPageVideoResolution :
	public WizardPage
{
	WizardWidgetListBox* Selected;
	SettingsDictionary* GlobalSettings;
public:

	virtual void DefaultSetup(SettingsDictionary* AVWizardSettings);

	virtual int DoApplySetting(SettingsDictionary* Dictionary);

	virtual void DoIncreaseSetting();
	virtual void DoDecreaseSetting();
	virtual void DoNextFocusItem();
	virtual void DoPreviousFocusItem();

	WizardPageVideoResolution(SDLFrontEnd* FrontEnd, std::string Name);
	virtual ~WizardPageVideoResolution();
};

#endif
