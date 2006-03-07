//
// C++ Interface: GL2DEffectSlideFromLeft
//
// Description: 
//
//
// Author: ,,, <opengl@edgar>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GL2DEffectSlideFromLeft_H
#define GL2DEffectSlideFromLeft_H

#include "../GL2DEffects/gl2deffect.h"
#include "../GL2DWidgets/basicwindow.h"
#include "../../DesignObj_Orbiter.h"
#include <memory>

namespace DCE {

class GL2DEffectSlideFromLeft : public GL2DEffect
{
	TBasicWindow* Background;
	TBasicWindow* Destination;

	FloatRect ButtonSize;
	FloatRect FullScreen;
	
public:
	GL2DEffectSlideFromLeft (GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect);
	virtual ~GL2DEffectSlideFromLeft();
	
	void Configure(PlutoRectangle* EffectSourceSize);
	
	virtual void Paint(int Now);	
	
};

}

#endif
