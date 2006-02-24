#include "gl2deffectbeziertranzit.h"
#include "gl2deffectfactory.h"
#include "../OpenGLTextureConverter.h"

namespace DCE {

GL2DBezierEffectTransit::GL2DBezierEffectTransit (GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect)
	: GL2DEffect(EffectsEngine, TimeForCompleteEffect)
{
	FullScreen.Left = 0.0f;
	FullScreen.Top = 0.0f;
	FullScreen.Width = (float)Effects->Widgets->GetWidth();
	FullScreen.Height = (float)Effects->Widgets->GetHeight();

	//creating a basic window that merge the effect
	Destination = (TBasicWindow *)Effects->Widgets->CreateWidget(BASICWINDOW, 
		0, 0, 
		Effects->Widgets->GetWidth(), Effects->Widgets->GetHeight(), 
		"Destination");	
	Destination->SetVisible(true);

	//create the button which keep the source of the screen (the button part)
	Button = (TBezierWindow *)Effects->Widgets->CreateWidget(BEZIERWINDOW, 
		0, 0, 
		Effects->Widgets->GetWidth(), Effects->Widgets->GetHeight(), 
		"Button");
	Button->SetVisible(true);
}


GL2DBezierEffectTransit::~GL2DBezierEffectTransit() {
	glDeleteTextures(1, &IDBack);
	glDeleteTextures(1, &IDFront);

	Effects->Widgets->DeleteWidget(Destination);
	Effects->Widgets->DeleteWidget(Button);	
}

void GL2DBezierEffectTransit::Configure(PlutoGraphic* SourceFrame, PlutoGraphic* DestFrame, PlutoRectangle ButtonSourceSize)
{
	BackSrf.reset(SourceFrame);
	FrontSrf.reset(DestFrame);

	ButtonSize.Left = (float)ButtonSourceSize.X;
	ButtonSize.Top = (float)ButtonSourceSize.Y-ButtonSourceSize.Height;
	ButtonSize.Width = (float)ButtonSourceSize.Width;
	ButtonSize.Height = (float)ButtonSourceSize.Height;

	Configured = false;
}

void GL2DBezierEffectTransit::Paint(int Now)
{
	if(!Configured) {
		//Generate textures for OpenGL
		IDBack = OpenGLTextureConverter::GenerateTexture(BackSrf.get());
		IDFront = OpenGLTextureConverter::GenerateTexture(FrontSrf.get());

		BackSrf.release();
		FrontSrf.release();
		
		//Set up the textures for triangles
		Button->SetTexture(IDFront);
		Destination->SetTexture(IDBack);
		
		float MaxCoordU = (FullScreen.Width)/MathUtils::MinPowerOf2((int)FullScreen.Width);
		float MaxCoordV = (FullScreen.Height)/MathUtils::MinPowerOf2((int)FullScreen.Height);
		
		Button->SetTextureWraping(0.0, 0.0, 
			MaxCoordU, MaxCoordV);
			
 		Destination->SetTextureWraping(0.0, 0.0, 
			MaxCoordU, MaxCoordV);
			
		Button->BezierDefinition.Divisions = 15;

		Start = Effects->MilisecondTimmer();	
		Configured = true;
	}
	
	float Step = Stage((float)Now);
	Step = 1-Step;
	
	bool FistStage = true;
	if (Step <= 0.5)
	{
		Step*= 2;
		Button->SetRectCoordinates(FullScreen);
	}
	else
	{
		FistStage = false;
		Step = (Step - 0.5f)*2.0f;

		FloatRect Animation = ButtonSize.Interpolate(FullScreen, 1-Step);
		Button->SetRectCoordinates(Animation);
		Button->SetAlpha(1-Step);
	}
	
	Destination->SetRectCoordinates(FullScreen);

	int i, j;
	float Average = FullScreen.Width * 2/3;

	if(!FistStage)
		Step = 1.0;

	// 0 = Start, 1 = Control Start, 2 = Control end, 3 = End 
	POINT_3D LeftProfile [4], RightProfile[4];

	for(i = 0; i<4; i++)
	{
		LeftProfile[i] = Button->BezierDefinition.anchors[0][i];
		RightProfile[i] = Button->BezierDefinition.anchors[3][i];
	}

	LeftProfile[0].x += MathUtils::InterpolateValues(0, Average, Step);
	LeftProfile[1].x = LeftProfile[0].x;

	RightProfile[0].x -= MathUtils::InterpolateValues(0, Average/2, Step);
	RightProfile[0].y = LeftProfile[0].y;
	RightProfile[1].x = RightProfile[0].x;
	RightProfile[1].y = LeftProfile[1].y;

	POINT_3D Left1 = MathUtils::Bernstein(0.33f, LeftProfile);
	POINT_3D Left2 = MathUtils::Bernstein(0.66f, LeftProfile);

	LeftProfile[1].x = Left1.x;
	LeftProfile[2].x = Left2.x;


	POINT_3D Right1 = MathUtils::Bernstein(0.66f, RightProfile);
	POINT_3D Right2 = MathUtils::Bernstein(0.33f, RightProfile);

	RightProfile[1] = Right1;
	RightProfile[2] = Right2;
	for(i = 0; i<4; i++)
	{
		Button->BezierDefinition.anchors[0][i] = LeftProfile[i];
		Button->BezierDefinition.anchors[3][i] = RightProfile[i];
		for(j=1; j<=2; j++)
		{
			Button->BezierDefinition.anchors[j][i].x = 
				MathUtils::InterpolateValues(
				float(LeftProfile[i].x),
				float(RightProfile[i].x),
				j/3.0f
				);
		}
	}

}

} // end of namespace DCE

