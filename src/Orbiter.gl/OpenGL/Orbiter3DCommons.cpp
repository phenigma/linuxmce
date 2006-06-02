#include "Orbiter3DCommons.h"

#include "GL2DWidgets/DrawingWidgetsEngine.h"
#include "GL2DEffects/gl2deffectfactory.h"


#include <SDL.h>
#include "OpenGLTextureConverter.h"

Orbiter3DCommons::Orbiter3DCommons()
	: Screen3D(NULL),
	  HighLight(NULL),
	  Selected(NULL)
{
}

void Orbiter3DCommons::BuildCommons (TBasicWindow* Screen3D, 
									 TBasicWindow* HighLight, 
									 TBasicWindow* Selected)
{
	this->Screen3D = Screen3D;
	this->HighLight = HighLight;
	this->Selected = Selected;
}

void Orbiter3DCommons::SetNewScreen(OpenGLTexture NewScreen)
{
	//Widgets->NewScreen = NewScreen;
}

void Orbiter3DCommons::SetOldScreen(OpenGLTexture OldScreen)
{
	//Widgets->OldScreen = OldScreen;
}


Orbiter3DCommons::~Orbiter3DCommons(void)
{

}

OpenGLTexture Orbiter3DCommons::GetNewScreen()
{
	//return Widgets->NewScreen;
	return 0;
}

OpenGLTexture Orbiter3DCommons::GetOldScreen()
{
	//return Widgets->OldScreen;
	return 0;
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
#ifdef WIN32
	::ShowCursor(CursorSurface == NULL);
#else
	SDL_ShowCursor(CursorSurface == NULL);
#endif

	if(CursorSurface == NULL)
		return;
}

void Orbiter3DCommons::SetMousePosition(int X, int Y)
{
}
