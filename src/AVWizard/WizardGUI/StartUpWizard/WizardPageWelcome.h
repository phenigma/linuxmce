#ifndef WizardPageWelcome_H_
#define WizardPageWelcome_H_

#include "WizardPage.h"

#include "WizardWidgetListBox.h"

class WizardPageWelcome :
	public WizardPage
{
public:
	virtual void DefaultSetup(SettingsDictionary* AVWizardSettings);

	virtual int DoApplySetting(SettingsDictionary* Dictionary);

	WizardPageWelcome(SDLFrontEnd* FrontEnd, std::string Name);
	virtual ~WizardPageWelcome();
};

#endif //WizardPageWelcome_H_
