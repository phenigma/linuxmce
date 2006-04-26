#include "WizardPage.h"

#include "WizardWidgetButton.h"

#include "Wizard.h"

WizardPage::WizardPage(SDLFrontEnd* FrontEnd, std::string Name)
{
	this->FrontEnd = FrontEnd;
	this->Name = Name;
	this->Page = NULL;
}

/*virtual*/ WizardPage::~WizardPage()
{

}

WizardWidgetPage* WizardPage::GetPageLayout()
{
	return this->Page;
}

void WizardPage::SetPageLayout(WizardWidgetPage* PageDescription)
{
	this->Page = PageDescription;
}

void WizardPage::FindFocusableControlsRecursive(WizardWidgetBase* RootWidget)
{
	if (RootWidget == Page)
		this->FocusableControls.clear();

	if (dynamic_cast<WizardWidgetButton*>(RootWidget) != NULL)
	{
		FocusableControls.push_back(RootWidget);
	}

	std::vector<WizardWidgetBase*>::iterator Item;

    for(Item = RootWidget->Children.begin(); Item != RootWidget->Children.end(); Item++)
		FindFocusableControlsRecursive(*Item);
}

/*virtual*/ void WizardPage::DoNextFocusItem()
{
	if(FocusControl != FocusableControls.end())
		FocusControl ++;
	else
		FocusControl = FocusableControls.begin();
}

/*virtual*/ void WizardPage::DoPreviousFocusItem()
{
	if(FocusControl != FocusableControls.begin())
		FocusControl ++;
	else
		FocusControl = FocusableControls.end();
}

/*virtual*/ void WizardPage::DoIncreaseSetting()
{

}

/*virtual*/ void WizardPage::DoDecreaseSetting()
{

}

/*virtual*/ int WizardPage::DoApplySetting(SettingsDictionary* Dictionary)
{
	if(Dictionary == NULL)
		return -1;
	return 0;
}

/*virtual*/ void WizardPage::DoCancelSetting()
{

}

void WizardPage::FindFocusableControls()
{
	FindFocusableControlsRecursive(Page);
	FocusControl = FocusableControls.begin();
}

void WizardPage::SetWizard(Wizard* MainWizard)
{
	this->MainWizard = MainWizard;
	if(this->MainWizard != NULL)
	{
		this->DefaultSetup(MainWizard->AVWizardOptions->GetDictionary());
	}
}

/*virtual*/ void WizardPage::DefaultSetup(SettingsDictionary* AVWizardSettings)
{

}
