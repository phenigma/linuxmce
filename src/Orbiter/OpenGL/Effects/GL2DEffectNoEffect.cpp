#include "GL2DEffectNoEffect.h"

#include "gl2deffectfactory.h"

//#include "../Orbiter3DCommons.h"

GL2DEffectNoEffect::GL2DEffectNoEffect(GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect)
	: GL2DEffect(EffectsEngine, TimeForCompleteEffect)
{
	Configured = false;
}


GL2DEffectNoEffect::~GL2DEffectNoEffect()
{
	//Commons3D::Instance().GetScreen3D()->SetBackgroundImage(Effects->Widgets->NewScreen);
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
}

/**
 *  No code... the efect really doesn't do nothing 
 */
void GL2DEffectNoEffect::Configure(PlutoRectangle* EffectSourceSize)
{
	Configured = true;
}
