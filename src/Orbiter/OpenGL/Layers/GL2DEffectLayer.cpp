#include "GL2DEffectLayer.h"

#include "../Mesh/MeshPainter.h"

namespace GLEffect2D 
{

Layer::Layer(int Width, int Height)
: RenderFrame(NULL),
RenderGraphic(NULL)
{
	Widgets  = new DrawingWidgetsEngine(Width, Height);
	Effects  = new EffectFactory(this, Widgets);	
}

Layer::~Layer(void)
{
	delete Effects;
	delete Widgets;
}

/*virtual*/ void Layer::Paint()
{
	if (!Effects->HasEffects())
		return;
	Effects->UpdateEffects();
	MeshTransform Transform;
	Effects->Paint(Transform);
}


Effect* Layer::CreateEffect(int IDEffect, int StartAfter, int TimeForComplete)
{
	if(Effects == NULL)
		return NULL;
	return Effects->CreateEffect(IDEffect, StartAfter, TimeForComplete);
}

TBaseWidget* Layer::CreateWidget(int WidgetType, int Top, int Left, int Width, int Height, char* Text)
{
	if (!Widgets)
		return NULL;
	return Widgets->CreateWidget(WidgetType, Top, Left, Width, Height, Text);
}

bool Layer::HasEffects()
{
	return Effects->HasEffects();
}

void Layer::Resize(int Width, int Height)
{
	this->Width = Width;
	this->Height = Height;
}

}
