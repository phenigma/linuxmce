#include "gl2deffectbeziertranzit_topleft.h"
#include "gl2deffectfactory.h"

#include "../Orbiter3DCommons.h"

GL2DBezierEffectTransit_TopLeft::GL2DBezierEffectTransit_TopLeft (GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect)
	: GL2DEffectTransit(EffectsEngine, TimeForCompleteEffect)
{
	FullScreen.Left = 0.0f;
	FullScreen.Top = 0.0f;
	FullScreen.Width = (float)Effects->Widgets->GetWidth();
	FullScreen.Height = (float)Effects->Widgets->GetHeight();

	//create the button which keep the source of the screen (the button part)
	Button = (TBezierWindow *)Effects->Widgets->CreateWidget(BEZIERWINDOW, 
		0, 0, 
		Effects->Widgets->GetWidth(), Effects->Widgets->GetHeight(), 
		"Button");
	Button->SetVisible(true);
}


GL2DBezierEffectTransit_TopLeft::~GL2DBezierEffectTransit_TopLeft() {
	Effects->Widgets->DeleteWidget(Button);	

	Button = NULL;
}

void GL2DBezierEffectTransit_TopLeft::Configure(PlutoRectangle* EffectSourceSize)
{
	ButtonSize.Left = (float)EffectSourceSize->X;
	ButtonSize.Top = (float)EffectSourceSize->Y;
	ButtonSize.Width = (float)EffectSourceSize->Width;
	ButtonSize.Height = (float)EffectSourceSize->Height;

	Configured = false;
}

void GL2DBezierEffectTransit_TopLeft::Paint(int Now)
{
	GL2DEffectTransit::Paint();
	if(!Configured) 
	{
		//Set up the textures for triangles
		Button->SetTexture(Effects->Widgets->NewScreen);
		
		Orbiter3DCommons::GetInstance()->GetScreen3D()->SetTexture(Effects->Widgets->OldScreen);
		
		//Orbiter3DCommons::GetInstance()->GetScreen3D()->SetVisible(false);
		
		float MaxCoordU = (FullScreen.Width)/MathUtils::MinPowerOf2((int)FullScreen.Width);
		float MaxCoordV = (FullScreen.Height)/MathUtils::MinPowerOf2((int)FullScreen.Height);
		
		Button->SetTextureWraping(0.0, 0.0, 
			MaxCoordU, MaxCoordV);
			
		Button->BezierDefinition.Divisions = 50;

		Start = Effects->MilisecondTimmer();	
		Configured = true;

		Button->BezierDefinition.Divisions = 25;
	}
	
	float Step = Stage((float)Now);
	//Step = 1-Step;
	//Step2 = keeped for backup reason of Step variable
	
	//Button->SetAlpha(Step);

	FloatRect Animation;
	
	if(Step < 0.5)
		Animation = ButtonSize.Interpolate(FullScreen, Step*2);
	else
		Animation = FullScreen;

	Button->SetRectCoordinates(Animation);
	
	int i, j;

	// bit-to-bit copy (backup)
	BEZIER_PATCH BezierDefinition = Button->BezierDefinition;

	// 0 = Start, 1 = Control Start, 2 = Control end, 3 = End 
	float TopProfile [4], LeftProfile[4];

	//Step = 1-Step;

	if(Step<0.5)
		Step = 1.0f;
	else
		Step = (1.0f - (Step-0.5f)*2.0f);

	for(i = 0; i<4; i++)
	{
		LeftProfile[0] = 0.0f;
		LeftProfile[1] = MathUtils::InterpolateValues(
			0,
			Animation.Width,
			0.33f*Step*i/3.0f
			);
		LeftProfile[2] = LeftProfile[1];
		LeftProfile[3] = 0.0f;

		TopProfile[0] = 0.0f;
		TopProfile[1] = MathUtils::InterpolateValues(
			0,
			Animation.Height,
			0.33f*Step*i/3.0f
			);
		TopProfile[2] = TopProfile[1];
		TopProfile[3] = 0.0f;
	}


	for(i = 0; i<4; i++)
	{
		Button->BezierDefinition.anchors[i][3].y -= 
			TopProfile[i];
		Button->BezierDefinition.anchors[i][0].y += 
			TopProfile[i];
		
		Button->BezierDefinition.anchors[0][i].x += 
			LeftProfile[i];
		Button->BezierDefinition.anchors[3][i].x -= 
			LeftProfile[i];
	}

	for (i = 1; i<= 2; i++)
		for (j = 1; j<= 2; j++)
		{
			Button->BezierDefinition.anchors[j][i].x = 
				MathUtils::InterpolateValues(
				Button->BezierDefinition.anchors[0][i].x,
				Button->BezierDefinition.anchors[3][i].x,
				j*0.33f);
			Button->BezierDefinition.anchors[i][j].y = 
				MathUtils::InterpolateValues(
				Button->BezierDefinition.anchors[i][0].y,
				Button->BezierDefinition.anchors[i][3].y,
				j*0.33f);
		}
		for (i = 1; i< 3; i++)
			for (j = 1; j< 3; j++)
			{
				float Delta = 
					fabs(BezierDefinition.anchors[j][i].x -
					Button->BezierDefinition.anchors[j][i].x)+
					fabs(BezierDefinition.anchors[j][i].y -
					Button->BezierDefinition.anchors[j][i].y);
				Button->BezierDefinition.anchors[j][i].z =
					Button->BezierDefinition.anchors[j][i].z +
					Delta;
			}
					
}
