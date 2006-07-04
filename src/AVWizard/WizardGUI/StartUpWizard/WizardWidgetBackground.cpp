//---------------------------------------------------------------------------
#include "WizardWidgetBackground.h"
//---------------------------------------------------------------------------
#include "SDL_image.h"
//---------------------------------------------------------------------------

WizardWidgetBackground::WizardWidgetBackground(SDLFrontEnd* FrontEnd, std::string Name)
	: WizardWidgetBase(FrontEnd, Name)
{
	Surface = NULL;
	Top = 0;
	Left = 0;
	Width = -1;
	Height = -1;
}

WizardWidgetBackground::~WizardWidgetBackground()
{
	if(Surface != NULL)
	{
		SDL_FreeSurface(Surface);
		Surface = NULL;
	}
}

/*virtual*/ void WizardWidgetBackground::Paint()
{
	if(!Surface)
	{
		WizardWidgetBase::Paint();
		return;
	}
	SDL_Rect Src, Dest;
	Src.x = 0; Src.y = 0;
	Src.w = Surface->w;
	Src.h = Surface->h;

	Dest.x = Left;
	Dest.y = Top;
	if (Width == -1)
		Dest.w = Surface->w;
	else
        Dest.w = Width;

	if (Height == -1)
		Dest.h = Surface->h;
	else
		Dest.h = Height;

	//SDL_SaveBMP(Surface, "test.bmp");
	FrontEnd->Blit(Surface, Src, Dest);
	WizardWidgetBase::Paint();
}

void WizardWidgetBackground::SetSurface(std::string FileName)
{
	if(Surface!=NULL)
		SDL_FreeSurface(Surface);

	Surface = IMG_Load(FileName.c_str());
	if(Surface== NULL)
		return;

	Width = Surface->w;
	Height = Surface->h;
}

/*virtual*/ void WizardWidgetBackground::SetUpAttribute(std::string Attribute, std::string DataValue)
{
	if(Attribute == "Picture")
		SetSurface(DataValue);
	else
        WizardWidgetBase::SetUpAttribute(Attribute, DataValue);
}
