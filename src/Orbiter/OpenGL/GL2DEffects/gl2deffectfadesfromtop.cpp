#include "gl2deffectfadesfromtop.h"

#include "gl2deffectfactory.h"
#include "../OpenGLTextureConverter.h"

namespace DCE {

GL2DEffectFadesFromTop::GL2DEffectFadesFromTop (GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect)
	: GL2DEffect(EffectsEngine, TimeForCompleteEffect)
{
	FullScreen.Left = 0;
	FullScreen.Top = 0;
	FullScreen.Width = float(Effects->Widgets->GetWidth());
	FullScreen.Height = float(Effects->Widgets->GetHeight());

	//creating the basic window which it stay on back
	Background = (TBasicWindow*)Effects->Widgets->CreateWidget(BASICWINDOW, 
		0, 0, 
		Effects->Widgets->GetWidth(), Effects->Widgets->GetHeight(), 
		"Background");
	Background->SetVisible(true);

	//creating a basic window that merge the effect
	Destination = (TBasicWindow*)Effects->Widgets->CreateWidget(BASICWINDOW, 
		0, 0, 
		Effects->Widgets->GetWidth(), Effects->Widgets->GetHeight(), 
		"Destination");	
	Destination->SetVisible(true);

}


GL2DEffectFadesFromTop::~GL2DEffectFadesFromTop() {
	Effects->Widgets->DeleteWidget(Background);
	Effects->Widgets->DeleteWidget(Destination);
}

/**
 * It copy the SourceFrame as button texture, DestFrame as destination image
 * @param SourceFrame 
 * @param DestFrame 
 * @param ButtonSourceSize 
 */
void GL2DEffectFadesFromTop::Configure(PlutoRectangle* EffectSourceSize)
{
	ButtonSize.Left = 0;
	ButtonSize.Top = FullScreen.Height;
	ButtonSize.Width = FullScreen.Width;
	ButtonSize.Height = FullScreen.Height;
	
	Configured = false;
}

/**
 * Paint the effect based of the current time
 * @param Now Gives the global time
 */
void GL2DEffectFadesFromTop::Paint(int Now)
{
	if(!Configured) {
		//Set up the textures for triangles
		Background->SetTexture(Effects->Widgets->OldScreen);
		Destination->SetTexture(Effects->Widgets->NewScreen);

		float MaxCoordU = (FullScreen.Width)/MathUtils::MinPowerOf2((int)FullScreen.Width);
		float MaxCoordV = (FullScreen.Height)/MathUtils::MinPowerOf2((int)FullScreen.Height);
		
		Destination->SetTextureWraping(0.0, 0.0, 
			MaxCoordU, MaxCoordV);
		Start = Effects->MilisecondTimmer();

		Background->SetTextureWraping(0.0, 0.0, 
			MaxCoordU, MaxCoordV);
		
		Configured = true;
	}
	
	float Step = Stage(float(Now));
	
	FloatRect Animation = ButtonSize.Interpolate(FullScreen, Step);

	Destination->SetRectCoordinates(Animation);
	Destination->SetAlpha(Step);

	Background->SetRectCoordinates(FullScreen);
}

} // end of namespace DCE

