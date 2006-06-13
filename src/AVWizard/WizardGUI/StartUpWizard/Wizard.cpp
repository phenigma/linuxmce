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

#include "WizardCommandLineParser.h"

#include "SafetyLock.h"

#ifndef WIN32
void signal_handler(int signal)
{
	switch (signal)
	{
		case SIGPIPE:
			std::cout<<"Signal SIGPIPE treated"<<std::endl;
			break;
		case SIGUSR1:
			std::cout<<"Signal SIGUSR1 treated"<<std::endl;
			Wizard::GetInstance()->SetExitWithCode(2);
			break;
	}
}
#endif

Wizard::Wizard()
	: Quit(false),
	  StatusChange(true)	
{
	this->ExitCode = 0;
	AVWizardConfParser ConfigurationParser;
	this->AVWizardOptions = ConfigurationParser.ParseFile();
	this->FrontEnd = NULL;
	this->Width = 640;
	this->Height = 480;

	this->FullScreen = false;
	MainPage = NULL;
	CurrentPage = 1;
	AVWizardOptions->GetDictionary()->SetName("AVSettings");
	AVWizardOptions->GetDictionary()->SetType("Config_file");

#ifndef WIN32
	signal(SIGUSR1, signal_handler);
	signal(SIGPIPE, signal_handler);
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
	WizardCommandLineParser *CmdLineParser = WizardCommandLineParser::GetInstance();

	AVWizardOptions->GetDictionary()->Set("CurrentStep", Utils::Int32ToString(CurrentPage));
	AVWizardOptions->SaveToXMLFile(CmdLineParser->ConfigFileDefault);

	
	if(AVWizardOptions->GetDictionary()->Exists("RemoteAVWizardServerPort"))
	{
		int Port = Utils::StringToInt32( AVWizardOptions->GetDictionary()->GetValue("RemoteAVWizardServerPort"));
		Server.StartServer(Port);
	}

	PaintStatus();
	while(!Quit)
	{
		while (!(Quit) && (FrontEnd->HasEventPending()))
		{
			AVWizardOptions->GetDictionary()->Set("CurrentStep", Utils::Int32ToString(CurrentPage));
			AVWizardOptions->SaveToXMLFile(CmdLineParser->ConfigFileDefault);

			FrontEnd->TranslateEvent( Event);
			EvaluateEvent(Event);
		}

		if(!Quit)
		{
			if(StatusChange)
			{
				PaintStatus();
			}
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
	if(CurrentPage == 10)
	{
		AVWizardOptions->SaveToXMLFile(CmdLineParser->ConfigFileDefault);
		SetExitWithCode(ExitCode);
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
		SetExitWithCode(1);
	}
	CreateDialogs();
	StatusChange = true;
}

void Wizard::EvaluateEvent(WM_Event& Event)
{
	StatusChange = true;
	switch(Event.Type) {
	case WMET_QUIT:
		Quit = true;
		return;
	case WMET_LEFT_KEY:
		DoDecreaseAction();
		return;
	case WMET_RIGHT_KEY:
		DoIncreaseAction();
		return;
	case WMET_UP_KEY:
		DoChangeActionBefore();
		return;
	case WMET_DOWN_KEY:
		DoChangeActionAfter();
		return;
	case WMET_ENTER_KEY:
		DoApplyScreen(AVWizardOptions->GetDictionary());
		return;
	case WMET_SAVE:
		AVWizardOptions->GetDictionary()->Set("ExitCode", this->ExitCode);
		AVWizardOptions->SaveToXMLFile(WizardCommandLineParser::GetInstance()->ConfigFileDefault);
		return;
	case WMET_ESCAPE_KEY:
		DoCancelScreen();
		return;
	}
	StatusChange = false;
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
	if(FrontEnd != NULL)
		return;

	FrontEnd = new SDLFrontEnd();

	SettingsDictionary * Dictionary = AVWizardOptions->GetDictionary();
	if (Dictionary->Exists("WindowWidth"))
		Width = Utils::StringToInt32(Dictionary->GetValue("WindowWidth"));
	if (Dictionary->Exists("WindowHeight"))
		Height = Utils::StringToInt32(Dictionary->GetValue("WindowHeight"));
	FrontEnd->StartVideoMode(Width, Height, FullScreen);
}

void Wizard::GenerateCustomEvent(WM_Event Event)
{
	EvaluateEvent(Event);
}

int Wizard::GetExitCode()
{
	return this->ExitCode;
}

void Wizard::SetExitWithCode(int Code)
{
	AVWizardOptions->GetDictionary()->Set("ExitCode", Code);
	AVWizardOptions->SaveToXMLFile(WizardCommandLineParser::GetInstance()->ConfigFileDefault);

	ExitCode = Code;
	WM_Event Event;
	Event.Quit();
	GenerateCustomEvent(Event);
}

void Wizard::Resize(int Width, int Height, bool FullScreen)
{
	// if there is the same screen resolution, there is no need of resize
	if (this->Width == Width &&  this->Height == Height)
		return; 
	this->Width = Width;
	this->Height = Height;

	FrontEnd->StartVideoMode(Width, Height, FullScreen);	
}
