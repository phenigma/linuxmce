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
#ifndef GL2DEffectFadesFromUnderneath_H
#define GL2DEffectFadesFromUnderneath_H

#include "../GL2DEffects/gl2deffect.h"
#include "../GL2DWidgets/basicwindow.h"
#include "../../DesignObj_Orbiter.h"
#include <memory>

namespace DCE {

class GL2DEffectFadesFromUnderneath : public GL2DEffect
{
	TBasicWindow* Background;
	TBasicWindow* Destination;
	
	FloatRect ButtonSize;
	FloatRect FullScreen;
	
public:
	GL2DEffectFadesFromUnderneath (GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect);
	virtual ~GL2DEffectFadesFromUnderneath();
	
/**
 * It copy the SourceFrame as button texture, DestFrame as destination image
 * @param SourceFrame 
 * @param DestFrame 
 */
	void Configure(PlutoRectangle* EffectSourceSize);
	
	virtual void Paint(int Now);	
	
};

}

#endif
