//
// C++ Interface: GL2DEffectWipeIn
//
// Description: 
//
//
// Author: ,,, <opengl@edgar>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GL2DEffectWipeIn_H_
#define GL2DEffectWipeIn_H_

#include "gl2deffecttransit.h"
#include "../GL2DWidgets/basicwindow.h"
#include "../../DesignObj_Orbiter.h"
#include <memory>

namespace DCE {

class GL2DEffectWipeIn : public GL2DEffectTransit
{
	TBasicWindow* Destination;
	
	FloatRect ButtonSize;
	FloatRect FullScreen;
	
public:
	GL2DEffectWipeIn (GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect);
	virtual ~GL2DEffectWipeIn();
	
	void Configure(PlutoRectangle* ButtonSourceSize);
	
	virtual void Paint(int Now);	
	
};

}

#endif
