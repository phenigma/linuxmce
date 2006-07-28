#include "WizardPageVideoRatio.h"

#include "Wizard.h"

#include "GUIWizardUtils.h"

#include "ConfigureCommons.h"

#include "SkinGenerator.h"

#include <iostream>

WizardPageVideoRatio::WizardPageVideoRatio(SDLFrontEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{
	Selected = NULL;
}

WizardPageVideoRatio::~WizardPageVideoRatio(void)
{
}

/*virtual*/ int WizardPageVideoRatio::DoApplySetting(SettingsDictionary* Dictionary)
{
	if(Dictionary == NULL || Selected == NULL)
		return -1;
	WizardWidgetScrollList* List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("ResolutionScroll"));
	std::string ResolutionValue = List->GetSelectedValue();
	std::cout<<"Selected resolution is: "<< ResolutionValue;

	List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("RefreshScroll"));
	std::string RefreshValue = List->GetSelectedValue();
	std::cout<<"Selected Refresh is: "<<RefreshValue;

	Dictionary->Set("Resolution", ResolutionValue);
	Dictionary->Set("Refresh", RefreshValue);

	return 0;
}

/*virtual*/ void WizardPageVideoRatio::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
	system(SkinGenerator::Instance()->CommandResetResolution.c_str());

	WizardWidgetScrollList* List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("ResolutionScroll"));
	int NoResolutions = Utils::StringToInt32(AVWizardSettings->GetValue("ResolutionCount"));
	//Resulution0 ... ResolutionN-1
	//AspectRatio0...
	//-> Resulution

	//Comenzi: Reset la intrare in pagina
	//La sfarsit: CommandSetResolution Resolution Refresh
	if (NoResolutions == 0)
	{
		List->AddItem("640x480", "640x480");
		List->AddItem("480p", "480p");
		List->AddItem("800x600", "800x600");
		List->AddItem("720p", "720p");
		List->AddItem("1024x768", "1024x768");
		List->AddItem("1280x1024", "1280x1024");
		List->AddItem("1080i", "1080i");
		List->AddItem("1080p", "1080p");
		List->AddItem("1600x1200", "1600x1200");
	}
	else
	{
		std::string ResolutionIdentifier = "Resolution";
		std::string AspectRatioIdentifier = "AspectRatio";
		
		for(int i = 0; i<NoResolutions; i++)
		{
			std::string ItemCaption, ItemValue;
			ItemValue = AVWizardSettings->GetValue(ResolutionIdentifier +
				Utils::Int32ToString(i) );
			ItemCaption = ItemValue;
			ItemCaption = ItemCaption + " (" + AVWizardSettings->GetValue(AspectRatioIdentifier +
				Utils::Int32ToString(i) ) + ")";
			List->AddItem( ItemCaption, ItemValue);
		}
	}
	List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("ResolutionScroll"));
	if(AVWizardSettings->Exists("Resolution"))
	{
		std::string ResolutionValue = AVWizardSettings->GetValue("Resolution");
		List->SetItemIndex(ResolutionValue);
	}
	else
		List->SetItemIndex(0);
	
	List->SetFocus(true);


	List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("RefreshScroll"));
	List->AddItem("50", "50");
	List->AddItem("60", "60");
	List->AddItem("65", "65");
	List->AddItem("70", "70");
	List->AddItem("75", "75");
	List->AddItem("80", "80");
	List->AddItem("85", "85");
	List->SetFocus(false);

	if(AVWizardSettings->Exists("Refresh"))
	{
		std::string RefreshValue = AVWizardSettings->GetValue("Refresh");
		List->SetItemIndex(RefreshValue);
	}
	else
		List->SetItemIndex(0);



	Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("ResolutionScroll"));
}

void WizardPageVideoRatio::DoIncreaseSetting()
{
	Selected->SetFocus(false);
	Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("RefreshScroll"));
	Selected->SetFocus(true);
}

void WizardPageVideoRatio::DoDecreaseSetting()
{
	Selected->SetFocus(false);
	Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("ResolutionScroll"));
	Selected->SetFocus(true);
}

/*virtual*/ void WizardPageVideoRatio::DoNextFocusItem()
{
	Selected->SetItemIndex(Selected->GetItemIndex()+1);
}

/*virtual*/ void WizardPageVideoRatio::DoPreviousFocusItem()
{
	Selected->SetItemIndex(Selected->GetItemIndex()-1);
}
