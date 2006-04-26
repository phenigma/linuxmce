#include "WizardPageVideoResolution.h"

#include "ConfigureCommons.h"

WizardPageVideoResolution::WizardPageVideoResolution(SDLFrontEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{
	Selected = NULL;
}

WizardPageVideoResolution::~WizardPageVideoResolution(void)
{
}


/*virtual*/ int WizardPageVideoResolution::DoApplySetting(SettingsDictionary* Dictionary)
{
	if(Dictionary == NULL)
		return -1;
	return 0;
}

/*virtual*/ void WizardPageVideoResolution::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
	std::string ListBoxName = "ListBox1";
	Selected = dynamic_cast<WizardWidgetListBox*> (Page->GetChildRecursive(ListBoxName));
	Selected->SetFocus(true);
}

/*virtual*/ void WizardPageVideoResolution::DoIncreaseSetting()
{
	std::string ListBoxName = Selected->GetCaption();
	if (ListBoxName == "ListBox1")
		system(COMMAND_SET_RESOLUTION_PLUS);
	else
		system(COMMAND_SET_REFRESH_PLUS);

}

/*virtual*/ void WizardPageVideoResolution::DoDecreaseSetting()
{
	std::string ListBoxName = Selected->GetCaption();
	if (ListBoxName == "ListBox1")
		system(COMMAND_SET_RESOLUTION_MINUS);
	else
		system(COMMAND_SET_REFRESH_MINUS);
}

/*virtual*/ void WizardPageVideoResolution::DoNextFocusItem()
{
	Selected->SetFocus(false);
	std::string ListBoxName = "ListBox2";
	Selected = dynamic_cast<WizardWidgetListBox*> (Page->GetChildRecursive(ListBoxName));
	if(Selected == NULL)
		return;
	Selected->SetFocus(true);
}

/*virtual*/ void WizardPageVideoResolution::DoPreviousFocusItem()
{
	Selected->SetFocus(false);
	std::string ListBoxName = "ListBox1";
	Selected = dynamic_cast<WizardWidgetListBox*> (Page->GetChildRecursive(ListBoxName));
	if(Selected == NULL)
		return;
	Selected->SetFocus(true);
}
