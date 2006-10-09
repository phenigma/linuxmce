//
// C++ Implementation: WizardPageUISwitcher
//
// Description: 
//
//
// Author: Ciprian Mustiata,,, <ciplogic@ciplogic-desktop>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "WizardPageUISwitcher.h"

#include "WizardPageDTSTest.h"

#include "GUIWizardUtils.h"

#include <iostream>

#include "WizardWidgetLabel.h"
#include "WizardWidgetScrollList.h"
#include "Wizard.h"

WizardPageUISwitcher::WizardPageUISwitcher(GenericBackEnd* FrontEnd, std::string Name)
: WizardPage(FrontEnd, Name)
{
#ifdef DEBUG
	std::cout<<"WizardPageDTSTest::WizardPageDTSTest()"<<std::endl;
#endif
	Buttons["UI Version 2"] = 1;
	Buttons["Simple Interface"] = 2;
	OutputValue = "Yes, I can hear it";
	Selected = NULL;
}

WizardPageUISwitcher::~WizardPageUISwitcher(void)
{
#ifdef DEBUG
	std::cout<<"WizardPageDTSTest::~WizardPageDTSTest()"<<std::endl;
#endif
}

/*virtual*/ int WizardPageUISwitcher::DoApplySetting(SettingsDictionary* Dictionary)
{
#ifdef DEBUG
	std::cout<<"WizardPageDTSTest::DoApplySetting()"<<std::endl;
#endif
	WizardWidgetScrollList* List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("UIScroll"));
	if(NULL == List)
	{
	#ifdef DEBUG
		std::cout<<"Warning: WizardPageDTSTest::DefaultSetup()"<<std::endl;
	#endif
		return -1;
	}

	OutputValue = List->GetSelectedValue();
	Dictionary->Set("UIVersion", OutputValue);
	return 0;
}

/*virtual*/ void WizardPageUISwitcher::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
#ifdef DEBUG
	std::cout<<"WizardPageDTSTest::DefaultSetup()"<<std::endl;
#endif
	WizardWidgetScrollList* List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("UIScroll"));
	if(NULL == List)
	{
	#ifdef DEBUG
		std::cout<<"Warning: WizardPageDTSTest::DefaultSetup()"<<std::endl;
	#endif
		return;
	}
	List->AddItem("Alpha blended", "UI2");
	List->AddItem("Normal UI", "UI1");
	List->SetFocus(true);
}

/*virtual*/ void WizardPageUISwitcher::DoIncreaseSetting()
{
	WizardWidgetScrollList* List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("UIScroll"));
	if(NULL == List)
	{
	#ifdef DEBUG
		std::cout<<"Warning: WizardPageDTSTest::DefaultSetup()"<<std::endl;
	#endif
		return;
	}
	List->SetItemIndex(List->GetItemIndex()+1);
}

/*virtual*/ void WizardPageUISwitcher::DoDecreaseSetting()
{
	WizardWidgetScrollList* List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("UIScroll"));
	if(NULL == List)
	{
	#ifdef DEBUG
		std::cout<<"Warning: WizardPageDTSTest::DefaultSetup()"<<std::endl;
	#endif
		return;
	}
	List->SetItemIndex(List->GetItemIndex()-1);
}

/*virtual*/ void WizardPageUISwitcher::DoNextFocusItem()
{
	WizardWidgetScrollList* List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("UIScroll"));
	if(NULL == List)
	{
	#ifdef DEBUG
		std::cout<<"Warning: WizardPageDTSTest::DefaultSetup()"<<std::endl;
	#endif
		return;
	}
	List->SetItemIndex(List->GetItemIndex()+1);
}

/*virtual*/ void WizardPageUISwitcher::DoPreviousFocusItem()
{
	WizardWidgetScrollList* List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("UIScroll"));
	if(NULL == List)
	{
	#ifdef DEBUG
		std::cout<<"Warning: WizardPageDTSTest::DefaultSetup()"<<std::endl;
	#endif
		return;
	}
	List->SetItemIndex(List->GetItemIndex()-1);
}
