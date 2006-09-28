/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef WizardPageDTSTest_H_
#define WizardPageDTSTest_H_ 

#include "WizardPage.h"

#include "WizardWidgetButton.h"

#ifndef WIN32
#include "XinePlayer.h"
#endif

class WizardPageDTSTest :
	public WizardPage
{
	std::string OutputValue;

	std::map<std::string, int> Buttons;

	WizardWidgetButton* Selected;

	void SearchSelectedItem();
#ifndef WIN32
	XinePlayer* Player;
#endif
public:
	virtual int DoApplySetting(SettingsDictionary* Dictionary);
	virtual void DefaultSetup(SettingsDictionary* AVWizardSettings);
	virtual void DoIncreaseSetting();
	virtual void DoDecreaseSetting();

	WizardPageDTSTest(GenericBackEnd* FrontEnd, std::string Name);
	virtual ~WizardPageDTSTest();
};

#endif
