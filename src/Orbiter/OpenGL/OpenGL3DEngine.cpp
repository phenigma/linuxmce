/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
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
#include "OpenGL3DEngine.h"

#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL_ttf.h>

#include "Mesh/MeshPainter.h"
#include "Mesh/MeshBuilder.h"
#include "Layers/GL2DEffectLayersCompose.h"
#include "Texture/TextureManager.h"
#include "DatagridAnimationManager.h"

#ifdef VIA_OVERLAY
#include "VIA/ViaOverlay.h"
#endif

#include "Simulator.h"

#include "DCE/Logger.h"
using namespace DCE;

//#define SCENE_DEBUG 1
//#define DUMP_SCENE_DEBUG 
//#define DISABLE_HIGHLIGHT 

OpenGL3DEngine::OpenGL3DEngine() : 
	AnimationRemain (false),
	SceneMutex("scene mutex"),
	CurrentLayer(NULL),
	OriginalCurrentLayer(NULL),
	OldLayer(NULL),
	SelectedFrame(NULL),
	HighLightFrame(NULL),
	HighLightPopup(NULL),
	FrameBuilder(NULL),			 
	FrameDatagrid(NULL),
	ModifyGeometry(0),
	MilisecondsHighLight(0),
	m_bNeedToRefresh(true),
	m_bWorldChanged(true),
	m_bQuit(false),
	Compose(NULL),
	m_bDatagridRendering(false)
{
	if(TTF_Init()==-1) {
		printf("Error on TTF_Init: %s\n", TTF_GetError());
		return;
	}

	MilisecondsHighLight = Simulator::GetInstance()->m_iMilisecondsHighLight;

	m_spDatagridAnimationManager.reset(new DatagridAnimationManager(this));

	pthread_mutexattr_t m_SceneAttr;
	pthread_mutexattr_init(&m_SceneAttr);
	pthread_mutexattr_settype(&m_SceneAttr, PTHREAD_MUTEX_RECURSIVE_NP);
	SceneMutex.Init(&m_SceneAttr);
	pthread_mutexattr_destroy(&m_SceneAttr);

	ForceReleaseTextures = false;

	TextureManager::Instance()->Setup(this);
}

OpenGL3DEngine::~OpenGL3DEngine()
{
	Finalize();

	pthread_mutex_destroy(&SceneMutex.mutex);
}

void OpenGL3DEngine::Finalize(void)
{
	UnHighlight();
	UnSelect();

	bool ReleaseVolatileOnly = !TextureManager::Instance()->MemoryManagementEnabled();

	if(OldLayer)
		OldLayer->CleanUp(ReleaseVolatileOnly);

	if(CurrentLayer)
		CurrentLayer->CleanUp(ReleaseVolatileOnly);

	delete OldLayer;
	OldLayer = NULL;

	delete CurrentLayer;
	CurrentLayer = NULL;

	MeshPainter::Instance()->CleanUp();
	GLEffect2D::LayersCompose::Instance()->CleanUp();
	TextureManager::Instance()->CleanUp();
}

bool OpenGL3DEngine::Paint()
{
	if(m_bQuit)
		return false;

	TextureManager::Instance()->ConvertImagesToTextures();
	TextureManager::Instance()->ReleaseTextures();	

	if(ForceReleaseTextures)
	{
		ForceReleaseTextures = false;
		return false;
	}

	// some geometry changes are expected, means we will not paint the "broken painting tree"
	if(ModifyGeometry)
		return false;

	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);
	if(NULL == CurrentLayer)
		return false;

	if(Compose->HasEffects())
		AnimationRemain = true;
	
	if(CurrentLayer->Children.size() == 0)
	{
#ifdef DEBUG
	    LoggerWrapper::GetInstance()->Write(LV_STATUS, "NOTHING to paint (current layer: %p)", CurrentLayer);		
#endif
		return false;
	}

	GL.SetClearColor(.0f, .0f, 0.0f);
	GL.ClearScreen(true, false);
		
	GL.EnableZBuffer(false);

	//LoggerWrapper::GetInstance()->Write(LV_WARNING, "OpenGL3DEngine::Paint before highlight");
	bool bDatagridAnimation = m_spDatagridAnimationManager->Update();
	if(bDatagridAnimation)
	{
		m_bWorldChanged = true;
	}
	else
	{
		if(HighLightPopup)
		{
			//LoggerWrapper::GetInstance()->Write(LV_WARNING, "OpenGL3DEngine::Paint after highlight");
			Point3D Color;
			Color.X = 1.0f;
			Color.Y = 1.0f;
			Color.Z = (GetTick() / 2 % 512) / 255.0f*MilisecondsHighLight/300;
			Color.Z = fabs(Color.Z - 1.0f)/2.0f+ 0.5f;
			Color.X = Color.Z;
			Color.Y = Color.Z;

			HighLightPopup->SetColor(Color);
		}

		if(HighLightFrame)
		{
			//LoggerWrapper::GetInstance()->Write(LV_WARNING, "OpenGL3DEngine::Paint after highlight");
			Point3D Color;
			Color.X = 1.0f;
			Color.Y = 1.0f;
			Color.Z = (GetTick() / 2 % 512) / 255.0f*MilisecondsHighLight/300;
			Color.Z = fabs(Color.Z - 1.0f)/2.0f+ 0.5f;
			Color.X = Color.Z;
			Color.Y = Color.Z;
			HighLightFrame->SetColor(Color);
		}
	}
	UpdateTopMostObjects();

	Compose->Paint();
	//LoggerWrapper::GetInstance()->Write(LV_STATUS, "Flip!");
	GL.Flip();
	
	if(m_bWorldChanged && !m_bDatagridRendering)// && !m_spDatagridAnimationManager->HasAnimations())
	{
		m_bWorldChanged = false;

#ifdef VIA_OVERLAY
		ViaOverlay::Instance().WorldChanged();
#endif
	}

	if (Compose->HasEffects() == false)
		AnimationRemain = false;

	m_bNeedToRefresh = false;
	return true;
}

