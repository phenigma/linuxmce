#ifndef GL2D_EFFECT_NOEFFECT_H
#define GL2D_EFFECT_NOEFFECT_H

#include "gl2deffect.h"
/**
 * Effect that has only the duty to redraw the screen, it has no drawing code
 */

namespace GLEffect2D
{

class GL2DEffectNoEffect : public Effect
{
public:
	GL2DEffectNoEffect(EffectFactory * EffectsEngine, int StartAfter, 
		int TimeForCompleteEffect);
	virtual ~GL2DEffectNoEffect();

	void Configure(PlutoRectangle* EffectSourceSize);

	virtual void Paint(int Now);
};

}

#endif
