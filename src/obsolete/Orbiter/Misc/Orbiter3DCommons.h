#ifndef ORBITER3D_COMMONS_H
#define ORBITER3D_COMMONS_H

/**
 * That class keeps the common objects and may be accessed easily 
 * from different classes as widgets or effects 
 */

#include "Widgets/basicwindow.h"

//--------------------------------------------------------------------------------------------------------------
#include "../PlutoGraphic.h"
#include "../PlutoUtils/MultiThreadIncludes.h"
#include "../PlutoUtils/singleton.h"
#include "../CallBackTypes.h"
using namespace cpp;
using namespace cpp::Threading;
//--------------------------------------------------------------------------------------------------------------
namespace GLEffect2D
{
class EffectFactory;
class DrawingWidgetsEngine;
}
class Orbiter3DCommons
{
	//Cached members
	TBasicWindow* Screen3D;
	TBasicWindow* HighLight;
	TBasicWindow* Selected;
	TBasicWindow* MouseCursor;

	GLEffect2D::EffectFactory* Effects;
	GLEffect2D::DrawingWidgetsEngine * Widgets;
	OpenGLTexture MouseTexture;

public:
	Orbiter3DCommons();
	void BuildCommons(
		GLEffect2D::EffectFactory* Effects, 
		TBasicWindow* Screen3D, 
		TBasicWindow* HighLight, 
		TBasicWindow* Selected,
		TBasicWindow* MouseCursor);

	void SetNewScreen(OpenGLGraphic NewScreen);
	void SetOldScreen(OpenGLGraphic OldScreen);

	OpenGLTexture GetNewScreen();
	OpenGLTexture GetOldScreen();
	TBasicWindow* GetScreen3D();

	TBasicWindow* GetSelected();
	void SetSelectedArea(FloatRect* SelectedArea);

	TBasicWindow* GetHighLight();
	void SetHighLightArea(FloatRect* HighLightedArea);

	virtual ~Orbiter3DCommons(void);
};
//--------------------------------------------------------------------------------------------------------------
typedef Singleton<Orbiter3DCommons, CreateUsingNew<Orbiter3DCommons>, DefaultLifetime, MultiThreaded> Commons3D;
//--------------------------------------------------------------------------------------------------------------


#endif //Orbiter3DCommons_H