/*virtual*/ int OpenGL3DEngine::GetTick()
{
	return SDL_GetTicks();
}

void OpenGL3DEngine::NewScreen(string ScreenName)
{
	RefreshScreen();

	UnHighlight();
	UnSelect();
	m_spDatagridAnimationManager->StopAnimations();
	m_spDatagridAnimationManager->StopPendingAnimations();

	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);
	m_bWorldChanged = true;

	if(NULL != OldLayer)
	{
		bool bReleaseVolatileOnly = !TextureManager::Instance()->MemoryManagementEnabled();
		OldLayer->CleanUp(bReleaseVolatileOnly);
		delete OldLayer;
	}
	OldLayer = CurrentLayer;
	
	StartTick = GetTick();

	CurrentLayer = new MeshFrame(ScreenName);
	OriginalCurrentLayer = CurrentLayer;

#ifdef SCENE_DEBUG
	MeshTransform transform;
	transform.ApplyRotateY(5.0f);
	CurrentLayer->ApplyTransform(transform);
#endif
	
	if(NULL != Compose)
		Compose->UpdateLayers(CurrentLayer, OldLayer);
}

void OpenGL3DEngine::Setup()
{
	GL.Setup();
	Compose = GLEffect2D::LayersCompose::Instance();
	Compose->Setup(GL.Width, GL.Height);
	CurrentLayer = new MeshFrame("desktop");
	OriginalCurrentLayer = CurrentLayer;
}

MeshFrame* OpenGL3DEngine::AddMeshFrameToDesktop(string ParentObjectID, MeshFrame* Frame)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);
	m_bWorldChanged = true;

	DumpScene();

#ifdef SCENE_DEBUG
	static float ZThing = 0.0f;
	ZThing += 0.2f;
	MeshTransform transform;
	transform.Translate(0.0f, 0.0f, ZThing);
	Frame->ApplyTransform(transform);
#endif

	if(NULL == CurrentLayer)
	{
		Frame->CleanUp();
		delete Frame;
		return NULL;
	}

	MeshFrame *Parent = NULL;
	
	if(ParentObjectID != "")
		Parent = CurrentLayer->FindChild(ParentObjectID);

	if(NULL != Parent)
	{
		MeshFrame *OldChild = Parent->FindChild(Frame->Name());
		if(NULL != OldChild)
		{
			if(OldChild->GetParent() != Parent)
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Got child, but I'm not its parent!");
				//throw "Got child, but I'm not its parent!";
			}
			else
			{
				return Parent->ReplaceChild(OldChild, Frame);
			}
		}
	}
	else
	{
		Parent = CurrentLayer;
		MeshFrame *DuplicatedFrame = CurrentLayer->FindChild(Frame->Name());

		if(DuplicatedFrame == Frame)
			return Frame;

		if(NULL != DuplicatedFrame)
			return CurrentLayer->ReplaceChild(DuplicatedFrame, Frame);
	}

	Parent->AddChild(Frame);

	DumpScene();
	return Frame;
}

inline void OpenGL3DEngine::DumpScene()
{
#ifdef DUMP_SCENE_DEBUG
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);
	
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "DUMPING SCENE: current layer %p, size %d",
		CurrentLayer, CurrentLayer->Children.size());

	CurrentLayer->Print("##");
