#ifndef GL2D_BEZIER_TRANZIT_FLOW_SLIDELEFT_REVERSE_H
#define GL2D_BEZIER_TRANZIT_FLOW_SLIDELEFT_REVERSE_H

#include "gl2deffecttransit.h"
#include "../GL2DWidgets/bezierwindow.h"
#include "../GL2DWidgets/basicwindow.h"
#include "../../DesignObj_Orbiter.h"
#include <memory>


class GL2DBezierEffectTransit_Flow_SlideLeft_Reverse : public GL2DEffectTransit
{
	TBezierWindow* ButonTop;
	TBezierWindow* ButonBottom;

	FloatRect ButtonSize;
	FloatRect FullScreen;

public:
	GL2DBezierEffectTransit_Flow_SlideLeft_Reverse (GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect);
	virtual ~GL2DBezierEffectTransit_Flow_SlideLeft_Reverse();

	void Configure(PlutoRectangle* ButtonSourceSize);

	virtual void Paint(int Now);

};

#endif
