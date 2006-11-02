/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef WizardPageVideoResolution_H_
#define WizardPageVideoResolution_H_

#include "WizardPage.h"

#include "WizardWidgetButton.h"
class WizardPageVideoResolution :
	public WizardPage
{
	WizardWidgetButton* Selected;
	SettingsDictionary* GlobalSettings;

	std::string PreviousResolution;
public:

	virtual void DefaultSetup(SettingsDictionary* AVWizardSettings);

	virtual int DoApplySetting(SettingsDictionary* Dictionary);

	virtual void DoIncreaseSetting();
	virtual void DoDecreaseSetting();
	virtual void DoClickWidget(WizardWidgetBase *pWidget);

	WizardPageVideoResolution(GenericBackEnd* FrontEnd, std::string Name);
	virtual ~WizardPageVideoResolution();
};

#endif
