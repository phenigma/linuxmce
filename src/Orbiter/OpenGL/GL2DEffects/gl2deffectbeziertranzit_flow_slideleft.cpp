#include "gl2deffectbeziertranzit_flow_slideleft.h"
#include "gl2deffectfactory.h"

#include "../Orbiter3DCommons.h"

GL2DBezierEffectTransit_Flow_SlideLeft::GL2DBezierEffectTransit_Flow_SlideLeft (GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect)
: GL2DEffectTransit(EffectsEngine, TimeForCompleteEffect)
{
	FullScreen.Left = 0.0f;
	FullScreen.Top = 0.0f;
	FullScreen.Width = (float)Effects->Widgets->GetWidth();
	FullScreen.Height = (float)Effects->Widgets->GetHeight();

	//create the button which keep the source of the screen (the button part)
	ButonTop = (TBezierWindow *)Effects->Widgets->CreateWidget(BEZIERWINDOW, 
		0, 0, 
		Effects->Widgets->GetWidth(), Effects->Widgets->GetHeight(), 
		"Button");
	ButonTop ->SetVisible(true);
}


GL2DBezierEffectTransit_Flow_SlideLeft::~GL2DBezierEffectTransit_Flow_SlideLeft() {
	Effects->Widgets->DeleteWidget(ButonTop);	

	ButonTop = NULL;
}

void GL2DBezierEffectTransit_Flow_SlideLeft::Configure(PlutoRectangle* EffectSourceSize)
{
	ButtonSize.Left = (float)EffectSourceSize->X;
	ButtonSize.Top = (float)EffectSourceSize->Y-EffectSourceSize->Height;
	ButtonSize.Width = (float)EffectSourceSize->Width;
	ButtonSize.Height = (float)EffectSourceSize->Height;

	Configured = false;
}

void GL2DBezierEffectTransit_Flow_SlideLeft::Paint(int Now)
{
	GL2DEffectTransit::Paint();
	if(!Configured) 
	{
		//Set up the textures for triangles
		ButonTop->SetTexture(Effects->Widgets->NewScreen);

		float MaxCoordU = (FullScreen.Width)/MathUtils::MinPowerOf2((int)FullScreen.Width);
		float MaxCoordV = (FullScreen.Height)/MathUtils::MinPowerOf2((int)FullScreen.Height);

		ButonTop->SetTextureWraping(0.0, 0.0, 
			MaxCoordU, MaxCoordV);

		ButonTop->BezierDefinition.Divisions = 50;

		Start = Effects->MilisecondTimmer();	
		Configured = true;
	}

	float Step = Stage((float)Now), Step2;
	Step = 1-Step;
	//Step2 = keeped for backup reason of Step variable
	Step2 = Step;

	ButonTop->SetAlpha(1-Step);
	FloatRect Animation;

	Animation = ButtonSize.Interpolate(FullScreen, 1-Step);

	ButonTop->SetRectCoordinates(Animation);
	Step = Step2;
	if( Step < 0.5)
		Step *= 2;
	else
		Step= 1-(Step - 0.5f)*2; 

	int i;
	float Average = Animation.Width * 2/3;

	// bit-to-bit copy 
	BEZIER_PATCH BezierDefinition = ButonTop->BezierDefinition;

	// 0 = Start, 1 = Control Start, 2 = Control end, 3 = End 
	float TopProfile [4], LeftProfile[4];

	Step = Step2;
	LeftProfile[0] = 0.0f;
	LeftProfile[1] = MathUtils::InterpolateValues(
		0,
		Animation.Width,
		0.33f*Step);
	LeftProfile[2] = LeftProfile[1];
	LeftProfile[3] = 0.0f;

	TopProfile[0] = 0.0f;
	TopProfile[1] = MathUtils::InterpolateValues(
		0,
		Animation.Height,
		0.33f*Step);
	TopProfile[2] = TopProfile[1];
	TopProfile[3] = 0.0f;

	for(i = 0; i<4; i++)
	{
		ButonTop->BezierDefinition.anchors[i][0].y -= 
			TopProfile[i];
		ButonTop->BezierDefinition.anchors[i][3].y -= 
			TopProfile[i];

		ButonTop->BezierDefinition.anchors[0][i].x -= 
			2*LeftProfile[i];
		ButonTop->BezierDefinition.anchors[3][i].x -= 
			LeftProfile[i];
	}
}
