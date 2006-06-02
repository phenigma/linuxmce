#ifndef ORBITER3D_COMMONS_H
#define ORBITER3D_COMMONS_H

/**
 * That class keeps the common objects and may be accessed easily 
 * from different classes as widgets or effects 
 */

#include "GL2DWidgets/basicwindow.h"

//--------------------------------------------------------------------------------------------------------------
#include "../PlutoUtils/MultiThreadIncludes.h"
#include "../PlutoUtils/singleton.h"
#include "../CallBackTypes.h"
using namespace cpp;
using namespace cpp::Threading;
//--------------------------------------------------------------------------------------------------------------

class GL2DEffectFactory;
class DrawingWidgetsEngine;

class Orbiter3DCommons
{
	//Cached members
	TBasicWindow* Screen3D;
	TBasicWindow* HighLight;
	TBasicWindow* Selected;
	TBasicWindow* MouseCursor;

	GL2DEffectFactory* Effects;
	DrawingWidgetsEngine * Widgets;
	OpenGLTexture MouseTexture;

public:
	Orbiter3DCommons();
	void BuildCommons(
		GL2DEffectFactory* Effects, 
		TBasicWindow* Screen3D, 
		TBasicWindow* HighLight, 
		TBasicWindow* Selected,
		TBasicWindow* MouseCursor);

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

	/**
	 *	That function creates a custom OpenGL cursor
	 *	If Surface == NULL, it clean up the openGL mouse cursor and reneable the mouse	
	 */
	void SetMouseCursor(PlutoGraphic* Surface);
	/**
	 *	Set to OpenGL cursor position
	 */
	void SetMousePosition(int X, int Y);
};
//--------------------------------------------------------------------------------------------------------------
typedef Singleton<Orbiter3DCommons, CreateUsingNew<Orbiter3DCommons>, DefaultLifetime, MultiThreaded> Commons3D;
//--------------------------------------------------------------------------------------------------------------


#endif //Orbiter3DCommons_H