#endif
}

/*virtual*/ void OpenGL3DEngine::Select(PlutoRectangle* SelectedArea)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);

	if(NULL == CurrentLayer || NULL == SelectedArea)
		return;

	MeshBuilder MB;
	MB.Begin(MBMODE_TRIANGLE_STRIP);
	//MB.SetTexture(HighSurface);
	MB.SetAlpha(0.5f);
	MB.SetColor(1.0f, 0.0f, 0.0f);
	MB.SetTexture2D(
		float(SelectedArea->Left()/GL.Width),
		float(SelectedArea->Top()/GL.Height)
		);
	MB.AddVertexFloat(
		float(SelectedArea->Left()), 
		float(SelectedArea->Top()), 
		0);

	MB.SetTexture2D(
		float(SelectedArea->Right()/GL.Width),
		float(SelectedArea->Top()/GL.Height)
		);
	MB.AddVertexFloat(
		float(SelectedArea->Right()),
		float(SelectedArea->Top()),
		0);

	MB.SetTexture2D(
		float(SelectedArea->Left()/GL.Width),
		float(SelectedArea->Bottom()/GL.Height)
		);
	MB.AddVertexFloat(
		float(SelectedArea->Left()),
		float(SelectedArea->Bottom()),
		0);

	MB.SetTexture2D(
		float(SelectedArea->Right()/GL.Width),
		float(SelectedArea->Bottom()/GL.Height)
		);

	MB.AddVertexFloat(
		float(SelectedArea->Right()), 
		float(SelectedArea->Bottom()),
		0);

	UnSelect();
	SelectedFrame = new MeshFrame("select");
	SelectedFrame->SetMeshContainer(MB.End());
	CurrentLayer->AddChild(SelectedFrame);
}

/*virtual*/ void OpenGL3DEngine::Highlight(PlutoRectangle* HightlightArea, OpenGLGraphic* HighSurface)
{
#ifdef DISABLE_HIGHLIGHT
	return;
#endif

	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);
	
	if(NULL == CurrentLayer)
		return;
	if(NULL == HightlightArea)
		return;

	MeshTransform Transform;

	MeshBuilder MB;
	MB.SetAlpha(0.4f);
	MB.Begin(MBMODE_TRIANGLE_STRIP);
	MB.SetColor(1.0f, 1.0f, 1.0f);

	MB.SetTexture2D(
		float(HightlightArea->Left())/GL.Width,
		1- float(HightlightArea->Top())/GL.Height
		);
	MB.AddVertexFloat(0, 0, 0);

	MB.SetTexture2D(
		float(HightlightArea->Right())/GL.Width,
		1- float(HightlightArea->Top())/GL.Height
		);
	MB.AddVertexFloat(1, 0, 0);

	MB.SetTexture2D(
		float(HightlightArea->Left())/GL.Width,
		1- float(HightlightArea->Bottom())/GL.Height
		);
	MB.AddVertexFloat(0, 1, 0);
	MB.SetTexture2D(
		float(HightlightArea->Right())/GL.Width,
		1- float(HightlightArea->Bottom())/GL.Height
		);

	MB.AddVertexFloat(1, 1, 0);

	//Transform.ApplyScale(1.2f, 1.2f, 1.2f);
	//Transform.ApplyTranslate(-0.1f, -0.1f, 0.1f);
	Transform.ApplyScale(
		float(HightlightArea->Width),
		float(HightlightArea->Height),
		1.0f);
	Transform.ApplyTranslate(
		float(HightlightArea->Left()),
		float(HightlightArea->Top()),
		0.f);

	MeshContainer* Container = MB.End();
	UnHighlight();

	//LoggerWrapper::GetInstance()->Write(LV_WARNING, "OpenGL3DEngine::Highlight: %d %d %d %d",
	//	HightlightArea->Left(), HightlightArea->Top(), 
	//	HightlightArea->Width, HightlightArea->Height);

	HighLightFrame = new MeshFrame("highlight");

	HighLightFrame->SetTransform(Transform);
	HighLightFrame->SetMeshContainer(Container);

	if(NULL != OriginalCurrentLayer)
		OriginalCurrentLayer->AddChild(HighLightFrame);

	AddTopMostObject("highlight");
}

/*virtual*/ void OpenGL3DEngine::UnHighlight()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);

	if(NULL != HighLightFrame)
	{
		//LoggerWrapper::GetInstance()->Write(LV_WARNING, "OpenGL3DEngine::Unhighlight");
		CurrentLayer->RemoveChild(HighLightFrame, true);
		HighLightFrame->CleanUp();
		delete HighLightFrame;
		HighLightFrame = NULL;
	}
}

