#ifndef WizardPageDTSTest_H_
#define WizardPageDTSTest_H_ 

#include "WizardPage.h"

#include "WizardWidgetButton.h"

class WizardPageDTSTest :
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

	WizardPageDTSTest(SDLFrontEnd* FrontEnd, std::string Name);
	virtual ~WizardPageDTSTest();
};

#endif
