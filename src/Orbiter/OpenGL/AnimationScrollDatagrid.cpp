#include "AnimationScrollDatagrid.h"

#include "Mesh/MeshFrame.h"
#include "OpenGL3DEngine.h"

AnimationScrollDatagrid::AnimationScrollDatagrid(OpenGL3DEngine* Engine, MeshFrame *BeforeGrid, 
	MeshFrame *AfterGrid, int MilisecondTime, int Direction)
{
	this->Engine = Engine;
	this->BeforeGrid = BeforeGrid;
	this->AfterGrid = AfterGrid;
	this->MilisecondTime = MilisecondTime;
	this->Direction = Direction;
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

	BeforeGrid->SetAlpha(1-Progress);
	AfterGrid->SetAlpha(Progress);

	if(Direction != 2)
	{
		MeshTransform Transform;
		Transform.Translate(CurrentTime - StartTime, 0, 0);
		Transform.Translate(-0, -360, -360);
		Transform.ApplyRotateX((-Progress)*90);
		Transform.ApplyTranslate(0, 360, 360);
		BeforeGrid->SetTransform(Transform);

		MeshTransform Transform2;
		Transform2.Translate(0, -360, -360);
		Transform2.ApplyRotateX((1-Progress)*90);
		Transform2.ApplyTranslate(0, 360, 360);
		AfterGrid->SetTransform(Transform2);
	}
	else
	{
		MeshTransform Transform;
		Transform.Translate(CurrentTime - StartTime, 0, 0);
		Transform.Translate(-0, -360, -360);
		Transform.ApplyRotateX((Progress)*90);
		Transform.ApplyTranslate(0, 360, 360);
		BeforeGrid->SetTransform(Transform);

		MeshTransform Transform2;
		Transform2.Translate(0, -360, -360);
		Transform2.ApplyRotateX((-1+Progress)*90);
		Transform2.ApplyTranslate(0, 360, 360);
		AfterGrid->SetTransform(Transform2);
	}

	if(CurrentTime - StartTime > MilisecondTime)
		return true;

	return false;
}
