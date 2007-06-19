/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include "WizardPageVideoRatio.h"

#include "Wizard.h"

#include "GUIWizardUtils.h"

#include "ConfigureCommons.h"

#include "SkinGenerator.h"

#include "WizardWidgetImage.h"

#include <iostream>

WizardPageVideoRatio::WizardPageVideoRatio(GenericBackEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{
	Selected = NULL;
	std::cout << "WizardPageVideoRatio constructor" << std::endl;
}

WizardPageVideoRatio::~WizardPageVideoRatio(void)
{
	std::cout << "WizardPageVideoRatio destructor" << std::endl;
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

	List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("VideoConnectorScroll"));
	std::string VideoConnectorValue = List->GetSelectedValue();
	std::cout<<"Selected Video Connector is: "<< VideoConnectorValue;

	Dictionary->Set("Resolution", ResolutionValue);
	Dictionary->Set("Refresh", RefreshValue);
	Dictionary->Set("VideoOutput", VideoConnectorValue);

	system((SkinGenerator::Instance()->CommandChangeResolution + " " + ResolutionValue
		+ " " + RefreshValue + " " + VideoConnectorValue).c_str());
	
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
		List->AddItem("640x480 (4:3)", "640x480");
		List->AddItem("480p (16:9)", "480p");
		List->AddItem("800x600 (4:3)", "800x600");
		List->AddItem("720p (16:9)", "720p");
		List->AddItem("1024x768 (4:3)", "1024x768");
		List->AddItem("1280x800 (custom)", "1280x800");
		List->AddItem("1280x1024 (5:4)", "1280x1024");
		List->AddItem("1080i (16:9)", "1080i");
		List->AddItem("1080p (16:9)", "1080p");
		List->AddItem("1600x1200 (4:3)", "1600x1200");
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
	List->AddItem("50 Hz", "50");
	List->AddItem("60 Hz", "60");
	List->AddItem("65 Hz", "65");
	List->AddItem("70 Hz", "70");
	List->AddItem("75 Hz", "75");
	List->AddItem("80 Hz", "80");
	List->AddItem("85 Hz", "85");
	List->SetFocus(false);

	if(AVWizardSettings->Exists("Refresh"))
	{
		std::string RefreshValue = AVWizardSettings->GetValue("Refresh");
		List->SetItemIndex(RefreshValue);
	}
	else
		List->SetItemIndex(0);

	List = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("VideoConnectorScroll"));
	List->AddItem("VGA", "VGA");
	List->AddItem("DVI", "DVI");
	List->AddItem("Component", "Component");
	List->AddItem("Composite", "Composite");
	List->AddItem("S-Video","S-Video");
	if(AVWizardSettings->Exists("VideoOutput")) {
		std::string VideoConnectorValue = AVWizardSettings->GetValue("VideoOutput");
		List->SetItemIndex(VideoConnectorValue);
	} else {
		List->SetItemIndex(0);
	}
	List->SetFocus(false);

	Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("ResolutionScroll"));
}

void WizardPageVideoRatio::DoIncreaseSetting()
{
	Selected->SetFocus(false);
	if ( Selected ==  dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("RefreshScroll")) ) {
		Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("VideoConnectorScroll")); 
	} else {
		Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("RefreshScroll"));
	}
	Selected->SetFocus(true);
}

void WizardPageVideoRatio::DoDecreaseSetting()
{
	Selected->SetFocus(false);
	if ( Selected != dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("VideoConnectorScroll"))) {
		Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("ResolutionScroll"));
	} else {
		Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("RefreshScroll"));
	}
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

/*virtual*/ void WizardPageVideoRatio::DoClickWidget(WizardWidgetBase *pWidget)
{
	std::vector<WizardWidgetBase*>::iterator Item;
	WizardWidgetScrollList* NewSelectedScrollList = NULL;
	WizardWidgetImage* NewSelectedImage = NULL;
	for (Item = Page->Children.begin(); Item < Page->Children.end(); ++Item)
	{
		if (*Item == pWidget)
		{
			NewSelectedScrollList = dynamic_cast<WizardWidgetScrollList*>(pWidget);
			NewSelectedImage = dynamic_cast<WizardWidgetImage*>(pWidget);

			if (NewSelectedScrollList)
			{
				Selected->SetFocus(false);
				Selected = NewSelectedScrollList;
				Selected->SetFocus(true);
			}
			else if (NewSelectedImage)
			{
				if (NewSelectedImage->GetName() == "ResolutionUp")
				{
					Selected->SetFocus(false);
					Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("ResolutionScroll"));
					Selected->SetFocus(true);

					SDL_Event Event;
					Event.type = SDL_KEYUP;
					Event.key.state = SDL_RELEASED;
					Event.key.keysym.sym = SDLK_UP;
					SDL_PushEvent(&Event);
				}
				else if (NewSelectedImage->GetName() == "ResolutionDown")
				{
					Selected->SetFocus(false);
					Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("ResolutionScroll"));
					Selected->SetFocus(true);

					SDL_Event Event;
					Event.type = SDL_KEYUP;
					Event.key.state = SDL_RELEASED;
					Event.key.keysym.sym = SDLK_DOWN;
					SDL_PushEvent(&Event);
				}
				else if (NewSelectedImage->GetName() == "RefreshUp")
				{
					Selected->SetFocus(false);
					Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("RefreshScroll"));
					Selected->SetFocus(true);

					SDL_Event Event;
					Event.type = SDL_KEYUP;
					Event.key.state = SDL_RELEASED;
					Event.key.keysym.sym = SDLK_UP;
					SDL_PushEvent(&Event);
				}
				else if (NewSelectedImage->GetName() == "RefreshDown")
				{
					Selected->SetFocus(false);
					Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("RefreshScroll"));
					Selected->SetFocus(true);

					SDL_Event Event;
					Event.type = SDL_KEYUP;
					Event.key.state = SDL_RELEASED;
					Event.key.keysym.sym = SDLK_DOWN;
					SDL_PushEvent(&Event);
				} 
				else if (NewSelectedImage->GetName() == "VideoConnectorUp")
				{
					Selected->SetFocus(false);
					Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("VideoConnectorScroll"));
					Selected->SetFocus(true);

					SDL_Event Event;
					Event.type = SDL_KEYUP;
					Event.key.state = SDL_RELEASED;
					Event.key.keysym.sym = SDLK_UP;
					SDL_PushEvent(&Event);
				}
				else if (NewSelectedImage->GetName() == "VideoConnectorDown") 
				{
					Selected->SetFocus(false);
					Selected = dynamic_cast<WizardWidgetScrollList*> (Page->GetChildRecursive("VideoConnectorScroll"));
					Selected->SetFocus(true);

					SDL_Event Event;
					Event.type = SDL_KEYUP;
					Event.key.state = SDL_RELEASED;
					Event.key.keysym.sym = SDLK_DOWN;
					SDL_PushEvent(&Event);
				}
			}
			else if ((*Item)->GetName() == "BtnOK")
			{
				SDL_Event Event;
				Event.type = SDL_KEYUP;
				Event.key.state = SDL_RELEASED;
				Event.key.keysym.sym = SDLK_RETURN;
				SDL_PushEvent(&Event);
			}
		}
	}
}
