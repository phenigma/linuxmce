#include "Orbiter3DCommons.h"

#include "GL2DWidgets/DrawingWidgetsEngine.h"
#include "GL2DEffects/gl2deffectfactory.h"


#include <SDL.h>
#include "OpenGLTextureConverter.h"

Orbiter3DCommons::Orbiter3DCommons()
        : Screen3D(NULL),
          HighLight(NULL),
          Selected(NULL),
          MouseCursor(NULL),
          MouseTexture(0)
{
}

void Orbiter3DCommons::BuildCommons (GL2DEffectFactory* Effects, 
									 TBasicWindow* Screen3D, 
									 TBasicWindow* HighLight, 
									 TBasicWindow* Selected,
									 TBasicWindow* MouseCursor)
{
	this->Screen3D = Screen3D;
	this->HighLight = HighLight;
	this->Selected = Selected;
	this->Effects = Effects;
	this->Widgets = Effects->Widgets;
	this->MouseCursor = MouseCursor;
}

void Orbiter3DCommons::SetNewScreen(OpenGLTexture NewScreen)
{
	Widgets->NewScreen = NewScreen;
}

void Orbiter3DCommons::SetOldScreen(OpenGLTexture OldScreen)
{
	Widgets->OldScreen = OldScreen;
}


Orbiter3DCommons::~Orbiter3DCommons(void)
{

}

OpenGLTexture Orbiter3DCommons::GetNewScreen()
{
	return Widgets->NewScreen;
}

OpenGLTexture Orbiter3DCommons::GetOldScreen()
{
	return Widgets->OldScreen;
}

TBasicWindow* Orbiter3DCommons::GetScreen3D()
{
	return Screen3D;
}

TBasicWindow* Orbiter3DCommons::GetHighLight()
{
	return HighLight;
}

TBasicWindow* Orbiter3DCommons::GetSelected()
{
	return Selected;
}

void Orbiter3DCommons::SetHighLightArea(FloatRect* HighLightedArea)
{
	if (!HighLightedArea)
	{
		FloatRect Empty;
		this->HighLight->SetRectCoordinates(Empty);
		return;
	}

	this->HighLight->SetRectCoordinates(*HighLightedArea);
}

void Orbiter3DCommons::SetSelectedArea(FloatRect* SelectedArea)
{
	if (!SelectedArea)
	{
		FloatRect Empty;
		this->Selected->SetRectCoordinates(Empty);
		return;
	}

	this->Selected->SetRectCoordinates(*SelectedArea);
}

void Orbiter3DCommons::SetMouseCursor(PlutoGraphic* CursorSurface)
{
	if(MouseTexture != 0)
		glDeleteTextures(1, &MouseTexture);

#ifdef WIN32
	::ShowCursor(CursorSurface == NULL);
#else
	SDL_ShowCursor(CursorSurface == NULL);
#endif

	if(CursorSurface == NULL)
		return;
	if(MouseCursor == NULL)
		return;

	MouseTexture = OpenGLTextureConverter::GenerateTexture(CursorSurface);
	MouseCursor->SetTexture(MouseTexture);
}

void Orbiter3DCommons::SetMousePosition(int X, int Y)
{
	if(!MouseCursor)
		return;
	MouseCursor->SetBackgroundColor(1, 1, 1, 1.0/2);
	MouseCursor->SetTexture	(MouseTexture);
	//MouseCursor->SetTexture	(GetNewScreen());
	MouseCursor->SetLeft(X);
	MouseCursor->SetTop(Y);
}
