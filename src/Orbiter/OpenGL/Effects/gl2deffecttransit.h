#ifndef GL2D_EFFECT_TRANSIT_H
#define GL2D_EFFECT_TRANSIT_H

#include "gl2deffect.h"

namespace GLEffect2D
{

class GL2DEffectTransit : public Effect
{
public:
	GL2DEffectTransit(EffectFactory * EffectsEngine, int StartAfter, 
		int TimeForCompleteEffect);
	virtual ~GL2DEffectTransit();

	virtual void Paint();
};

}

#endif //GL2D_EFFECTTRANSIT_H
