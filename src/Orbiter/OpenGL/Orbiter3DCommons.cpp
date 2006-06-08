#include "Orbiter3DCommons.h"

#include "Widgets/DrawingWidgetsEngine.h"
#include "Effects/gl2deffectfactory.h"
#include "OpenGLGraphic.h"

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

void Orbiter3DCommons::SetNewScreen(OpenGLGraphic NewScreen)
{
	//Widgets->NewScreen = NewScreen;
}

void Orbiter3DCommons::SetOldScreen(OpenGLGraphic OldScreen)
{
	//Widgets->OldScreen = OldScreen;
}

Orbiter3DCommons::~Orbiter3DCommons(void)
{

}

OpenGLTexture Orbiter3DCommons::GetNewScreen()
{
	//todo
	return 0;//return Widgets->NewScreen;
}

OpenGLTexture Orbiter3DCommons::GetOldScreen()
{
	//todo
	return 0;//return Widgets->OldScreen;
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

