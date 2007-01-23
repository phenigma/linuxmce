#include "GalleryScenario.h"

#include "GL/gl.h"

#include "Builder.h"
#include "Painter.h"
#include "../DCE/Logger.h"

using namespace DCE;

GalleryScenario::GalleryScenario(int Width, int Height, int FaddingTime, 
	int ZoomTime, string SearchImageFolder, bool bUseAnimation)
: StateMachine(NULL), nLastTimeUpdated(0)
{
	StateMachine = new GaleryStateMachine();
	this->FaddingTime = FaddingTime;
	this->ZoomTime = ZoomTime;

	Browser = new FileBrowser(SearchImageFolder);
	this->Width = Width;
	this->Height = Height;
	m_bUseAnimation = bUseAnimation;

	ZoomFactory::Instance()->Setup(Width, Height);

	AfterPicture = new AnimatedPicture(Width, Height);
	BeforePicture = new AnimatedPicture(Width, Height);
}

GalleryScenario::~GalleryScenario(void)
{
	delete StateMachine;
	delete Browser;

	delete AfterPicture;
	delete BeforePicture;

	delete Fades;

	ZoomFactory::Destroy();
	FadeFactory::Destroy();
	TextureManager::Destroy();
}

bool GalleryScenario::Update(void)
{
	glEnable(GL_BLEND);
	int Status = StateMachine->GetStatus();
	int CurrentTime = SDL_GetTicks();

	if(!m_bUseAnimation)
	{
		if(Status == STATUS_STARTUP)
		{
			g_pPlutoLogger->Write(LV_ALARM, "Painting the image...");
			BeforePicture->LoadFromFile(Browser->NextFile());
			BeforePicture->SetZoomKind(CurrentTime, ZoomTime);
			BeforePicture->Update(CurrentTime);
			BeforePicture->Paint();
			StateMachine->AboutToPaintStaticImage();
			return true;
		}

		if(int(SDL_GetTicks()) - nLastTimeUpdated > ZoomTime)
		{
			g_pPlutoLogger->Write(LV_ALARM, "Painting the image...");
			BeforePicture->LoadFromFile(Browser->NextFile());
			BeforePicture->SetZoomKind(CurrentTime, ZoomTime);
			BeforePicture->Update(CurrentTime);
			BeforePicture->Paint();
			nLastTimeUpdated = SDL_GetTicks();
			return true;
		}

		return false;
	}

	bool Result;
	switch(Status) {
	case STATUS_STARTUP:
		BeforePicture->LoadFromFile(Browser->NextFile());
		AfterPicture->LoadFromFile(Browser->NextFile());


		BeforePicture->SetZoomKind(CurrentTime, ZoomTime);
		AfterPicture->SetZoomKind(CurrentTime, ZoomTime);

		NewImageStartTime = CurrentTime;

		StateMachine->FadeImages();
		Fades = FadeFactory::Instance()->CreateFadeMode(0, 
			AfterPicture->GetFrame(), BeforePicture->GetFrame(), 
			SDL_GetTicks(),
			FaddingTime);

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

			AfterPicture->SetZoomKind(CurrentTime, ZoomTime);
			StateMachine->FadeImages();

			delete Fades;
			Fades = FadeFactory::Instance()->CreateFadeMode(0, 
				AfterPicture->GetFrame(), BeforePicture->GetFrame(), 
				SDL_GetTicks(),
				FaddingTime);
		}
		break;
	default:;
	}

	return true;
}

void GalleryScenario::RefreshFileList(string sFileList)
{
	if(NULL != Browser)
		Browser->RefreshFileList(sFileList);
}