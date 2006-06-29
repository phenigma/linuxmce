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
#include "Wizard.h"

WizardPageWelcome::WizardPageWelcome(SDLFrontEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{
#ifdef DEBUG
		std::cout<<"WizardPageWelcome::WizardPageWelcome"<<std::endl;
#endif
}

/*virtual*/ WizardPageWelcome::~WizardPageWelcome()
{

}

/*virtual*/ void WizardPageWelcome::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
#ifdef DEBUG
		std::cout<<"WizardPageWelcome::DefaultSetup"<<std::endl;
#endif
	WizardWidgetButton* Btn = dynamic_cast<WizardWidgetButton*>
		(Page->GetChildRecursive("MainBtn"));
	if(Btn == NULL)
		return;
	Btn->SetFocus(true);
}

/*virtual*/ int WizardPageWelcome::DoApplySetting(SettingsDictionary* Dictionary)
{
#ifdef DEBUG
		std::cout<<"WizardPageWelcome::DoApplySetting"<<std::endl;
#endif
	return 0;
}

/*virtual*/ void WizardPageWelcome::DoCancelSetting()
{
#ifdef DEBUG
		std::cout<<"WizardPageWelcome::DoCancelSetting"<<std::endl;
#endif
	Wizard::GetInstance()->SetExitWithCode(3);
}
