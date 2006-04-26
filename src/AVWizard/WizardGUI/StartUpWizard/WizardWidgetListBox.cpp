//---------------------------------------------------------------------------
#include "WizardWidgetListBox.h"
//---------------------------------------------------------------------------

WizardWidgetListBox::WizardWidgetListBox(SDLFrontEnd* FrontEnd, std::string Name)
	: WizardWidgetButton(FrontEnd, Name)
{
	
}

WizardWidgetListBox::~WizardWidgetListBox()
{

}

/*virtual*/ void WizardWidgetListBox::Paint()
{
	WizardWidgetButton::Paint();
	WizardWidgetPage* Page = WizardWidgetButton::GetContainerPage();
	SDL_Surface* LeftArrow, *RightArrow;

	if(!Focused)
	{
		LeftArrow = Page->GetSurface(SURFACE_ARROW_LEFT);
		RightArrow = Page->GetSurface(SURFACE_ARROW_RIGHT);
	}
	else
	{
		LeftArrow = Page->GetSurface(SURFACE_HIGH_ARROW_LEFT);
		RightArrow = Page->GetSurface(SURFACE_HIGH_ARROW_RIGHT);
	}

	if(LeftArrow == NULL)
		return;
	if(RightArrow == NULL)
		return;

	SDL_Rect Src, Dest;
	//Left button area
	Src.x = 0; Src.y = 0; Src.w = RightArrow->w; Src.h = RightArrow->h;
	Dest.x = Left - Width/2 - Src.w - 15; Dest.y = Top - Page->GetFontHeight(); Dest.w = Src.w; Dest.h = Src.h; 
	FrontEnd->Blit(LeftArrow, Src, Dest);

	//Right button area
	Src.x = 0; Src.y = 0; Src.w = RightArrow->w; Src.h = RightArrow->h;
	Dest.x = Left + Width/2 + 15; Dest.y = Top - Page->GetFontHeight(); Dest.w = Src.w; Dest.h = Src.h; 
	FrontEnd->Blit(RightArrow, Src, Dest);
}


/*virtual*/ void WizardWidgetListBox::SetUpAttribute(std::string Attribute, std::string DataValue)
{
	WizardWidgetButton::SetUpAttribute(Attribute, DataValue);
}

/*virtual*/ void WizardWidgetListBox::SetFocus(bool IsFocused)
{
	WizardWidgetButton::SetFocus(IsFocused);
}


/*virtual*/ void WizardWidgetListBox::DisplayArrows(bool Left, bool Right)
{
	this->DisplayLeftArrow = Left;
	this->DisplayRightArrow = Right;
}
