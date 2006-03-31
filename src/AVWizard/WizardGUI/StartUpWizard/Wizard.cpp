#include "Wizard.h"

#include "WM_Event.h"

#ifdef WIN32
#define LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <iostream>

Wizard::Wizard()
	: Quit(false),
	  StatusChange(true)	
{
	Manager.ParseConfig();
	WizardChangeType = WIZARD_CHANGE_RESOLUTION;
	FrontEnd = new SDLFrontEnd();
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

	while(!Quit)
	{
#ifdef DEBUG
		std::cout<<"Event: "<<Count<<std::endl;
#endif
		while (this->FrontEnd->HasEventPending())
		{
#ifdef DEBUG
			Count ++;
			std::cout<<"Event: "<<Count<<std::endl;
#endif
			FrontEnd->TranslateEvent(Event);
			EvaluateEvent(Event);
		}
#ifdef DEBUG
		Count = 0;
#endif
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

void Wizard::TreatEvents()
{
	StatusChange = true;
	switch(Event.Type)
	{
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
		default:
			StatusChange = false;
			return;
	};
}

void Wizard::DoDecreaseAction()
{
	StatusChange = true;
	switch(WizardChangeType) 
	{
	case WIZARD_CHANGE_RESOLUTION:
		Manager.SetupLowerResolution();
		break;
	default:
		Manager.SetupLowerRefresh();
	}
}

void Wizard::DoIncreaseAction()
{
	StatusChange = true;
	switch(WizardChangeType) 
	{
	case WIZARD_CHANGE_RESOLUTION:
		Manager.SetupHigherResolution();
		break;
	default:
		Manager.SetupHigherRefresh();
	}
}

void Wizard::DoChangeActionBefore()
{
	StatusChange = true;
	// the domain is 1..n, we resize it to 0.. n-1
	WizardChangeType -= 1;
	WizardChangeType--;
	WizardChangeType += WIZARD_NO_POSSIBLE_ACTIONS;
	WizardChangeType %= WIZARD_NO_POSSIBLE_ACTIONS;
	// the domain is 0..n-1, we resize it to 1..n
	WizardChangeType += 1;
}

void Wizard::DoChangeActionAfter()
{
	StatusChange = true;
	// the domain is 1..n, we resize it to 0.. n-1
	WizardChangeType -= 1;
	WizardChangeType++;
	WizardChangeType += WIZARD_NO_POSSIBLE_ACTIONS;
	WizardChangeType %= WIZARD_NO_POSSIBLE_ACTIONS;
	// the domain is 0..n-1, we resize it to 1..n
	WizardChangeType += 1;
}

void Wizard::StartSDLVideoMode(int Width, int Height, bool FullScreen)
{
	FrontEnd->StartVideoMode(Width, Height, FullScreen);
}

void Wizard::EvaluateEvent(WM_Event& Event)
{
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
	}
}

void Wizard::PaintStatus()
{
	FrontEnd->PaintBackground();

	TColorDesc Black(0, 0, 0), Yellow (255, 255, 0), White(255, 255, 255);

	char ResolutionOutput[128], RefreshOutput[128], Buffer[128];

	sprintf(ResolutionOutput, "%dx%d", Manager.GetModes()->GetCurrentWidth(), Manager.GetModes()->GetCurrentHeight());
	sprintf(RefreshOutput, "%d Hz", Manager.GetModes()->GetCurrentRefresh());

	int IncrementLabelSelected = WIZARD_SELECTED_LABEL_SIZE+15;
	int IncrementLabelDeselected = WIZARD_DESELECTED_LABEL_SIZE+10;
	
	switch(WizardChangeType) 
	{
	case WIZARD_CHANGE_RESOLUTION:
		sprintf(Buffer, "<%s>", ResolutionOutput);
		sprintf(ResolutionOutput, "%s", Buffer);

		FrontEnd->TextOutput("Screen resolution", 320, 240, 
			WIZARD_SELECTED_LABEL_SIZE, Yellow, Black, 1);
		FrontEnd->TextOutput(ResolutionOutput, 320, 240+IncrementLabelSelected, 
			WIZARD_SELECTED_FONT_SIZE, Yellow, Black, 1);
		FrontEnd->TextOutput("Screen refresh", 320, 370, 
			WIZARD_DESELECTED_LABEL_SIZE, White, Black, 1);
		FrontEnd->TextOutput(RefreshOutput, 320, 370+IncrementLabelDeselected, 
			WIZARD_DESELECTED_FONT_SIZE, White, Black, 1);
		break;
	default:
		sprintf(Buffer, "<%s>", RefreshOutput);
		sprintf(RefreshOutput, "%s", Buffer);

		FrontEnd->TextOutput("Screen resolution", 320, 240, 
			WIZARD_DESELECTED_LABEL_SIZE, White, Black, 1);
		FrontEnd->TextOutput(ResolutionOutput, 320, 240+IncrementLabelDeselected, 
			WIZARD_DESELECTED_FONT_SIZE, White, Black, 1);
		FrontEnd->TextOutput("Screen refresh", 320, 340, 
			WIZARD_SELECTED_LABEL_SIZE, Yellow, Black, 1);
		FrontEnd->TextOutput(RefreshOutput, 320, 340+IncrementLabelSelected, 
			WIZARD_SELECTED_FONT_SIZE, Yellow, Black, 1);
	}

	FrontEnd->Flip();

	StatusChange = false;
}
