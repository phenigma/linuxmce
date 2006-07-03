#include "WizardPageVideoAdjustSize.h"

#include "ConfigureCommons.h"

#include "GUIWizardUtils.h"

#include <iostream>

#include "SettingsDictionaryTree.h"

#include "Wizard.h"

#include "SkinGenerator.h"

WizardPageVideoAdjustSize::WizardPageVideoAdjustSize(SDLFrontEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{
	Selected = NULL;
}

WizardPageVideoAdjustSize::~WizardPageVideoAdjustSize(void)
{
}


/*virtual*/ int WizardPageVideoAdjustSize::DoApplySetting(SettingsDictionary* Dictionary)
{
	if(Dictionary == NULL)
		return -1;



	return 0;
}

/*virtual*/ void WizardPageVideoAdjustSize::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
	this->GlobalSettings = AVWizardSettings;
	WizardWidgetListBox* ResListBox = dynamic_cast<WizardWidgetListBox*> 
		(Page->GetChildRecursive("ListBox1"));
	ResListBox->SetFocus(true);
	Selected = ResListBox;
}

/*virtual*/ void WizardPageVideoAdjustSize::DoIncreaseSetting()
{
	std::string ListBoxName = Selected->GetName();
	#ifdef DEBUG
	std::cout<<"Caption: "<<Selected->GetName()<<std::endl;
	#endif
	if (ListBoxName == "ListBox1")
	{
		if(Wizard::GetInstance()->WizardBorder*2 > 
			Wizard::GetInstance()->LeftBorder )
		Wizard::GetInstance()->LeftBorder ++;
	}
	else
	{
		if(Wizard::GetInstance()->WizardBorder*2 > 
			Wizard::GetInstance()->TopBorder )
		Wizard::GetInstance()->TopBorder ++;
	}
}

/*virtual*/ void WizardPageVideoAdjustSize::DoDecreaseSetting()
{
	std::string ListBoxName = Selected->GetName();
	#ifdef DEBUG
	std::cout<<"Caption: "<<Selected->GetName()<<std::endl;
	#endif
	if (ListBoxName == "ListBox1")
	{
		if(Wizard::GetInstance()->LeftBorder>0)
			Wizard::GetInstance()->LeftBorder --;
	}
	else
	{
		if(Wizard::GetInstance()->TopBorder>0)
			Wizard::GetInstance()->TopBorder --;
	}
}

/*virtual*/ void WizardPageVideoAdjustSize::DoNextFocusItem()
{
	Selected->SetFocus(false);
	std::string ListBoxName = "ListBox2";
	Selected = dynamic_cast<WizardWidgetListBox*> (Page->GetChildRecursive(ListBoxName));
	if(Selected == NULL)
		return;
	Selected->SetFocus(true);
	#ifdef DEBUG
	std::cout<<"Caption: "<<Selected->GetName()<<std::endl;
	#endif
	Selected->SetFocus(true);
}

/*virtual*/ void WizardPageVideoAdjustSize::DoPreviousFocusItem()
{
	Selected->SetFocus(false);
	std::string ListBoxName = "ListBox1";
	Selected = dynamic_cast<WizardWidgetListBox*> (Page->GetChildRecursive(ListBoxName));
	if(Selected == NULL)
		return;
	#ifdef DEBUG
	std::cout<<"Caption: "<<Selected->GetName()<<std::endl;
	#endif
	Selected->SetFocus(true);
}
