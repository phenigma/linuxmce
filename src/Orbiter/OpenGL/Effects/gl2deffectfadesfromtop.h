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

#include "gl2deffecttransit.h"
#include "../Widgets/basicwindow.h"

class GL2DEffectFadesFromTop : public GL2DEffectTransit
{
	TBasicWindow* Destination;

	FloatRect ButtonSize;
	FloatRect FullScreen;
	
public:
	GL2DEffectFadesFromTop (GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect);
	virtual ~GL2DEffectFadesFromTop();
	
	void Configure(PlutoRectangle* EffectSourceSize);
	
	virtual void Paint(int Now);	
	
};

#endif
