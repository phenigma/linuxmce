#include "GL2DEffectNoEffect.h"

#include "gl2deffectfactory.h"

GL2DEffectNoEffect::GL2DEffectNoEffect(GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect)
	: GL2DEffect(EffectsEngine, TimeForCompleteEffect)
{
	Configured = false;
}


GL2DEffectNoEffect::~GL2DEffectNoEffect()
{

}

/**
 * Resets the time to be valid
 */
void GL2DEffectNoEffect::Paint(int Now)
{
	if(!Configured) {
		Start = Effects->MilisecondTimmer();
		this->Length = -1;
		Configured = true;
	}
	Effects->UpdateEffects();
}

/**
 *  No code... the efect really doesn't do nothing 
 */
void GL2DEffectNoEffect::Configure(PlutoRectangle* EffectSourceSize)
{	
}
