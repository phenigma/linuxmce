#ifndef ORBITER3D_COMMONS_H
#define ORBITER3D_COMMONS_H

/**
 * That class keeps the common objects and may be accessed easily 
 * from different classes as widgets or effects 
 */

#include "GL2DWidgets/basicwindow.h"


class GL2DEffectFactory;
class DrawingWidgetsEngine;

class Orbiter3DCommons
{
	//Cached members
	TBasicWindow* Screen3D;
	TBasicWindow* HighLight;
	TBasicWindow* Selected;

	GL2DEffectFactory* Effects;
	DrawingWidgetsEngine * Widgets;
	static Orbiter3DCommons* Instance;


	Orbiter3DCommons();
public:
	void BuildCommons(GL2DEffectFactory* Effects, TBasicWindow* Screen3D, TBasicWindow* HighLight, TBasicWindow* Selected);

	void SetNewScreen(OpenGLTexture NewScreen);
	void SetOldScreen(OpenGLTexture OldScreen);

	OpenGLTexture GetNewScreen();
	OpenGLTexture GetOldScreen();
	TBasicWindow* GetScreen3D();

	TBasicWindow* GetSelected();
	void SetSelectedArea(FloatRect* SelectedArea);

	TBasicWindow* GetHighLight();
	void SetHighLightArea(FloatRect* HighLightedArea);

	virtual ~Orbiter3DCommons(void);

	static Orbiter3DCommons* GetInstance();
};

#endif //Orbiter3DCommons_H

