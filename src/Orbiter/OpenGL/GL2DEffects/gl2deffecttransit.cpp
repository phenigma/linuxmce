#include "gl2deffecttransit.h"

#include "../Orbiter3DCommons.h"

#include "../DCE/Logger.h"

#include "gl2deffectfactory.h"

GL2DEffectTransit::GL2DEffectTransit(GL2DEffectFactory * EffectsEngine, int TimeForCompleteEffect)
	: GL2DEffect(EffectsEngine, TimeForCompleteEffect)
{
	//clean up the highlighted items to not have remain on the new screen
	Orbiter3DCommons::GetInstance()->SetHighLightArea(NULL);
	Orbiter3DCommons::GetInstance()->SetSelectedArea(NULL);
	TBasicWindow* Screen3D = Orbiter3DCommons::GetInstance()->GetScreen3D();
	Screen3D->SetBackgroundImage(
		Effects->Widgets->OldScreen
	);
}
GL2DEffectTransit::~GL2DEffectTransit()
{
	//clean up the highlighted items to not have remain on the new screen
	Orbiter3DCommons::GetInstance()->SetHighLightArea(NULL);
	Orbiter3DCommons::GetInstance()->SetSelectedArea(NULL);
	Orbiter3DCommons::GetInstance()->GetScreen3D()->SetBackgroundImage(Effects->Widgets->NewScreen);
}

void GL2DEffectTransit::Paint()
{
	//clean up the highlighted items to not have remain on the new screen
	Orbiter3DCommons::GetInstance()->SetHighLightArea(NULL);
	Orbiter3DCommons::GetInstance()->SetSelectedArea(NULL);

	TBasicWindow* WinHigh = Orbiter3DCommons::GetInstance()->GetHighLight();
	int Width = WinHigh->GetWidth();
	int Height = WinHigh->GetHeight();

	g_pPlutoLogger->Write(LV_CRITICAL, "Transit higlight size: %d %d\n", Width, Height);
	
}