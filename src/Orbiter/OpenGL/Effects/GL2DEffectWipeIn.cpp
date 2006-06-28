#include "GL2DEffectWipeIn.h"
#include "gl2deffectfactory.h"

#include "DCE/Logger.h"

#include "../Layers/GL2DEffectLayersCompose.h"

namespace GLEffect2D
{

GL2DEffectWipeIn::GL2DEffectWipeIn (EffectFactory * EffectsEngine, int StartAfter, 
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


GL2DEffectWipeIn::~GL2DEffectWipeIn() {
	Effects->Widgets->DeleteWidget(Destination);
}

/**
 * It copy the SourceFrame as button texture, DestFrame as destination image
 * @param SourceFrame 
 * @param DestFrame 
 * @param ButtonSourceSize 
 */
void GL2DEffectWipeIn::Configure(PlutoRectangle* EffectSourceSize)
{
	DCE::g_pPlutoLogger->Write(LV_CRITICAL, "Effect size: %d %d %d %d", 
		EffectSourceSize->X,
		EffectSourceSize->Y,
		EffectSourceSize->Width,
		EffectSourceSize->Height);

  	ButtonSize.Left = float(EffectSourceSize->X);
	ButtonSize.Top = float(EffectSourceSize->Y);
	ButtonSize.Width = float(EffectSourceSize->Width);
	ButtonSize.Height = float(EffectSourceSize->Height);
	
	Configured = false;
}

/**
 * Paint the effect based of the current time
 * @param Now Gives the global time
 */
void GL2DEffectWipeIn::Paint(int Now)
{
	GL2DEffectTransit::Paint();

	float MaxCoordU = 1;
	float MaxCoordV = 1;

	if(!Configured) {
		//Set up the textures for triangles
		Start = Effects->MilisecondTimmer();
		OpenGLGraphic* ScreenRenderGraphic = Effects->ParentLayer->RenderGraphic;
		Destination->SetBackgroundImage(ScreenRenderGraphic);
		Destination->SetTextureWraping(0.0, 0.0, 1.0f, 1.0f);
		
		Configured = true;
	}
	
	float Step = Stage(float(Now));

	FloatRect Animation = ButtonSize.Interpolate(FullScreen, Step);

	Destination->SetRectCoordinates(Animation);

	FloatRect UVRect_;
	UVRect_.Left = (Animation.Left)/FullScreen.Width;
	UVRect_.Top = (Animation.Top) / FullScreen.Height;
	UVRect_.Width = (Animation.Width)/FullScreen.Width;
	UVRect_.Height = (Animation.Height) / FullScreen.Height;

	Destination->SetAlpha(0.7f + 0.3f * Step);
	Destination->SetAbsoluteUV(UVRect_);
}

}
