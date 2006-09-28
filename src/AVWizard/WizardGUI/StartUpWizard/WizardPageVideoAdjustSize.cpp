#include "WizardPageVideoAdjustSize.h"

#include "ConfigureCommons.h"

#include "GUIWizardUtils.h"

#include <iostream>

#include "SettingsDictionaryTree.h"

#include "Wizard.h"

#include "SkinGenerator.h"

WizardPageVideoAdjustSize::WizardPageVideoAdjustSize(GenericBackEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{
	Wizard::GetInstance()->WizardBorder = 0;
	Wizard::GetInstance()->LeftBorder = 0;	
	Wizard::GetInstance()->TopBorder = 0;

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
	UpdateSelected();
}

/*virtual*/ void WizardPageVideoAdjustSize::DoIncreaseSetting()
{
	if(Wizard::GetInstance()->WizardBorder*2 > 
		Wizard::GetInstance()->LeftBorder )
	Wizard::GetInstance()->LeftBorder +=BORDER_JUMP;
	UpdateSelected();
}

/*virtual*/ void WizardPageVideoAdjustSize::DoDecreaseSetting()
{
	if(Wizard::GetInstance()->LeftBorder>BORDER_JUMP)
		Wizard::GetInstance()->LeftBorder -=BORDER_JUMP;
	else
		Wizard::GetInstance()->LeftBorder = 0;
	UpdateSelected();
}

/*virtual*/ void WizardPageVideoAdjustSize::DoNextFocusItem()
{
	if(Wizard::GetInstance()->WizardBorder*2 > 
		Wizard::GetInstance()->TopBorder )
	Wizard::GetInstance()->TopBorder +=BORDER_JUMP;
	UpdateSelected();
}

/*virtual*/ void WizardPageVideoAdjustSize::DoPreviousFocusItem()
{
	if(Wizard::GetInstance()->TopBorder>BORDER_JUMP)
		Wizard::GetInstance()->TopBorder -= BORDER_JUMP;
	else
		Wizard::GetInstance()->TopBorder = 0;
	UpdateSelected();
}

/*virtual*/ void WizardPageVideoAdjustSize::UpdateSelected()
{
	if (Wizard::GetInstance()->WizardBorder < 0)
		Wizard::GetInstance()->WizardBorder = 0;

	int LeftBorder = Wizard::GetInstance()->LeftBorder;
	int TopBorder = Wizard::GetInstance()->TopBorder;
	int Border = Wizard::GetInstance()->WizardBorder;

	WizardWidgetButton* Button = NULL;
	Button = dynamic_cast<WizardWidgetButton*> 
		(Page->GetChildRecursive("ButtonLeft"));
	Button->SetFocus(LeftBorder);

	Button = dynamic_cast<WizardWidgetButton*> 
		(Page->GetChildRecursive("ButtonUp"));
	Button->SetFocus(TopBorder);

	Button = dynamic_cast<WizardWidgetButton*> 
		(Page->GetChildRecursive("ButtonRight"));
	Button->SetFocus(LeftBorder < Border*2);
	
	Button = dynamic_cast<WizardWidgetButton*> 
		(Page->GetChildRecursive("ButtonDown"));
	Button->SetFocus(TopBorder < Border*2);

	Button = dynamic_cast<WizardWidgetButton*> 
		(Page->GetChildRecursive("ButtonPlus"));
	Button->SetFocus(0 < Border);

	Button = dynamic_cast<WizardWidgetButton*> 
		(Page->GetChildRecursive("ButtonMinus"));
	Button->SetFocus(Border*5 < FrontEnd->GetScreenWidth());

	Wizard::GetInstance()->AVWizardOptions->GetDictionary()->Set("WizardBorder", Border);
	Wizard::GetInstance()->AVWizardOptions->GetDictionary()->Set("LeftBorder", LeftBorder);
	Wizard::GetInstance()->AVWizardOptions->GetDictionary()->Set("TopBorder", TopBorder);

#ifdef DEBUG
	cout << "WizardPageVideoAdjustSize::UpdateSelected()" << endl;
	cout << "LeftBorder: " << LeftBorder << endl;
	cout << "TopBorder: " << TopBorder << endl;
	cout << "WizardBorder: " << Border << endl;
#endif
}
