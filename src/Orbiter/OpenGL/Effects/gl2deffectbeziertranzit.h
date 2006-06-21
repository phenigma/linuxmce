#ifndef GL2D_BEZIER_TRANZIT_H
#define GL2D_BEZIER_TRANZIT_H

#include "gl2deffecttransit.h"
#include "../Widgets/bezierwindow.h"
#include "../Widgets/basicwindow.h"
#include <memory>

namespace GLEffect2D
{

class GL2DBezierEffectTransit : public GL2DEffectTransit
{
	TBezierWindow* Button;
	
	FloatRect ButtonSize;
	FloatRect FullScreen;
	
public:
	GL2DBezierEffectTransit (EffectFactory * EffectsEngine, int StartAfter, int TimeForCompleteEffect);
	virtual ~GL2DBezierEffectTransit();
	
	void Configure(PlutoRectangle* ButtonSourceSize);
	
	virtual void Paint(int Now);
	
};

}

#endif
