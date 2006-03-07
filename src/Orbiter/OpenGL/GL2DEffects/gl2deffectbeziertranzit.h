#ifndef GL2D_BEZIER_TRANZIT_H
#define GL2D_BEZIER_TRANZIT_H

#include "../GL2DEffects/gl2deffect.h"
#include "../GL2DWidgets/bezierwindow.h"
#include "../GL2DWidgets/basicwindow.h"
#include "../../DesignObj_Orbiter.h"
#include <memory>

namespace DCE {

class GL2DBezierEffectTransit : public GL2DEffect
{
	TBezierWindow* Button;	
	TBasicWindow* Destination;
	
	FloatRect ButtonSize;
	FloatRect FullScreen;
	
public:
	GL2DBezierEffectTransit (GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect);
	virtual ~GL2DBezierEffectTransit();
	
	void Configure(PlutoRectangle* ButtonSourceSize);
	
	virtual void Paint(int Now);
	
};

} //namespace DCE

#endif
