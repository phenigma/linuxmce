//
// C++ Interface: WizardPageUISwitcher
//
// Description: 
//
//
// Author: Ciprian Mustiata,,, <ciplogic@ciplogic-desktop>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef WizardPageUISwitcher_H_
#define WizardPageUISwitcher_H_

#include "WizardPage.h"

#include "WizardWidgetButton.h"

class WizardPageUISwitcher :
	public WizardPage
{
	std::string OutputValue;

	std::map<std::string, int> Buttons;

	WizardWidgetButton* Selected;

public:
	virtual int DoApplySetting(SettingsDictionary* Dictionary);
	virtual void DefaultSetup(SettingsDictionary* AVWizardSettings);
	virtual void DoIncreaseSetting();
	virtual void DoDecreaseSetting();
	virtual void DoNextFocusItem();
	virtual void DoPreviousFocusItem();

	WizardPageUISwitcher(SDLFrontEnd* FrontEnd, std::string Name);
	virtual ~WizardPageUISwitcher();
};

#endif 
