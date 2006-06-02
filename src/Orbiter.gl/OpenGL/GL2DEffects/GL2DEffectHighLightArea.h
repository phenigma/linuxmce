#ifndef GL2D_EFFECT_HIGHLIGH_AREA_H
#define GL2D_EFFECT_HIGHLIGH_AREA_H

#include "GL2DEffectHighLight.h"

class GL2DEffectHighLightArea :
	public GL2DEffectHighLight
{
	FloatRect EffectSourceSize;
	TBasicWindow* Button;
public:
	GL2DEffectHighLightArea(GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect);
	virtual ~GL2DEffectHighLightArea(void);

	void Configure(PlutoRectangle* EffectSourceSize);
	void Paint(int Time);
};

#endif
