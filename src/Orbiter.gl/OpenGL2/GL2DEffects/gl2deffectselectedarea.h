#ifndef GL2D_EFFECT_SELECT_AREA_H
#define GL2D_EFFECT_SELECT_AREA_H

#include "GL2DEffectHighLight.h"

class GL2DEffectSelectedArea :
	public GL2DEffectHighLight
{
	FloatRect EffectSourceSize;
	TBasicWindow* Button;
public:
	GL2DEffectSelectedArea(GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect);
	virtual ~GL2DEffectSelectedArea();

	void Configure(PlutoRectangle* EffectSourceSize);
	void Paint(int Time);
};

#endif
