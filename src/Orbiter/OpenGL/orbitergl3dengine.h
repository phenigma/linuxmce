//
// C++ Interface: orbitergl3dengine 
//
// Description: 
// That class implements the default events of the OpenGL drawing thread
//
// Author: ciplogic <ciprian.m@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GL3D_ENGINE_H
#define GL3D_ENGINE_H

#include "math3dutils.h"
#include "GL2DWidgets/basicwindow.h"
#include "GL2DWidgets/DrawingWidgetsEngine.h"
/**
That class creates a default decorator for OpenGL operations thread.

@author ciplogic
*/

#include "GL2DEffects/gl2deffect.h"
#include "GL2DEffects/gl2deffectfactory.h"

class OrbiterGL3D {
public: 
	OrbiterRenderer * pOrbiterRendererGL;
	TBasicWindow * Screen3D;
	TBasicWindow * HighLighArea;
	TBasicWindow * SelectedArea;
	TBasicWindow * MouseCursor;

#ifdef POCKETFROG
	HDC hdc;
#endif 

public:
	DrawingWidgetsEngine * Widgets;
	GL2DEffectFactory* EffectBuilder;
	FloatRect FullScreenSize;
	
	int BuildOrbiterGL(OrbiterRenderer * pOrbiterRendererGL);
	
	~OrbiterGL3D ();

	/**
	 * That method paints the frame based of current time in OpenGL
	 */
	void Paint();
	
	/**
	* Show the result of the animation
	*/
	void Flip();

	/**
	 * On pocket-frog or any OpenGL enabled on different context should be used BeginAnimation - EndAnimation
	 */
	void BeginAnimation();
	void EndAnimation();

	/**
	 * Because is a singleton class, that method gets the instance 
	 */
	static OrbiterGL3D* GetInstance();

private:
	/**
	*    Default constructor, creates one OpenGL 3D engine over one Orbiter
	* @param pOrbiterGL The Orbiter associated to the OpenGL
	*/
	OrbiterGL3D ();

	/**
	 * Set up defaults parameters on OpenGL
	 */
	int InitOpenGL();
	/**
	 * clean up code
	 */
	void ReleaseOpenGL();
	/**
	 * Instance of the singleton class
	 */
	static OrbiterGL3D* Instance;
};

#endif
//GL3DEngine
