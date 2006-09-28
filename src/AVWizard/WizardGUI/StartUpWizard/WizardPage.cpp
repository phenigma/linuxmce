#include "WizardPage.h"

#include "WizardWidgetButton.h"

#include "Wizard.h"
#include "GUIWizardUtils.h"

#include "GenerateWizardConfigDefaults.h"

#include "SkinGenerator.h"

#include "WizardWidgetLabel.h"

WizardPage::WizardPage(GenericBackEnd* FrontEnd, std::string Name)
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
}

/*virtual*/ void WizardPage::DoNextFocusItem()
{
}

/*virtual*/ void WizardPage::DoPreviousFocusItem()
{
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

/*static*/ void WizardPage::TabStatus(WizardPage* Page, int EnableStatus)
{
}
