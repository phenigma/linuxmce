#include "gl2deffecttransit.h"

#include "gl2deffectfactory.h"
#include "../OpenGLTextureConverter.h"

namespace DCE {

GL2DEffectTransit::GL2DEffectTransit (GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect)
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

	//create the button which keep the source of the screen (the button part)
	Button = (TBasicWindow*) Effects->Widgets->CreateWidget(BASICWINDOW, 
		0, 0, 
		Effects->Widgets->GetWidth(), Effects->Widgets->GetHeight(), 
		"Button");
	Button->SetVisible(true);
	
	cout<<"Effect with two widgets"<<endl;
}


GL2DEffectTransit::~GL2DEffectTransit() {
	Effects->Widgets->DeleteWidget(Background);
	Effects->Widgets->DeleteWidget(Destination);
	Effects->Widgets->DeleteWidget(Button);	
}

/**
 * It copy the SourceFrame as button texture, DestFrame as destination image
 * @param SourceFrame 
 * @param DestFrame 
 * @param ButtonSourceSize 
 */
void GL2DEffectTransit::Configure(PlutoRectangle* EffectSourceSize)
{
	ButtonSize.Left = float(EffectSourceSize->X);
	ButtonSize.Top = float(EffectSourceSize->Y-EffectSourceSize->Height);
	ButtonSize.Width = float(EffectSourceSize->Width);
	ButtonSize.Height = float(EffectSourceSize->Height);
	
	Configured = false;
}

/**
 * Paint the effect based of the current time
 * @param Now Gives the global time
 */
void GL2DEffectTransit::Paint(int Now)
{
	if(!Configured) {
		//Set up the textures for triangles
		Background->SetTexture(Effects->Widgets->OldScreen);
		Button->SetTexture(Effects->Widgets->OldScreen);
		Destination->SetTexture(Effects->Widgets->NewScreen);
		
		float MaxCoordU = (FullScreen.Width)/MathUtils::MinPowerOf2((int)FullScreen.Width);
		float MaxCoordV = (FullScreen.Height)/MathUtils::MinPowerOf2((int)FullScreen.Height);
		
		cout <<"CoordinatesButton:"<<ButtonSize.Top<<","<<
			ButtonSize.Left<<","<<
			ButtonSize.Width<<","<<
			ButtonSize.Height<<","<<
			endl;
		Button->SetTextureWraping(
			ButtonSize.Left/FullScreen.Width*MaxCoordU,
			MaxCoordV * (1 -(ButtonSize.Top+ButtonSize.Height)/FullScreen.Height),
			ButtonSize.Width/FullScreen.Width*MaxCoordU,
			MaxCoordV * ButtonSize.Height/FullScreen.Height
			);

			/*Button->SetTextureWraping(0.0, 0.0, 
			MaxCoordU, MaxCoordV);*/
			
		Destination->SetTextureWraping(0.0, 0.0, 
			MaxCoordU, MaxCoordV);
		Start = Effects->MilisecondTimmer();

		Background->SetTextureWraping(0.0, 0.0, 
			MaxCoordU, MaxCoordV);
		
		Configured = true;
	}
	
	float Step = Stage(float(Now));
	
	FloatRect Animation = ButtonSize.Interpolate(FullScreen, Step);

	Button->SetRectCoordinates(Animation);
	Button->SetAlpha(1-Step);
	
	Destination->SetRectCoordinates(Animation);
	Destination->SetAlpha(Step);

	Background->SetRectCoordinates(FullScreen);
}

} // end of namespace DCE

