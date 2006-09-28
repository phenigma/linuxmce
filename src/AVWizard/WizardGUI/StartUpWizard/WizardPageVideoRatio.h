/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef WizardPageVideoRatio_H_
#define WizardPageVideoRatio_H_

#include "WizardPage.h"

#include "WizardWidgetScrollList.h"

#include <map>
#include <string>

class WizardPageVideoRatio :
	public WizardPage
{
	std::map<std::string, int> Buttons;

	std::string OutputValue;

	WizardWidgetScrollList* Selected;
	int RatioMode;

	void SearchSelectedItem();
public:
	virtual void DefaultSetup(SettingsDictionary* AVWizardSettings);

	virtual int DoApplySetting(SettingsDictionary* Dictionary);

	virtual void DoIncreaseSetting();
	virtual void DoDecreaseSetting();
	virtual void DoNextFocusItem();
	virtual void DoPreviousFocusItem();


	WizardPageVideoRatio(GenericBackEnd* FrontEnd, std::string Name);
	virtual ~WizardPageVideoRatio(void);

};

#endif
