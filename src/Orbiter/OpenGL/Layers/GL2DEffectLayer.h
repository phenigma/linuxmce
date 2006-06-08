#ifndef GL2DEffectLayer_H_
#define GL2DEffectLayer_H_

#include "../Effects/gl2deffectfactory.h"

namespace GLEffect2D 
{

class GL2DEffectLayer
{
	int Width, Height;
	GL2DEffectFactory* Effects;
	DrawingWidgetsEngine* Widgets;
public:
	GL2DEffectLayer(int Width, int Height);
	virtual ~GL2DEffectLayer(void);

	virtual void Paint();

	GL2DEffect* CreateEffect(int IDEffect, int TimeForComplete);
	TBaseWidget* CreateWidget(int WidgetType, int Top, int Left, int Width, int Height, char* Text);

	bool HasEffects();
	
	void Resize(int Width, int Height);
	

};

}

#endif
