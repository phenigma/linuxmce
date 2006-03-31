#include "GL2DEffectBlending.h"

#include "gl2deffectfactory.h"
#include "../OpenGLTextureConverter.h"

#ifdef IGNORE_NV_EXTENSIONS
#include "../nvidia/nv_shaders.h"
#endif

#include "../Orbiter3DCommons.h"

GL2DEffectBlending::GL2DEffectBlending (GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect)
	: GL2DEffectTransit(EffectsEngine, TimeForCompleteEffect)
{
#ifdef NV_EXTENSIONS
	nvtest();
#endif

	FullScreen.Left = 0;
	FullScreen.Top = 0;
	FullScreen.Width = float(Effects->Widgets->GetWidth());
	FullScreen.Height = float(Effects->Widgets->GetHeight());

	//creating the basic window which it stay on back
	Background = (TBasicWindow*)Effects->Widgets->CreateWidget(BASICWINDOW, 
		0, 0, 
		Effects->Widgets->GetWidth(), Effects->Widgets->GetHeight(), 
		"Background");
	//Background->SetVisible(true);

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
}


GL2DEffectBlending::~GL2DEffectBlending() {
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
void GL2DEffectBlending::Configure(PlutoRectangle* EffectSourceSize)
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
void GL2DEffectBlending::Paint(int Now)
{
	GL2DEffectTransit::Paint();

	if(!Configured) {
		//Set up the textures for triangles
		Orbiter3DCommons::GetInstance()->GetScreen3D()->SetTexture(Effects->Widgets->OldScreen);
		//Background->SetTexture(Effects->Widgets->OldScreen);
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
