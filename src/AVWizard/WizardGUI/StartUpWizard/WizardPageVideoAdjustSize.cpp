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
	PreviousAdjustSize = "640x480";
}

WizardPageVideoAdjustSize::~WizardPageVideoAdjustSize(void)
{
}


/*virtual*/ int WizardPageVideoAdjustSize::DoApplySetting(SettingsDictionary* Dictionary)
{
	if(Dictionary == NULL)
		return -1;

	WizardWidgetListBox* ResListBox = dynamic_cast<WizardWidgetListBox*> 
		(Page->GetChildRecursive("ListBox1"));

	Dictionary->Set("VideoAdjustSize", ResListBox->GetCaption());

	WizardWidgetListBox* RefreshListBox = dynamic_cast<WizardWidgetListBox*> 
		(Page->GetChildRecursive("ListBox2"));

	Dictionary->Set("VideoRefresh", RefreshListBox->GetCaption());

	Dictionary->Set("AdjustSizeSelected", Selected->GetName() == "ListBox1" );

	return 0;
}

/*virtual*/ void WizardPageVideoAdjustSize::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
	this->GlobalSettings = AVWizardSettings;
	WizardWidgetListBox* ResListBox = dynamic_cast<WizardWidgetListBox*> 
		(Page->GetChildRecursive("ListBox1"));

	std::string AdjustSizeStr = "640x480";
	if(AVWizardSettings->Exists("VideoAdjustSize"))
		AdjustSizeStr =  AVWizardSettings->GetValue("VideoAdjustSize");
	
	ResListBox->SetCaption(AdjustSizeStr);

	WizardWidgetListBox* RefreshListBox = dynamic_cast<WizardWidgetListBox*> 
		(Page->GetChildRecursive("ListBox2"));

	std::string RefreshStr = "60Hz";
	if(AVWizardSettings->Exists("VideoRefresh"))
		RefreshStr =  AVWizardSettings->GetValue("VideoRefresh");
	
	RefreshListBox->SetCaption(RefreshStr);
	
	
	if(AVWizardSettings->Exists("PreviousAdjustSize"))
		PreviousAdjustSize = AVWizardSettings->GetValue("PreviousAdjustSize");

	bool IsSelectedAdjustSizeListBox = Utils::StringToInt32(AVWizardSettings->GetValue("AdjustSizeSelected"));
	ResListBox->SetFocus(IsSelectedAdjustSizeListBox);
	RefreshListBox->SetFocus(!IsSelectedAdjustSizeListBox);
	if(IsSelectedAdjustSizeListBox)
		Selected = ResListBox;
	else
		Selected = RefreshListBox;
}

/*virtual*/ void WizardPageVideoAdjustSize::DoIncreaseSetting()
{
	DoApplySetting(GlobalSettings);
	WM_Event Event;
	Event.Save();
	Wizard::GetInstance()->GenerateCustomEvent(Event);

	std::string ListBoxName = Selected->GetName();
	#ifdef DEBUG
	std::cout<<"Caption: "<<Selected->GetName()<<std::endl;
	#endif
	if (ListBoxName == "ListBox1")
	{
		#ifdef DEBUG
		std::cout<<SkinGenerator::Instance()->CommandSetResolutionPlus<<std::endl;
		#endif
		system(SkinGenerator::Instance()->CommandSetResolutionPlus.c_str());
	}
	else
	{
		#ifdef DEBUG
		std::cout<<SkinGenerator::Instance()->CommandSetRefreshPlus<<std::endl;
		#endif
		system(SkinGenerator::Instance()->CommandSetRefreshPlus.c_str());
	}

}

/*virtual*/ void WizardPageVideoAdjustSize::DoDecreaseSetting()
{
	DoApplySetting(GlobalSettings);
	WM_Event Event;
	Event.Save();
	Wizard::GetInstance()->GenerateCustomEvent(Event);

	std::string ListBoxName = Selected->GetName();
	#ifdef DEBUG
	std::cout<<"Caption: "<<Selected->GetName()<<std::endl;
	#endif
	if (ListBoxName == "ListBox1")
	{
		#ifdef DEBUG
		std::cout<<SkinGenerator::Instance()->CommandSetResolutionMinus<<std::endl;
		#endif
		system(SkinGenerator::Instance()->CommandSetResolutionMinus.c_str());
	}
	else
	{
		#ifdef DEBUG
		std::cout<<SkinGenerator::Instance()->CommandSetRefreshMinus<<std::endl;
		#endif
		system(SkinGenerator::Instance()->CommandSetRefreshMinus.c_str());
	}
}

/*virtual*/ void WizardPageVideoAdjustSize::DoNextFocusItem()
{
	Selected->SetFocus(false);
	std::string ListBoxName = "ListBox2";
	Selected = dynamic_cast<WizardWidgetListBox*> (Page->GetChildRecursive(ListBoxName));
	if(Selected == NULL)
		return;
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
	Selected->SetFocus(true);
	#ifdef DEBUG
	std::cout<<"Caption: "<<Selected->GetName()<<std::endl;
	#endif
	Selected->SetFocus(true);
}
