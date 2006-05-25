#include "GL2DEffectWipeIn.h"

#include "gl2deffectfactory.h"
#include "../OpenGLTextureConverter.h"

#include "../Orbiter3DCommons.h"

GL2DEffectWipeIn::GL2DEffectWipeIn (GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect)
	: GL2DEffectTransit(EffectsEngine, TimeForCompleteEffect)
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

	float MaxCoordU = FullScreen.Width/MathUtils::MinPowerOf2((int)FullScreen.Width);
	float MaxCoordV = FullScreen.Height/MathUtils::MinPowerOf2((int)FullScreen.Height);

	if(!Configured) {
		//Set up the textures for triangles
		Destination->SetTexture(Effects->Widgets->NewScreen);
		
		Start = Effects->MilisecondTimmer();
		
		Configured = true;
	}
	
	float Step = Stage(float(Now));
	
	FloatRect Animation = ButtonSize.Interpolate(FullScreen, Step);

	Destination->SetRectCoordinates(Animation);

	FloatRect UVRect_, UVRect_FullScreen;
	UVRect_.Left = Animation.Left/FullScreen.Width*MaxCoordU;
	UVRect_.Top = Animation.Top/FullScreen.Height*MaxCoordV;
	UVRect_.Width = Animation.Width/FullScreen.Width*MaxCoordU;
	UVRect_.Height = Animation.Height/FullScreen.Height*MaxCoordV;

	Destination->SetAlpha(0.7f + 0.3f * Step);
	Destination->SetTextureWraping(
		UVRect_
		);

}
