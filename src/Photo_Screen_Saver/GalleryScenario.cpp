#include "GalleryScenario.h"

#include "GL/gl.h"

#include "Builder.h"
#include "Painter.h"
#include "../DCE/Logger.h"

using namespace DCE;

GalleryScenario::GalleryScenario(int Width, int Height, int FaddingTime, int ZoomTime, string SearchImageFolder)
: StateMachine(NULL)
{
	StateMachine = new GaleryStateMachine();
	this->FaddingTime = FaddingTime;
	this->ZoomTime = ZoomTime;

	Browser = new FileBrowser(SearchImageFolder);
	this->Width = Width;
	this->Height = Height;

	ZoomFactory::Instance()->Setup(Width, Height);

	AfterPicture = new AnimatedPicture(Width, Height);
	BeforePicture = new AnimatedPicture(Width, Height);
}

GalleryScenario::~GalleryScenario(void)
{
	delete StateMachine;
	delete Browser;
}

void GalleryScenario::Update(void)
{
	glEnable(GL_BLEND);
	int Status = StateMachine->GetStatus();
	int CurrentTime = SDL_GetTicks();

	bool Result;
	switch(Status) {
	case STATUS_STARTUP:
		BeforePicture->LoadFromFile(Browser->NextFile());
		AfterPicture->LoadFromFile(Browser->NextFile());


		BeforePicture->SetZoomKind(CurrentTime, 1500);
		AfterPicture->SetZoomKind(CurrentTime, 5400);

		NewImageStartTime = CurrentTime;

		StateMachine->FadeImages();
		Fades = FadeFactory::Instance()->CreateFadeMode(0, 
			AfterPicture->GetFrame(), BeforePicture->GetFrame(), 
			SDL_GetTicks(),
			FaddingTime*5);

		break;
	case STATUS_FADE_IMAGES:
		Result = Fades->Update(CurrentTime);

		AfterPicture->Update(CurrentTime);
		BeforePicture->Update(CurrentTime);

		AfterPicture->Paint();
		BeforePicture->Paint();

		if (Result)
		{
			g_pPlutoLogger->Write(LV_ALARM, "Start zoom one image");
			StateMachine->AnimateCurrentImage();
		}

		break;
	case STATUS_ANIMATE_CURRENT_IMAGE:
		Result = AfterPicture->Update(CurrentTime);
		AfterPicture->Paint();
		if (Result)
		{
			g_pPlutoLogger->Write(LV_ALARM, "Start fade images");
			AnimatedPicture* Temp = AfterPicture;
			AfterPicture = BeforePicture;
			BeforePicture = Temp;
			AfterPicture->LoadFromFile(Browser->NextFile());
			StateMachine->FadeImages();

			AfterPicture->SetZoomKind(CurrentTime, 5400);
			StateMachine->FadeImages();

			delete Fades;
			Fades = FadeFactory::Instance()->CreateFadeMode(0, 
				AfterPicture->GetFrame(), BeforePicture->GetFrame(), 
				SDL_GetTicks(),
				FaddingTime*5);
		}
		break;
	default:;
	}
}
