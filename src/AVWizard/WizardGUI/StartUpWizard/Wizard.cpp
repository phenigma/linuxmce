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
#include "GenerateWizardConfigDefaults.h"
#include "WizardPageVideoAdjustSize.h"

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
	: 
	Quit(false),
	StatusChange(true),
	LeftBorder(ARROWS_BORDER),
	TopBorder(ARROWS_BORDER),
	WizardBorder(ARROWS_BORDER)
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

	pthread_mutexattr_t m_MutexAttr;
	pthread_mutexattr_init( &m_MutexAttr );	
	pthread_mutexattr_settype( &m_MutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
	pthread_mutex_init(&SafeMutex, &m_MutexAttr);
	pthread_mutexattr_destroy(&m_MutexAttr);


#ifndef WIN32
	signal(SIGUSR1, signal_handler);
	signal(SIGPIPE, signal_handler);
#endif
}

Wizard::~Wizard()
{
	pthread_mutex_destroy(&SafeMutex);
	Server.Finish = true;
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

	if(AVWizardOptions->GetDictionary()->Exists("WizardBorder"))
	{
		WizardBorder = Utils::StringToInt32( AVWizardOptions->GetDictionary()->
			GetValue("WizardBorder"));
	}

	if(AVWizardOptions->GetDictionary()->Exists("LeftBorder"))
	{
		LeftBorder = Utils::StringToInt32( AVWizardOptions->GetDictionary()->
			GetValue("LeftBorder"));
	}

	if(AVWizardOptions->GetDictionary()->Exists("TopBorder"))
	{
		TopBorder = Utils::StringToInt32( AVWizardOptions->GetDictionary()->
			GetValue("TopBorder"));
	}

	PaintStatus();
	while(!Quit)
	{
		while (!(Quit) && (FrontEnd->HasEventPending()))
		{
			AVWizardOptions->GetDictionary()->Set("CurrentStep", Utils::Int32ToString(CurrentPage));
			AVWizardOptions->SaveToXMLFile(CmdLineParser->ConfigFileDefault);

			FrontEnd->TranslateEvent( Event);
			if(Event.Type)
				EvaluateEvent(Event);
		}

		if(!Quit)
		{
			if(StatusChange)
			{
				SafetyLock Lock(&SafeMutex);
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
	if(CurrentPage == WIZARD_NO_PAGES+1)
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
		//MainPage->GetPageLayout()->Paint();
		//FrontEnd->Flip(LeftBorder, TopBorder, WizardBorder);
		
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
	CreateDialogs();
	StatusChange = true;
}

void Wizard::EvaluateEvent(WM_Event& Event)
{
	SafetyLock Lock(&SafeMutex);

#ifdef DEBUG
	std::cout<<"Key pressed: ";
#endif
	StatusChange = true;
	switch(Event.Type) {
	case WMET_QUIT:
#ifdef DEBUG
		std::cout<<"Quit"<<std::endl;
#endif
		Quit = true;
		return;
	case WMET_LEFT_KEY:
#ifdef DEBUG
		std::cout<<"Left"<<std::endl;
#endif
		DoDecreaseAction();
		return;
	case WMET_RIGHT_KEY:
#ifdef DEBUG
		std::cout<<"Right"<<std::endl;
#endif
		DoIncreaseAction();
		return;
	case WMET_UP_KEY:
#ifdef DEBUG
		std::cout<<"Up"<<std::endl;
#endif
		DoChangeActionBefore();
		return;
	case WMET_DOWN_KEY:
#ifdef DEBUG
		std::cout<<"Down"<<std::endl;
#endif
		DoChangeActionAfter();
		return;
	case WMET_ENTER_KEY:
#ifdef DEBUG
		std::cout<<"Enter"<<std::endl;
#endif
		DoApplyScreen(AVWizardOptions->GetDictionary());
		return;
	case WMET_PLUS_KEY:
#ifdef DEBUG
		std::cout<<"ZoomIn"<<std::endl;
#endif
		ZoomIn();
		return;
	case WMET_MINUS_KEY:
#ifdef DEBUG
		std::cout<<"ZoomOut"<<std::endl;
#endif
		ZoomOut();
		return;
	case WMET_SAVE:
#ifdef DEBUG
		std::cout<<"Save exit code"<<std::endl;
#endif
		AVWizardOptions->GetDictionary()->Set("ExitCode", this->ExitCode);
		AVWizardOptions->SaveToXMLFile(WizardCommandLineParser::GetInstance()->ConfigFileDefault);
		return;
	case WMET_ESCAPE_KEY:
#ifdef DEBUG
		std::cout<<"Escape"<<std::endl;
#endif
		DoCancelScreen();
		return;
	default:
		StatusChange = false;
	}
}

void Wizard::PaintStatus()
{
	StatusChange = false;
	if(MainPage)
		MainPage->GetPageLayout()->Paint();
	FrontEnd->Flip(LeftBorder, TopBorder, WizardBorder);
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
	FrontEnd->Resize(FullScreen);

	// Disable DPMS, screen blanking
	system("/usr/bin/X11/xset -dpms s off");
	system("/usr/bin/X11/xset r off");
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

void Wizard::Resize(bool FullScreen)
{
	int Width = 640;
	if(AVWizardSettings->Exists( "WindowWidth"))
		Width = Utils::StringToInt32(AVWizardSettings->GetValue("WindowWidth"));
	int Height = 480;
	if(AVWizardSettings->Exists( "WindowHeight"))
		Height = Utils::StringToInt32(AVWizardSettings->GetValue("WindowHeight"));

	// if there is the same screen resolution, there is no need of resize
	if (this->Width == Width &&  this->Height == Height)
		return;


	this->Width = Width;
	this->Height = Height;

	FrontEnd->StartVideoMode(Width, Height, FullScreen);	
}

void Wizard::ZoomIn()
{
	WizardPageVideoAdjustSize* ZoomPage = dynamic_cast <WizardPageVideoAdjustSize*> (MainPage);
	if (!ZoomPage)
		return;
	WizardBorder -= BORDER_JUMP;
	if(WizardBorder <= 0)
		WizardBorder = 0;

	LeftBorder-= BORDER_JUMP;
	if(LeftBorder<0)
		LeftBorder = 0;

	TopBorder-= BORDER_JUMP;
	if(TopBorder<0)
		TopBorder = 0;

	if(TopBorder >= 2*WizardBorder)
		TopBorder = 2*WizardBorder;
	if(LeftBorder >= 2*WizardBorder)
		LeftBorder = 2*WizardBorder;
	AVWizardOptions->GetDictionary()->Set("WizardBorder", WizardBorder);
	AVWizardOptions->GetDictionary()->Set("LeftBorder", LeftBorder);
	AVWizardOptions->GetDictionary()->Set("TopBorder", TopBorder);

	ZoomPage->UpdateSelected();	
}

void Wizard::ZoomOut()
{
	WizardPageVideoAdjustSize* ZoomPage = dynamic_cast <WizardPageVideoAdjustSize*> (MainPage);
	if (!ZoomPage)
		return;

	if(WizardBorder*5 >= Width)
		return;
	WizardBorder += BORDER_JUMP;
	LeftBorder+= BORDER_JUMP;
	TopBorder+= BORDER_JUMP;
	AVWizardOptions->GetDictionary()->Set("WizardBorder", WizardBorder);
	AVWizardOptions->GetDictionary()->Set("LeftBorder", LeftBorder);
	AVWizardOptions->GetDictionary()->Set("TopBorder", TopBorder);
	ZoomPage->UpdateSelected();	
}
