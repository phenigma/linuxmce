//
// C++ Implementation: WizardPageWelcome
//
// Description: 
//
//
// Author: Ciprian Mustiata <ciprian.m@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "WizardPageWelcome.h"

WizardPageWelcome::WizardPageWelcome(SDLFrontEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{

}

/*virtual*/ WizardPageWelcome::~WizardPageWelcome()
{

}

/*virtual*/ void WizardPageWelcome::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
	WizardWidgetButton* Btn = dynamic_cast<WizardWidgetButton*>
		(Page->GetChildRecursive("MainBtn"));
	if(Btn == NULL)
		return;
	Btn->SetFocus(true);
}

/*virtual*/ int WizardPageWelcome::DoApplySetting(SettingsDictionary* Dictionary)
{
	return 0;
}

