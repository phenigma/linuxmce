#include "GL2DEffectLayer.h"

namespace GLEffect2D 
{

GL2DEffectLayer::GL2DEffectLayer(int Width, int Height)
{
	this->Widgets = new DrawingWidgetsEngine(Width, Height);
	this->Effects = new GL2DEffectFactory(this->Widgets);
}

GL2DEffectLayer::~GL2DEffectLayer(void)
{
	delete Effects;
	delete Widgets;
}

/*virtual*/ void GL2DEffectLayer::Paint()
{
	if (!Effects->HasEffects())
		return;
	Effects->UpdateEffects();
	MeshTransform Transform;
	Effects->Paint(Transform);
}


GL2DEffect* GL2DEffectLayer::CreateEffect(int IDEffect, int TimeForComplete)
{
	if(Effects == NULL)
		return NULL;
	return Effects->CreateEffect(IDEffect, TimeForComplete);
}

TBaseWidget* GL2DEffectLayer::CreateWidget(int WidgetType, int Top, int Left, int Width, int Height, char* Text)
{
	if (!Widgets)
		return NULL;
	return Widgets->CreateWidget(WidgetType, Top, Left, Width, Height, Text);
}

bool GL2DEffectLayer::HasEffects()
{
	return Effects->HasEffects();
}

}
