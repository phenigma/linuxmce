#ifndef WizardPageVideoRatio_H_
#define WizardPageVideoRatio_H_

#include "WizardPage.h"

class WizardPageVideoRatio :
	public WizardPage
{
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
