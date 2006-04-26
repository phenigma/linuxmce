#ifndef WizardPageDolbyTest_H_
#define WizardPageDolbyTest_H_

#include "WizardPage.h"

#include "WizardWidgetButton.h"

class WizardPageDolbyTest :
	public WizardPage
{
	std::string OutputValue;

	std::map<std::string, int> Buttons;

	WizardWidgetButton* Selected;

	void SearchSelectedItem();
public:
	virtual int DoApplySetting(SettingsDictionary* Dictionary);
	virtual void DefaultSetup(SettingsDictionary* AVWizardSettings);
	virtual void DoIncreaseSetting();
	virtual void DoDecreaseSetting();

	WizardPageDolbyTest(SDLFrontEnd* FrontEnd, std::string Name);
	virtual ~WizardPageDolbyTest(void);
};

#endif
