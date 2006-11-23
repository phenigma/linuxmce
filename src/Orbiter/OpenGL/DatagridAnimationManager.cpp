/*
Orbiter

Copyright (C) 2006 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

@author: "Ciprian Mustiata" ciprian.m at plutohome dot com, "Cristian Miron" chris.m at plutohome dot com 

*/
#include "DatagridAnimationManager.h"
#include "AnimationScrollDatagrid.h"
#include "OpenGL3DEngine.h"

bool DatagridAnimationManager::Update()
{
	if(!m_vectCurrentAnimations.size())
		return false;

	bool bAnimationFinished = false;

	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, m_pEngine->GetSceneMutex());

	for(vector<AnimationScrollDatagrid*>::iterator it = m_vectCurrentAnimations.begin();
		it != m_vectCurrentAnimations.end(); ++it)
	{
		AnimationScrollDatagrid *pAnimation = *it;
		if(pAnimation->Update(true))
		{
			bAnimationFinished = true;
		}
	}

	if(bAnimationFinished)
		OnStopAnimation();

	return !bAnimationFinished;
}

void DatagridAnimationManager::Cleanup()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, m_pEngine->GetSceneMutex());

	for(vector<AnimationScrollDatagrid*>::iterator it = m_vectCurrentAnimations.begin();
		it != m_vectCurrentAnimations.end(); ++it)
	{
		AnimationScrollDatagrid *pAnimation = *it;

		m_pEngine->RemoveMeshFrameFromDesktop(pAnimation->BeforeGrid);
		delete pAnimation;
	}

	m_vectCurrentAnimations.clear();
}

void DatagridAnimationManager::PrepareForAnimation(string ObjectID, MeshFrame *BeforeGrid, MeshFrame *AfterGrid,
		int MilisecondTime, int Direction, float fMaxAlphaLevel, vector<string> Dependencies)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, m_pEngine->GetSceneMutex());

	if(m_vectPendingAnimations.size() == 0)
		OnPrepareForAnimation();

	AnimationScrollDatagrid* pAnimation = new AnimationScrollDatagrid(ObjectID, m_pEngine, BeforeGrid, AfterGrid, MilisecondTime, Direction, fMaxAlphaLevel, Dependencies); 

	m_vectPendingAnimations.push_back(pAnimation);
	pAnimation->StartAnimation();

	if(AnimationsPrepared())
	{
		OnStopAnimation();
		OnStartAnimation();
	}
}

bool DatagridAnimationManager::AnimationsPrepared()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, m_pEngine->GetSceneMutex());

	vector<string> vectPreparedObjects;
	map<string, bool> mapDependencies;

	for(vector<AnimationScrollDatagrid*>::iterator it = m_vectPendingAnimations.begin();
		it != m_vectPendingAnimations.end(); ++it)
	{
		AnimationScrollDatagrid *pAnimation = *it;
		vectPreparedObjects.push_back(pAnimation->ObjectID);

		for(vector<string>::iterator Item = pAnimation->Dependencies.begin(), End = pAnimation->Dependencies.end();
			Item != End; ++Item)
		{
			mapDependencies[*Item] = false;
		}
	}

	for(map<string, bool>::iterator itd = mapDependencies.begin(); itd != mapDependencies.end(); ++itd)
	{
		if(find(vectPreparedObjects.begin(), vectPreparedObjects.end(), itd->first) == vectPreparedObjects.end())
			return false;
	}

	return true;
}

void DatagridAnimationManager::OnStartAnimation()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, m_pEngine->GetSceneMutex());

	Cleanup();

	int StartTime = SDL_GetTicks();
	for(vector<AnimationScrollDatagrid*>::iterator it = m_vectPendingAnimations.begin();
		it != m_vectPendingAnimations.end(); ++it
	)
	{
		AnimationScrollDatagrid *pAnimation = *it;

		m_pEngine->AddMeshFrameToDesktop("", pAnimation->AfterGrid);
		pAnimation->UpdateStartTime(StartTime);
		m_vectCurrentAnimations.push_back(pAnimation);
	}

	m_vectPendingAnimations.clear();
}

void DatagridAnimationManager::OnPrepareForAnimation()
{

}

void DatagridAnimationManager::OnStopAnimation()
{
	Cleanup();
}

void DatagridAnimationManager::ReplaceMeshInAnimations(MeshFrame *OldFrame, MeshFrame *NewFrame)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, m_pEngine->GetSceneMutex());

	for(vector<AnimationScrollDatagrid*>::iterator it = m_vectCurrentAnimations.begin();
		it != m_vectCurrentAnimations.end(); ++it)
	{
		AnimationScrollDatagrid *pAnimationScrollDatagrid = *it;
		if(pAnimationScrollDatagrid->AfterGrid == OldFrame)
		{
			pAnimationScrollDatagrid->AfterGrid = NewFrame;
		}

	}
}

void DatagridAnimationManager::StopAnimations()
{
	Cleanup();
}
