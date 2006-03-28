#ifndef GL2D_EFFECT_NOEFFECT_H
#define GL2D_EFFECT_NOEFFECT_H

#include "gl2deffect.h"
/**
 * Effect that has only the duty to redraw the screen, it has no drawing code
 */
class GL2DEffectNoEffect : public GL2DEffect
{
public:
	GL2DEffectNoEffect(GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect);
	virtual ~GL2DEffectNoEffect();

	void Configure(PlutoRectangle* EffectSourceSize);

	virtual void Paint(int Now);
};

#endif
