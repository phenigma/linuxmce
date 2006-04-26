#ifndef WizardPageVideoOutput_H_
#define WizardPageVideoOutput_H_

#include "WizardPage.h"

#include "WizardWidgetButton.h"

#include <map>
#include <string>

class WizardPageVideoOutput :
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

	WizardPageVideoOutput(SDLFrontEnd* FrontEnd, std::string Name);
	virtual ~WizardPageVideoOutput();
};

#endif
