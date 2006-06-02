#include "gl2deffectselectedarea.h"

#include "GL2DEffectHighLightArea.h"

#include "gl2deffectfactory.h"

GL2DEffectSelectedArea::GL2DEffectSelectedArea(GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect)
: GL2DEffectHighLight(EffectsEngine, TimeForCompleteEffect)
{
	Configured = false;
	Button = dynamic_cast<TBasicWindow*> (Effects->Widgets->CreateWidget(
		BASICWINDOW,
		0, 0, 0, 0,
		"None"
		));
}

GL2DEffectSelectedArea::~GL2DEffectSelectedArea()
{
	Effects->Widgets->DeleteWidget(Button);
	Button = NULL;
}


void GL2DEffectSelectedArea::Configure(PlutoRectangle* EffectSourceSize)
{
	if (!EffectSourceSize)
		return;
	this->EffectSourceSize.Left = (float)EffectSourceSize->X;
	this->EffectSourceSize.Top = (float)EffectSourceSize->Y;
	this->EffectSourceSize.Width = (float)EffectSourceSize->Width;
	this->EffectSourceSize.Height = (float)EffectSourceSize->Height;
}

void GL2DEffectSelectedArea::Paint(int Time)
{
	if(!Configured)
	{
		Start = Effects->MilisecondTimmer();
		Configured = true;
		Button->SetRectCoordinates(EffectSourceSize);
		Button->SetVisible(true);
	}
	float Step = Stage(float(Time));
	if (Step <0.5)
	{
		Button->SetBackgroundColor(1.0f, 0.0f, 0.0f, Step);
	}
	else
	{
		Button->SetBackgroundColor(1.0f, 0.0f, 0.0f, 0.5f - (Step-0.5f)*2.f*0.3f);
	}

}