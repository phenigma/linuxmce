#ifndef GL2D_BEZIER_TRANZIT_FLOW_SLIDELEFT_H
#define GL2D_BEZIER_TRANZIT_FLOW_SLIDELEFT_H

#include "gl2deffecttransit.h"
#include "../GL2DWidgets/bezierwindow.h"
#include "../GL2DWidgets/basicwindow.h"
#include "../../DesignObj_Orbiter.h"
#include <memory>


class GL2DBezierEffectTransit_Flow_SlideLeft : public GL2DEffectTransit
{
	TBezierWindow* ButonTop;
	TBezierWindow* ButonBottom;

	FloatRect ButtonSize;
	FloatRect FullScreen;

public:
	GL2DBezierEffectTransit_Flow_SlideLeft (GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect);
	virtual ~GL2DBezierEffectTransit_Flow_SlideLeft();

	void Configure(PlutoRectangle* ButtonSourceSize);

	virtual void Paint(int Now);

};

#endif
