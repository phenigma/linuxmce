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
//
// C++ Implementation: WizardPageWelcome
//
// Description: 
//
//
// Author: Ciprian Mustiata <ciprian.m@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "WizardPageWelcome.h"
#include "Wizard.h"

#include <iostream>

WizardPageWelcome::WizardPageWelcome(GenericBackEnd* FrontEnd, std::string Name)
	: WizardPage(FrontEnd, Name)
{
#ifdef DEBUG
		std::cout<<"WizardPageWelcome::WizardPageWelcome"<<std::endl;
#endif
		system("beep -l 100 -f 1000 -n -l 100 -f 1500 -n -l 100 -f 2000");
}

/*virtual*/ WizardPageWelcome::~WizardPageWelcome()
{

}

/*virtual*/ void WizardPageWelcome::DefaultSetup(SettingsDictionary* AVWizardSettings)
{
#ifdef DEBUG
		std::cout<<"WizardPageWelcome::DefaultSetup"<<std::endl;
#endif
	WizardWidgetButton* Btn = dynamic_cast<WizardWidgetButton*>
		(Page->GetChildRecursive("MainBtn"));
	if(Btn == NULL)
		return;
	Btn->SetFocus(true);
}

/*virtual*/ int WizardPageWelcome::DoApplySetting(SettingsDictionary* Dictionary)
{
#ifdef DEBUG
		std::cout<<"WizardPageWelcome::DoApplySetting"<<std::endl;
#endif
	return 0;
}

/*virtual*/ void WizardPageWelcome::DoCancelSetting()
{
#ifdef DEBUG
		std::cout<<"WizardPageWelcome::DoCancelSetting"<<std::endl;
#endif
	Wizard::GetInstance()->SetExitWithCode(3);
}

/*virtual*/ void WizardPageWelcome::DoNumberKey(int KeyCode)
{
	const string CommandXConfigure = "/usr/pluto/bin/AVWizard_BlindVideoSettings.sh --resolution ";
	string Command = "";

	switch (KeyCode)
	{
		case SDLK_0:
			Command = CommandXConfigure + "640x480 --output DVI";
			break;
		case SDLK_1:
			Command = CommandXConfigure + "640x480 --output VGA";
			break;
		case SDLK_2:
			Command = CommandXConfigure + "640x480 --output Component";
			break;
		case SDLK_3:
			Command = CommandXConfigure + "640x480 --output Composite";
			break;
		case SDLK_4:
			Command = CommandXConfigure + "640x480 --output S-Video";
			break;
		case SDLK_5:
			Command = CommandXConfigure + "640x480";
			break;
		case SDLK_6:
			Command = CommandXConfigure + "1024x768";
			break;
		case SDLK_7:
			Command = CommandXConfigure + "720p";
			break;
		case SDLK_8:
			Command = CommandXConfigure + "1080i";
			break;
		case SDLK_9:
			Command = CommandXConfigure + "1080p";
			break;
		default:
			/* NOOP */
			break;
	}

	if (Command != "")
	{
		printf("Running command: %s\n", Command.c_str());
		system(Command.c_str());
		kill(getpid(), SIGUSR1);
	}
}

/*virtual*/ void WizardPageWelcome::DoClickWidget(WizardWidgetBase * pWidget)
{
#ifdef DEBUG
	std::cout << "WizardPageWelcome::DoClickWidget" << endl;
#endif
	if (pWidget == Page->GetChildRecursive("MainBtn"))
	{
		SDL_Event Event;
		Event.type = SDL_KEYUP;
		Event.key.state = SDL_RELEASED;
		Event.key.keysym.sym = SDLK_RETURN;
		SDL_PushEvent(&Event);
	}
}
