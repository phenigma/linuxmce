#include "GL2DEffectNoEffect.h"

#include "gl2deffectfactory.h"

//#include "../Orbiter3DCommons.h"

namespace GLEffect2D
{

GL2DEffectNoEffect::GL2DEffectNoEffect(EffectFactory * EffectsEngine, int StartAfter, 
									   int TimeForCompleteEffect)
	: Effect(EffectsEngine, StartAfter, TimeForCompleteEffect)
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

}
