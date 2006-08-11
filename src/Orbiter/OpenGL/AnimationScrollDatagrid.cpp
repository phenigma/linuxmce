#include "AnimationScrollDatagrid.h"

#include "Mesh/MeshFrame.h"
#include "OpenGL3DEngine.h"

#include "DCE/Logger.h"

AnimationScrollDatagrid::AnimationScrollDatagrid(OpenGL3DEngine* Engine, MeshFrame *BeforeGrid, 
	MeshFrame *AfterGrid, int MilisecondTime, int Direction, float fMaxAlphaLevel)
{
	this->Engine = Engine;
	this->BeforeGrid = BeforeGrid;
	this->AfterGrid = AfterGrid;
	this->MilisecondTime = MilisecondTime;
	this->Direction = Direction;
	this->MaxAlpha = fMaxAlphaLevel;
}

AnimationScrollDatagrid::~AnimationScrollDatagrid(void)
{
	this->AfterGrid = NULL;
	this->BeforeGrid = NULL;
}

void AnimationScrollDatagrid::StartAnimation()
{
	StartTime = SDL_GetTicks();
}

bool AnimationScrollDatagrid::Update()
{
	int CurrentTime = Engine->GetTick();
	float Progress = float(CurrentTime - StartTime) / MilisecondTime;
	if(CurrentTime - StartTime > MilisecondTime)
		Progress = 1.0f;
	int Height = Engine->Compose->Height;

	if(Direction != 2)
	{
		MeshTransform Transform;
		Transform.Translate(-0, -Height/2, -Height/2);
		Transform.ApplyRotateX((-Progress)*90);
		Transform.ApplyTranslate(0, Height/2, Height/2);
		BeforeGrid->SetTransform(Transform);

		MeshTransform Transform2;
		Transform2.Translate(0, -Height/2, -Height/2);
		Transform2.ApplyRotateX((1-Progress)*90);
		Transform2.ApplyTranslate(0, Height/2, Height/2);
		AfterGrid->SetTransform(Transform2);
	}
	else
	{
		MeshTransform Transform;
		Transform.Translate(-0, -Height/2, -Height/2);
		Transform.ApplyRotateX((Progress)*90);
		Transform.ApplyTranslate(0, Height/2, Height/2);
		BeforeGrid->SetTransform(Transform);

		MeshTransform Transform2;
		Transform2.Translate(0, -Height/2, -Height/2);
		Transform2.ApplyRotateX((-1+Progress)*90);
		Transform2.ApplyTranslate(0, Height/2, Height/2);
		AfterGrid->SetTransform(Transform2);
	}

	//DCE::g_pPlutoLogger->Write(LV_STATUS, "xxxxx Animation step: %f", 100 * Progress);

	//BeforeGrid->SetAlpha((1-Progress) * MaxAlpha);
	//AfterGrid->SetAlpha(Progress * MaxAlpha);

	if(CurrentTime - StartTime > MilisecondTime)
		return true;

	return false;
}
