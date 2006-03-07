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
#ifndef GL3DEngine_H
#define GL3DEngine_H

#include "math3dutils.h"
#include "GL2DWidgets/basicwindow.h"
#include "GL2DWidgets/DrawingWidgetsEngine.h"

/**
That class creates a default decorator for OpenGL operations thread.

@author ciplogic
*/

#include "GL2DEffects/gl2deffect.h"
#include "GL2DEffects/gl2deffectfactory.h"

namespace DCE {


class OrbiterGL3D {
	Orbiter * pOrbiterGL;
	TBasicWindow * Screen3D;

#ifdef POCKETFROG
	HDC hdc;
#endif 

public:
	DrawingWidgetsEngine * Widgets;
	GL2DEffectFactory* EffectBuilder;
	FloatRect FullScreenSize;
	
	OrbiterGL3D (Orbiter * pOrbiterGL);
	~OrbiterGL3D ();

	/**
	 * That method paints the actual frame
	 */
	void Paint();
	
	void BeginAnimation();
	void EndAnimation();

private:
	void InitOpenGL();
	void ReleaseOpenGL();
	void Flip();
};

}

#endif
//GL3DEngine
