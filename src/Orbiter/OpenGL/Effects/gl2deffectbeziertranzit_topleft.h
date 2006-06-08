#ifndef GL2D_BEZIER_TRANZIT_TOP_LEFT_H
#define GL2D_BEZIER_TRANZIT_TOP_LEFT_H

#include "gl2deffecttransit.h"
#include "../Widgets/bezierwindow.h"
#include <memory>


class GL2DBezierEffectTransit_TopLeft : public GL2DEffectTransit
{
	TBezierWindow* Button;	
	
	FloatRect ButtonSize;
	FloatRect FullScreen;
	
public:
	GL2DBezierEffectTransit_TopLeft (GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect);
	virtual ~GL2DBezierEffectTransit_TopLeft();
	
	void Configure(PlutoRectangle* ButtonSourceSize);
	
	virtual void Paint(int Now);
	
};

#endif
