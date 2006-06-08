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
#ifndef GL2D_EFFECT_BLENDING_H
#define GL2D_EFFECT_BLENDING_H

#include "gl2deffecttransit.h"
#include "../Widgets/basicwindow.h"
#include <memory>

class GL2DEffectBlending : public GL2DEffectTransit
{
	TBasicWindow* Button;	
	TBasicWindow* Destination;
	
	FloatRect ButtonSize;
	FloatRect FullScreen;
	
public:
	GL2DEffectBlending (GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect);
	virtual ~GL2DEffectBlending();
	
	void Configure(PlutoRectangle* ButtonSourceSize);
	
	virtual void Paint(int Now);	
	
};

#endif
