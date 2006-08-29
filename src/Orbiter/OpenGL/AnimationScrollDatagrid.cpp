#include "AnimationScrollDatagrid.h"

#include "Mesh/MeshFrame.h"
#include "OpenGL3DEngine.h"

#include "DCE/Logger.h"

AnimationScrollDatagrid::AnimationScrollDatagrid(string ObjectID, OpenGL3DEngine* Engine, MeshFrame *BeforeGrid, 
	MeshFrame *AfterGrid, int MilisecondTime, int Direction, float fMaxAlphaLevel, vector <string> pDependencies)
{
	Finished = false;
	this->Engine = Engine;
	this->BeforeGrid = BeforeGrid;
	this->AfterGrid = AfterGrid;
	this->MilisecondTime = MilisecondTime;
	this->Direction = Direction;
	this->MaxAlpha = fMaxAlphaLevel;
	this->ObjectID = ObjectID;

	CopyDependencies(pDependencies);
}

AnimationScrollDatagrid::~AnimationScrollDatagrid(void)
{
	this->AfterGrid = NULL;

	BeforeGrid->CleanUp();
	delete BeforeGrid;
	BeforeGrid = NULL;
}

void AnimationScrollDatagrid::StartAnimation()
{
	StartTime = SDL_GetTicks();
}

bool AnimationScrollDatagrid::Update(bool ModifyGeometry)
{
	if(Finished)
		return true;

	bool Result = false;
	int CurrentTime = Engine->GetTick();
	float Progress = float(CurrentTime - StartTime) / MilisecondTime;
	if(CurrentTime - StartTime > MilisecondTime)
	{
		Result = true;
		Progress = 1.0f;
	}

	if(!ModifyGeometry)
		return false;

	int Height = Engine->Compose->Height;

	if(Direction != 2)
	{
		MeshTransform Transform;
		Transform.Translate(-0, float(-Height/2), float(-Height/2));
		Transform.ApplyRotateX(float((-Progress)*90));
		Transform.ApplyTranslate(0, float(Height/2), float(Height/2));
		BeforeGrid->SetTransform(Transform);

		MeshTransform Transform2;
		Transform2.Translate(0, float(-Height/2), float(-Height/2));
		Transform2.ApplyRotateX(float((1-Progress)*90));
		Transform2.ApplyTranslate(0, float(Height/2), float(Height/2));
		AfterGrid->SetTransform(Transform2);
	}
	else
	{
		MeshTransform Transform;
		Transform.Translate(-0, float(-Height/2), float(-Height/2));
		Transform.ApplyRotateX(float((Progress)*90));
		Transform.ApplyTranslate(0, float(Height/2), float(Height/2));
		BeforeGrid->SetTransform(Transform);

		MeshTransform Transform2;
		Transform2.Translate(0, float(-Height/2), float(-Height/2));
		Transform2.ApplyRotateX(float((-1+Progress)*90));
		Transform2.ApplyTranslate(0, float(Height/2), float(Height/2));
		AfterGrid->SetTransform(Transform2);
	}

	//DCE::g_pPlutoLogger->Write(LV_STATUS, "xxxxx Animation step: %f", 100 * Progress);

	BeforeGrid->SetAlpha((1-Progress) * MaxAlpha, "text");
	AfterGrid->SetAlpha(Progress * MaxAlpha, "text");

	if(Result)
	{
		TextureManager::Instance()->ResumeTextureRelease();
		return true; 
	}
	Engine->UnHighlight();

	return Result;
}

void AnimationScrollDatagrid::CopyDependencies(vector <string> pDependencies)
{
	this->Dependencies.clear();
	for(vector<string>::iterator it = pDependencies.begin(), End = pDependencies.end();
		it != End; ++it)
		Dependencies.push_back(*it);

}

bool AnimationScrollDatagrid::UpdateStartTime(int StartTime)
{
	if(this->StartTime >= StartTime)
		return false;
	this->StartTime = StartTime;
	return true;
}

int AnimationScrollDatagrid::GetStartTime()
{
	return StartTime;
}

bool AnimationScrollDatagrid::DatagridDependenciesSatisfied(vector<AnimationScrollDatagrid*> &AnimationDatagrids)
{
	for(vector<string>::iterator Item = Dependencies.begin(), End = Dependencies.end();
		Item != End; ++Item)
	{
		bool Exists = Engine->IsCubeAnimatedDatagrid(*Item);
		if(!Exists)
			return false;
		else
		{
			for(vector<AnimationScrollDatagrid*>::iterator DGItem = AnimationDatagrids.begin(),
				DGEnd = AnimationDatagrids.end(); DGItem != DGEnd; ++DGItem)
				if(!((*DGItem)->UpdateStartTime(StartTime)))
					UpdateStartTime((*DGItem)->GetStartTime());
		}
	}



	return true;
}

void AnimationScrollDatagrid::StopAnimation()
{
	Finished = true;
	MeshTransform Transform2;
	AfterGrid->SetTransform(Transform2);
	Engine->RemoveMeshFrameFromDesktop(BeforeGrid);

	TextureManager::Instance()->ResumeTextureRelease();

	//BeforeGrid->CleanUp();
	//delete BeforeGrid;
	//BeforeGrid = NULL;
}