/*virtual*/ void OpenGL3DEngine::UnSelect()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);

	if(NULL != SelectedFrame)
	{
		CurrentLayer->RemoveChild(SelectedFrame, true);
		SelectedFrame->CleanUp();
		delete SelectedFrame;
		SelectedFrame = NULL;
	}
}

bool OpenGL3DEngine::NeedUpdateScreen()
{
	return Compose->HasEffects() || HighLightFrame != NULL || m_bNeedToRefresh;
}

MeshFrame* OpenGL3DEngine::GetMeshFrameFromDesktop(string ObjectID)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);

	if(NULL != OriginalCurrentLayer)
		return OriginalCurrentLayer->FindChild(ObjectID);

	return NULL;
}

void OpenGL3DEngine::RemoveMeshFrameFromDesktop(MeshFrame* Frame)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);
	if(NULL == OriginalCurrentLayer)
		return;

	m_bWorldChanged = true;
	OriginalCurrentLayer->RemoveChild(Frame, false);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "RemoveMeshFrameFromDesktop: removed object %p, size is not %d", 
		Frame, OriginalCurrentLayer->Children.size());
#endif
}

void OpenGL3DEngine::StartFrameDrawing(string ObjectHash)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OpenGL3DEngine::StartFrameDrawing!");
#endif

	m_bWorldChanged = true;
	if(NULL != FrameBuilder)
		EndFrameDrawing("");

	FrameBuilder = CurrentLayer;
	CurrentLayer = new MeshFrame(ObjectHash);
}

void OpenGL3DEngine::StartDatagridDrawing(string ObjectHash)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);

	m_bDatagridRendering = true;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OpenGL3DEngine::StartFrameDrawing!");
#endif

	m_bWorldChanged = true;
	if(NULL != FrameDatagrid)
		EndDatagridDrawing("");

	FrameDatagrid = CurrentLayer;
	CurrentLayer = new MeshFrame(ObjectHash);
}

MeshFrame* OpenGL3DEngine::EndDatagridDrawing(string ObjectHash)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "OpenGL3DEngine::EndFrameDrawing!");
#endif

	m_bWorldChanged = true;
	MeshFrame* Result = CurrentLayer;
	CurrentLayer = FrameDatagrid;
	FrameDatagrid = NULL;

	Result->MarkAsVolatileRecursively();

	m_bDatagridRendering = false;
	return Result;
}

void OpenGL3DEngine::InvalidateFrame(string ObjectHash)
{
	MeshFrame *pDatagridFrame = CurrentLayer->FindChild(ObjectHash);

	m_bWorldChanged = true;
	if(NULL != pDatagridFrame)
	{
		MeshFrame *pNewDummyDatagridFrame = new MeshFrame(ObjectHash);
		pNewDummyDatagridFrame->MarkAsVolatile();
		CurrentLayer->ReplaceChild(pDatagridFrame, pNewDummyDatagridFrame);
	}
}

/**
*	Return as result the popup
*/
MeshFrame* OpenGL3DEngine::EndFrameDrawing(string sObjectHash)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "OpenGL3DEngine::EndFrameDrawing!");
#endif
	m_bWorldChanged = true;
	MeshFrame* Result = CurrentLayer;

	CurrentLayer = FrameBuilder;
	if(!CurrentLayer)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Current layer = NULL, that should not happend!");
	}
	else
	{
		AddMeshFrameToDesktop("", Result);
	}

	FrameBuilder = NULL;
	return Result;
}

void OpenGL3DEngine::ShowHighlightRectangle(PlutoRectangle Rect)
{
	return;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);

	MeshFrame * LeftBar = new MeshFrame("highlight-frame-left");
	MeshFrame * TopBar = new MeshFrame("highlight-frame-top");
	MeshFrame * RightBar = new MeshFrame("highlight-frame-right");
	MeshFrame * BottomBar = new MeshFrame("highlight-frame-bottom");
	
	PlutoRectangle Original (Rect);

	Rect.Width = 2;
	LeftBar->SetMeshContainer(
		MeshBuilder::BuildRectangle(Rect, NULL)
		);
	Rect = Original;
	Rect.Height = 2;
	TopBar->SetMeshContainer(
		MeshBuilder::BuildRectangle(Rect, NULL)
		);

	Rect = Original;
	Rect.Y += Rect.Width;
	Rect.Width = 2;
	RightBar->SetMeshContainer(
		MeshBuilder::BuildRectangle(Rect, NULL)
		);

	Rect = Original;
	
	Rect.Y += Rect.Height;
	Rect.Height = 2;
	BottomBar->SetMeshContainer(
		MeshBuilder::BuildRectangle(Rect, NULL)
		);

	HideHighlightRectangle();

	if(HighLightPopup)
	{
		//HighLightPopup->CleanUp();
//		delete HighLightPopup;
		//HighLightPopup = NULL;
	}
	HighLightPopup = new MeshFrame("highlight-frame");
	HighLightPopup->AddChild(LeftBar);
	HighLightPopup->AddChild(TopBar);
	HighLightPopup->AddChild(RightBar);
	HighLightPopup->AddChild(BottomBar);
	HighLightPopup->MarkAsVolatileRecursively();


	//LoggerWrapper::GetInstance()->Write(LV_WARNING, "OpenGL3DEngine::Highlight: %d %d %d %d",
	//	HightlightArea->Left(), HightlightArea->Top(), 
	//	HightlightArea->Width, HightlightArea->Height);


}

