#include "gl2deffectfadesfromtop.h"

#include "gl2deffectfactory.h"
#include "../Layers/GL2DEffectLayersCompose.h"

namespace GLEffect2D
{

GL2DEffectFadesFromTop::GL2DEffectFadesFromTop (EffectFactory * EffectsEngine, int StartAfter,
												int TimeForCompleteEffect)
	: GL2DEffectTransit(EffectsEngine, StartAfter, TimeForCompleteEffect)
{
	FullScreen.Left = 0;
	FullScreen.Top = 0;
	FullScreen.Width = float(Effects->Widgets->GetWidth());
	FullScreen.Height = float(Effects->Widgets->GetHeight());

	//creating a basic window that merge the effect
	Destination = (TBasicWindow*)Effects->Widgets->CreateWidget(BASICWINDOW, 
		0, 0, 
		Effects->Widgets->GetWidth(), Effects->Widgets->GetHeight(), 
		"Destination");	
	Destination->SetVisible(true);
}


GL2DEffectFadesFromTop::~GL2DEffectFadesFromTop() {
	Effects->Widgets->DeleteWidget(Destination);
}

void GL2DEffectFadesFromTop::Configure(PlutoRectangle* EffectSourceSize)
{
	ButtonSize.Left = 0;
	ButtonSize.Top = FullScreen.Height;
	ButtonSize.Width = FullScreen.Width;
	ButtonSize.Height = FullScreen.Height;

	Configured = false;
}

void GL2DEffectFadesFromTop::Paint(int Now)
{
	GL2DEffectTransit::Paint();

	if(!Configured) {
		//Set up the textures for triangles
		OpenGLGraphic* ScreenRenderGraphic = Effects->ParentLayer->RenderGraphic;
		Destination->SetBackgroundImage(ScreenRenderGraphic);

		float MaxCoordU = 1;
		float MaxCoordV = 1;
		
		Destination->SetTextureWraping(0.0, 0.0, 
			MaxCoordU, MaxCoordV);
		Start = Effects->MilisecondTimmer();

		Configured = true;
	}
	
	float Step = Stage(float(Now));
	
	FloatRect Animation = ButtonSize.Interpolate(FullScreen, Step);

	Destination->SetRectCoordinates(Animation);
	Destination->SetAlpha(Step);
}

}
