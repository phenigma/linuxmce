#include "Wizard.h"

#include "WM_Event.h"

#ifdef WIN32
#define LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "WizardCommandLineParser.h"
#include "AVWizardConfParser.h"

#include "WizardPagesFactory.h"

#include <iostream>

#include "GUIWizardUtils.h"

#ifndef WIN32
void signal_handler(int signal)
{
	switch (signal)
	{
		case SIGUSR1:
			exit(2);
			break;
	}
}
#endif

Wizard::Wizard()
	: Quit(false),
	  StatusChange(true)	
{
	AVWizardConfParser ConfigurationParser;
	this->AVWizardOptions = ConfigurationParser.ParseFile();
	FrontEnd = new SDLFrontEnd();
	this->Width = 640;
	this->Height = 480;

	this->FullScreen = false;
	MainPage = NULL;
	CurrentPage = 1;
	AVWizardOptions->GetDictionary()->SetName("AVSettings");
	AVWizardOptions->GetDictionary()->SetType("Config_file");

#ifndef WIN32
	signal(SIGUSR1, signal_handler);
#endif
}

Wizard::~Wizard()
{
	delete FrontEnd;
	FrontEnd = NULL;
}

//#define DEBUG
void Wizard::MainLoop()
{
#ifdef DEBUG
	int Count = 0;
	std::cout<<"Event: "<<Count<<std::endl;
#endif
	WizardCommandLineParser *CmdLineParser = WizardCommandLineParser::GetInstance();

	AVWizardOptions->GetDictionary()->Set("CurrentStep", Utils::Int32ToString(CurrentPage));
	AVWizardOptions->SaveToXMLFile(CmdLineParser->ConfigFileDefault);

	while(!Quit)
	{
#ifdef DEBUG
		std::cout<<"Event: "<<Count<<std::endl;
#endif
		while (!(Quit) && (FrontEnd->HasEventPending()))
		{
#ifdef DEBUG
			Count ++;
			std::cout<<"Event: "<<Count<<std::endl;
#endif
			AVWizardOptions->GetDictionary()->Set("CurrentStep", Utils::Int32ToString(CurrentPage));
			AVWizardOptions->SaveToXMLFile(CmdLineParser->ConfigFileDefault);

			FrontEnd->TranslateEvent(Event);
			EvaluateEvent(Event);
		}
#ifdef DEBUG
		Count = 0;
#endif
		if(!Quit)
		{
			if(StatusChange)
				PaintStatus();
			else
			{
			#ifdef WIN32
				Sleep(10);
			#else
				usleep(10000);
			#endif
			}
		}
	}
	CmdLineParser->CleanUp();
}

void Wizard::DoDecreaseAction()
{
	StatusChange = true;
	MainPage->DoDecreaseSetting();
}

void Wizard::DoIncreaseAction()
{
	StatusChange = true;
	MainPage->DoIncreaseSetting();
}

void Wizard::DoChangeActionBefore()
{
	MainPage->DoPreviousFocusItem();
	StatusChange = true;
}

void Wizard::DoChangeActionAfter()
{
	MainPage->DoNextFocusItem();
	StatusChange = true;
}

void Wizard::DoApplyScreen(SettingsDictionary* Settings)
{
	WizardCommandLineParser *CmdLineParser = WizardCommandLineParser::GetInstance();

	if(MainPage == NULL)
		return;
	AVWizardOptions->LoadFromXMLFile(CmdLineParser->ConfigFileDefault);
	MainPage->DoApplySetting(Settings);
	delete MainPage;
	MainPage = NULL;
	CurrentPage ++ ;
	if(CurrentPage == 9)
	{
		AVWizardOptions->SaveToXMLFile(CmdLineParser->ConfigFileDefault);
		WM_Event Event;
		Event.Quit();
		GenerateCustomEvent(Event);
	}
	CreateDialogs();
	if(MainPage == NULL)
	{
#ifdef DEBUG
		std::cout<<"Warning, XML asociated page doesn't exist!"<<std::endl;
#endif
	}
	else
	{
		MainPage->GetPageLayout()->Paint();
		FrontEnd->Flip();
	}	
}

void Wizard::DoCancelScreen()
{
	if(MainPage == NULL)
		return;
	MainPage->DoCancelSetting();
	delete MainPage;
	MainPage = NULL;
	CurrentPage -- ;
	if(CurrentPage == 0)
	{
		WM_Event Event;
		Event.Quit();
		GenerateCustomEvent(Event);
	}
	CreateDialogs();
	if(MainPage == NULL)
	{
#ifdef DEBUG
		std::cout<<"Warning, XML asociated page doesn't exist!"<<std::endl;
#endif
	}
	else
	{
		//MainPage->GetPageLayout()->Paint();
		//FrontEnd->Flip();
	}	
}

void Wizard::EvaluateEvent(WM_Event& Event)
{
	StatusChange = true;
	switch(Event.Type) {
	case WMET_QUIT:
		Quit = true;
		break;
	case WMET_LEFT_KEY:
		DoDecreaseAction();
		break;
	case WMET_RIGHT_KEY:
		DoIncreaseAction();
		break;
	case WMET_UP_KEY:
		DoChangeActionBefore();
		break;
	case WMET_DOWN_KEY:
		DoChangeActionAfter();
		break;
	case WMET_ENTER_KEY:
		DoApplyScreen(AVWizardOptions->GetDictionary());
		break;
	case WMET_ESCAPE_KEY:
		DoCancelScreen();	
	}
}

void Wizard::PaintStatus()
{
	if(MainPage)
		MainPage->GetPageLayout()->Paint();
	FrontEnd->Flip();

	StatusChange = false;
}

int Wizard::ParseCommandLineParameters(int argc, char** argv)
{
	WizardCommandLineParser *CmdLineParser = WizardCommandLineParser::GetInstance();

	std::vector<std::string> Arguments = CmdLineParser->CommandLineSplit(argc, argv);
	CmdLineParser->ParseArguments(Arguments, *(AVWizardOptions->GetDictionary()));

	if (CmdLineParser->NeedQuit)
	{
		AVWizardOptions->SaveToXMLFile(CmdLineParser->ConfigFileDefault);
		exit(0);
	}

	if(CmdLineParser->StartStep != -1)
	{
		CurrentPage = CmdLineParser->StartStep;
	}

	CmdLineParser->CleanUp();
	CmdLineParser = NULL;


	return 0;
}

void Wizard::CreateDialogs()
{
	WizardPagesFactory* Factory = WizardPagesFactory::GetInstance();

	Factory->SetSDLFrontEnd(FrontEnd);

	if(MainPage!= NULL)
	{
		delete MainPage;
		MainPage = NULL;
	}

	MainPage = Factory->CreatePredefinedWizardPage(CurrentPage);
	if(MainPage)
		MainPage->SetWizard(this);
	Factory->CleanUp();
}

/*static*/ Wizard* Wizard::Instance = NULL;

/*static*/ Wizard* Wizard::GetInstance()
{
	if(Instance == NULL)
		Instance = new Wizard();
	return Instance;
}

void Wizard::CleanUp()
{
	if(Instance != NULL)
	{
		delete Instance;
		Instance = NULL;
	}
}

void Wizard::StartSDLVideoMode()
{
	FrontEnd->StartVideoMode(Width, Height, FullScreen);
}

void Wizard::GenerateCustomEvent(WM_Event Event)
{
	EvaluateEvent(Event);
}
