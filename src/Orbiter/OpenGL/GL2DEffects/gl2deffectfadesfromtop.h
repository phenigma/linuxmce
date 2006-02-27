//
// C++ Interface: gl2deffecttransit
//
// Description: 
//
//
// Author: ,,, <opengl@edgar>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GL2DEffectFadesFromTop_H
#define GL2DEffectFadesFromTop_H

#include "../GL2DEffects/gl2deffect.h"
#include "../GL2DWidgets/basicwindow.h"
#include "../../DesignObj_Orbiter.h"
#include <memory>

namespace DCE {

class GL2DEffectFadesFromTop : public GL2DEffect
{
	TBasicWindow* Background;
	TBasicWindow* Destination;

	auto_ptr<PlutoGraphic> BackSrf;
	auto_ptr<PlutoGraphic> FrontSrf;
	
	OpenGLTexture IDBack, IDFront;
	
	FloatRect ButtonSize;
	FloatRect FullScreen;
	
public:
	GL2DEffectFadesFromTop (GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect);
	virtual ~GL2DEffectFadesFromTop();
	
	void Configure(PlutoGraphic *SourceFrame, PlutoGraphic* DestFrame);
	
	virtual void Paint(int Now);	
	
};

}

#endif
