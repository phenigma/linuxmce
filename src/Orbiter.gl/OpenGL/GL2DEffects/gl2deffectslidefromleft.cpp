#include "gl2deffectslidefromleft.h"

#include "gl2deffectfactory.h"
#include "../OpenGLTextureConverter.h"

GL2DEffectSlideFromLeft::GL2DEffectSlideFromLeft (GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect)
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


GL2DEffectSlideFromLeft::~GL2DEffectSlideFromLeft() {
	Effects->Widgets->DeleteWidget(Destination);
}

/**
 * It copy the SourceFrame as button texture, DestFrame as destination image
 * @param SourceFrame 
 * @param DestFrame 
 * @param ButtonSourceSize 
 */
void GL2DEffectSlideFromLeft::Configure(PlutoRectangle* EffectSourceSize)
{
	ButtonSize.Left = -FullScreen.Width;
	ButtonSize.Top = 0;
	ButtonSize.Width = FullScreen.Width;
	ButtonSize.Height = FullScreen.Height;
	
	Configured = false;
}

/**
 * Paint the effect based of the current time
 * @param Now Gives the global time
 */
void GL2DEffectSlideFromLeft::Paint(int Time)
{
	GL2DEffectTransit::Paint();

	if(!Configured) {
		//Set up the textures for triangles
		//Destination->SetTexture(Effects->Widgets->NewScreen);
		
		float MaxCoordU = (FullScreen.Width)/MathUtils::MinPowerOf2((int)FullScreen.Width);
		float MaxCoordV = (FullScreen.Height)/MathUtils::MinPowerOf2((int)FullScreen.Height);
		
		Destination->SetTextureWraping(0.0, 0.0, 
			MaxCoordU, MaxCoordV);

		Start = Effects->MilisecondTimmer();

		Configured = true;
	}
	
	float Step = Stage(float(Time));
	
	FloatRect Animation = ButtonSize.Interpolate(FullScreen, Step);
	
	Destination->SetRectCoordinates(Animation);
}


