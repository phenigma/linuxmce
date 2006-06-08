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

#include "gl2deffecttransit.h"
#include "../Widgets/basicwindow.h"
#include <memory>

class GL2DEffectSlideFromLeft : public GL2DEffectTransit
{
	TBasicWindow* Destination;

	FloatRect ButtonSize;
	FloatRect FullScreen;
	
public:
	GL2DEffectSlideFromLeft (GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect);
	virtual ~GL2DEffectSlideFromLeft();
	
	void Configure(PlutoRectangle* EffectSourceSize);
	
	virtual void Paint(int Time);	
	
};

#endif
