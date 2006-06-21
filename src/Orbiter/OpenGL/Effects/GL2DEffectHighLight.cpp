#include "GL2DEffectHighLight.h"

namespace GLEffect2D
{

GL2DEffectHighLight::GL2DEffectHighLight(EffectFactory * EffectsEngine, int StartAfter,
										 int TimeForCompleteEffect)
	: Effect(EffectsEngine, StartAfter, TimeForCompleteEffect)
{
}

GL2DEffectHighLight::~GL2DEffectHighLight()
{
}

}