void OpenGL3DEngine::HideHighlightRectangle()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);
	m_bWorldChanged = true;

	if(NULL != HighLightFrame)
	{
		//LoggerWrapper::GetInstance()->Write(LV_WARNING, "OpenGL3DEngine::Unhighlight");
		CurrentLayer->RemoveChild(HighLightFrame, true);
		HighLightFrame->CleanUp();
		delete HighLightFrame;
		HighLightFrame = NULL;
	}
}

void OpenGL3DEngine::RemoveMeshFrameFromDesktopForID(string ObjectID)
{
	DumpScene();

	if(ObjectID == "")
		return;

	m_bWorldChanged = true;
	MeshFrame *Frame = CurrentLayer->FindChild(ObjectID);
	if(NULL != Frame)
		RemoveMeshFrameFromDesktop(Frame);

	DumpScene();
}

void OpenGL3DEngine::BeginModifyGeometry()
{
	++ModifyGeometry;
}

void OpenGL3DEngine::EndModifyGeometry()
{
	--ModifyGeometry;
}

void OpenGL3DEngine::AddTopMostObject(string ObjectID)
{
	if(ObjectID== "")
		return;
	if(TopMostObjects.find(ObjectID) != TopMostObjects.end())
		return;

	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);
	TopMostObjects[ObjectID] = ObjectID;
}

void OpenGL3DEngine::RemoveTopMostObject(string ObjectID)
{
	map<string,string>::iterator Item = TopMostObjects.find(ObjectID);
	if(Item != TopMostObjects.end())
		TopMostObjects.erase(Item);
}

void OpenGL3DEngine::UpdateTopMostObjects()
{
	if(TopMostObjects.size() == 0 || NULL == OriginalCurrentLayer)
		return;

	bool bOldWorldChanged = m_bWorldChanged;
	
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);

	map<string,string>::iterator Item;
	for(Item = TopMostObjects.begin(); Item != TopMostObjects.end(); )
	{
		MeshFrame*Frame = this->GetMeshFrameFromDesktop(Item->second);
		if(Frame == NULL)
		{
			map<string,string>::iterator SearchItem = TopMostObjects.find(Item->second);
			if(SearchItem != TopMostObjects.end())
				TopMostObjects.erase(Item++);
		}
		else
		{
			OriginalCurrentLayer->RemoveChild(Frame, true);
			OriginalCurrentLayer->AddChild(Frame);
			++Item;
		}
	}

	m_bWorldChanged = bOldWorldChanged;
}

void OpenGL3DEngine::ReplaceMeshInAnimations(MeshFrame *OldFrame, MeshFrame *NewFrame)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, SceneMutex);

	if(NULL == OriginalCurrentLayer)
		return;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OpenGL3DEngine::ReplaceMeshInAnimations %p/%s with %p/%s",
		OldFrame, OldFrame->Name().c_str(), NewFrame, NewFrame->Name().c_str());

	m_bWorldChanged = true;
	MeshFrame *pOldCloneFrame = OriginalCurrentLayer->FindChild(NewFrame->Name());
	if(NULL != pOldCloneFrame)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "OpenGL3DEngine::ReplaceMeshInAnimations also replacing %p/%s",
			pOldCloneFrame, pOldCloneFrame->Name().c_str());
		m_spDatagridAnimationManager->ReplaceMeshInAnimations(pOldCloneFrame, NewFrame);
	}

	AddMeshFrameToDesktop("", NewFrame);
	RemoveMeshFrameFromDesktop(OldFrame);

	m_spDatagridAnimationManager->ReplaceMeshInAnimations(OldFrame, NewFrame);
}


