#ifndef WizardPageVideoRatio_H_
#define WizardPageVideoRatio_H_

#include "WizardPage.h"

#include "WizardWidgetButton.h"

#include <map>
#include <string>

class WizardPageVideoRatio :
	public WizardPage
{
	std::map<std::string, int> Buttons;

	std::string OutputValue;

	WizardWidgetButton* Selected;
	int RatioMode;
public:
	virtual void DefaultSetup(SettingsDictionary* AVWizardSettings);

	virtual int DoApplySetting(SettingsDictionary* Dictionary);

	virtual void DoIncreaseSetting();
	virtual void DoDecreaseSetting();

	WizardPageVideoRatio(SDLFrontEnd* FrontEnd, std::string Name);
	virtual ~WizardPageVideoRatio(void);

};

#endif
