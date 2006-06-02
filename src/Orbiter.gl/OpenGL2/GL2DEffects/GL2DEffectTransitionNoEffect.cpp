//
// C++ Implementation: GL2DEffectTransitionNoEffect
//
// Description: 
//
//
// Author: Ciprian Mustiata,,, <ciplogic@dcerouter>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "GL2DEffectTransitionNoEffect.h"
#include "gl2deffectfactory.h"

GL2DEffectTransitionNoEffect::GL2DEffectTransitionNoEffect(GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect)
	: GL2DEffectTransit(EffectsEngine, TimeForCompleteEffect)
{
	Configured = false;
}


GL2DEffectTransitionNoEffect::~GL2DEffectTransitionNoEffect()
{

}

/**
 * Resets the time to be valid
 */
void GL2DEffectTransitionNoEffect::Paint(int Now)
{
	GL2DEffectTransit::Paint();
	if(!Configured) {
		this->Length = -1;
		Configured = true;
	}
	Effects->UpdateEffects();
}

/**
 *  No code... the efect really doesn't do nothing 
 */
void GL2DEffectTransitionNoEffect::Configure(PlutoRectangle* EffectSourceSize)
{
}
