#ifndef GL2D_EFFECT_HIGHLIGHT_H
#define GL2D_EFFECT_HIGHLIGHT_H

#include "gl2deffect.h"
#include "../Widgets/basicwindow.h"

namespace GLEffect2D
{

class GL2DEffectHighLight :
	public Effect
{
public:
	GL2DEffectHighLight(EffectFactory * EffectsEngine, int StartAfter, int TimeForCompleteEffect);
	virtual ~GL2DEffectHighLight(void);
};

}

#endif
