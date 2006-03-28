//
// C++ Interface: GL2DEffectTransitionNoEffect
//
// Description: 
//
//
// Author: Ciprian Mustiata,,, <ciplogic@dcerouter>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef GL2DEffect_Transition_NoEffect_H
#define GL2DEffect_Transition_NoEffect_H

#include "gl2deffecttransit.h"
#include "../GL2DWidgets/basicwindow.h"

class GL2DEffectTransitionNoEffect : public GL2DEffectTransit
{
	TBasicWindow* Destination;

	FloatRect ButtonSize;
	FloatRect FullScreen;
	
public:
	GL2DEffectTransitionNoEffect (GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect);
	virtual ~GL2DEffectTransitionNoEffect();
	
	virtual void Configure(PlutoRectangle* EffectSourceSize);
	
	virtual void Paint(int Now);	
	
};


#endif //#ifndef GL2DEffect_Transition_NoEffect_H
