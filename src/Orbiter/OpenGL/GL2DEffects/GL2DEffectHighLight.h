#ifndef GL2D_EFFECT_HIGHLIGHT_H
#define GL2D_EFFECT_HIGHLIGHT_H

#include "gl2deffect.h"
#include "../GL2DWidgets/basicwindow.h"

class GL2DEffectHighLight :
	public GL2DEffect
{
public:
	GL2DEffectHighLight(GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect);
	virtual ~GL2DEffectHighLight(void);
};

#endif
