#ifndef GL2D_EFFECT_TRANSIT_H
#define GL2D_EFFECT_TRANSIT_H

#include "gl2deffect.h"

class GL2DEffectTransit : public GL2DEffect
{
public:
	GL2DEffectTransit(GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect);
	virtual ~GL2DEffectTransit();

	virtual void Paint();
};

#endif //GL2D_EFFECTTRANSIT_H
