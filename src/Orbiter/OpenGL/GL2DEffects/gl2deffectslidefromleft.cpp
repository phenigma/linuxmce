#include "gl2deffectslidefromleft.h"

#include "gl2deffectfactory.h"
#include "../OpenGLTextureConverter.h"

namespace DCE {

GL2DEffectSlideFromLeft::GL2DEffectSlideFromLeft (GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect)
	: GL2DEffect(EffectsEngine, TimeForCompleteEffect)
{
	FullScreen.Left = 0;
	FullScreen.Top = 0;
	FullScreen.Width = Effects->Widgets->GetWidth();
	FullScreen.Height = Effects->Widgets->GetHeight();

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


GL2DEffectSlideFromLeft::~GL2DEffectSlideFromLeft() {
	glDeleteTextures(1, &IDBack);
	glDeleteTextures(1, &IDFront);

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
void GL2DEffectSlideFromLeft::Configure(PlutoGraphic* SourceFrame, PlutoGraphic* DestFrame)
{
	BackSrf.reset(SourceFrame);
	FrontSrf.reset(DestFrame);

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
void GL2DEffectSlideFromLeft::Paint(int Now)
{
	if(!Configured) {
		//Generate textures for OpenGL
		IDBack = OpenGLTextureConverter::GenerateTexture(BackSrf.get());
		IDFront = OpenGLTextureConverter::GenerateTexture(FrontSrf.get());

		BackSrf.release();
		FrontSrf.release();

		//Set up the textures for triangles
		Background->SetTexture(IDBack);
		Button->SetTexture(IDBack);
		Destination->SetTexture(IDFront);
		
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
	
	float Step = Stage(Now);
	
	FloatRect Animation = ButtonSize.Interpolate(FullScreen, Step);

	Button->SetRectCoordinates(Animation);
	
	Destination->SetRectCoordinates(Animation);

	Background->SetRectCoordinates(FullScreen);
}

} // end of namespace DCE

