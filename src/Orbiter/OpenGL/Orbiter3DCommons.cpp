#include "Orbiter3DCommons.h"

#include "GL2DWidgets/DrawingWidgetsEngine.h"
#include "GL2DEffects/gl2deffectfactory.h"

Orbiter3DCommons* Orbiter3DCommons::Instance = NULL;

Orbiter3DCommons::Orbiter3DCommons()
	: Screen3D(NULL),
	  HighLight(NULL),
	  Selected(NULL)
{
	Instance = this;
}

void Orbiter3DCommons::BuildCommons (GL2DEffectFactory* Effects, 
									 TBasicWindow* Screen3D, 
									 TBasicWindow* HighLight, 
									 TBasicWindow* Selected)
{
	this->Screen3D = Screen3D;
	this->HighLight = HighLight;
	this->Selected = Selected;
	this->Effects = Effects;
	this->Effects = Effects;
	this->Widgets = Effects->Widgets;
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


Orbiter3DCommons* Orbiter3DCommons::GetInstance()
{
	if(Instance)
		return Instance;
	
	return new Orbiter3DCommons();
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
