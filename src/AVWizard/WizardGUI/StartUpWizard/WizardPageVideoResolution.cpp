#include "WizardPageVideoResolution.h"

#include "ConfigureCommons.h"

#include "GUIWizardUtils.h"

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

	WizardWidgetListBox* ResListBox = dynamic_cast<WizardWidgetListBox*> 
		(Page->GetChildRecursive("ListBox1"));

	Dictionary->Set("VideoResolution", ResListBox->GetCaption());

	WizardWidgetListBox* RefreshListBox = dynamic_cast<WizardWidgetListBox*> 
		(Page->GetChildRecursive("ListBox2"));

	Dictionary->Set("VideoRefresh", RefreshListBox->GetCaption());

	Dictionary->Set("ResolutionSelected", Selected->GetCaption() == "ListBox1" );

	return 0;
}

/*virtual*/ void WizardPageVideoResolution::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
	WizardWidgetListBox* ResListBox = dynamic_cast<WizardWidgetListBox*> 
		(Page->GetChildRecursive("ListBox1"));

	std::string ResolutionStr = "640x480";
	if(AVWizardSettings->Exists("VideoResolution"))
		ResolutionStr =  AVWizardSettings->GetValue("VideoResolution");
	
	ResListBox->SetCaption(ResolutionStr);

	WizardWidgetListBox* RefreshListBox = dynamic_cast<WizardWidgetListBox*> 
		(Page->GetChildRecursive("ListBox2"));

	std::string RefreshStr = "60Hz";
	if(AVWizardSettings->Exists("VideoRefresh"))
		RefreshStr =  AVWizardSettings->GetValue("VideoRefresh");
	
	RefreshListBox->SetCaption(RefreshStr);


	bool IsSelectedResolutionListBox = Utils::StringToInt32(AVWizardSettings->GetValue("ResolutionSelected"));
	ResListBox->SetFocus(IsSelectedResolutionListBox);
	RefreshListBox->SetFocus(!IsSelectedResolutionListBox);
	if(IsSelectedResolutionListBox)
		Selected = ResListBox;
	else
		Selected = RefreshListBox;
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
