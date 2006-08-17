#include "OpenGL3DEngine.h"

#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL_ttf.h>

#include "Mesh/MeshPainter.h"
#include "Layers/GL2DEffectLayersCompose.h"
#include "Texture/TextureManager.h"
#include "Texture/GLFontManager.h"
#include "AnimationScrollDatagrid.h"

#include "Simulator.h"

#include "DCE/Logger.h"
using namespace DCE;

//#define SCENE_DEBUG 1

OpenGL3DEngine::OpenGL3DEngine()
	: OldLayer(NULL),
	CurrentLayer(NULL),
	Quit(false),
	HighLightFrame(NULL),
	SelectedFrame(NULL),
	AnimationRemain (false),
	SceneMutex("scene mutex"),
	Compose(NULL),
	FrameBuilder(NULL),
	FrameDatagrid(NULL),
	AnimationDatagrid(NULL),
	HighLightPopup(NULL)
{
	if(TTF_Init()==-1) {
		printf("Error on TTF_Init: %s\n", TTF_GetError());
		return;
	}

	pthread_mutexattr_t m_SceneAttr;
	pthread_mutexattr_init(&m_SceneAttr);
	pthread_mutexattr_settype(&m_SceneAttr, PTHREAD_MUTEX_RECURSIVE_NP);
	SceneMutex.Init(&m_SceneAttr);
	pthread_mutexattr_destroy(&m_SceneAttr);

	TextureManager::Instance()->Setup(this);
}

OpenGL3DEngine::~OpenGL3DEngine()
{
	//TODO: shared objects between old and current layer? let's debug this
	//if(OldLayer)
	//	OldLayer->CleanUp();
	
	if(CurrentLayer)
		CurrentLayer->CleanUp();

	delete OldLayer;
	delete CurrentLayer;
	pthread_mutex_destroy(&SceneMutex.mutex);
}

void OpenGL3DEngine::Finalize(void)
{
	MeshPainter::Instance()->CleanUp();
	GLEffect2D::LayersCompose::Instance()->CleanUp();
	GLFontManager::GetInstance()->CleanUp();
	TextureManager::Instance()->CleanUp();

	CurrentLayerObjects_.clear();
	TTF_Quit();

	SDL_Quit();
}

bool OpenGL3DEngine::Paint()
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);
	if(NULL == CurrentLayer)
		return false;

	if(Compose->HasEffects())
		AnimationRemain = true;
	
	TextureManager::Instance()->ReleaseTextures();
	TextureManager::Instance()->ConvertImagesToTextures();

	if(CurrentLayer->Children.size() == 0)
	{
	    //g_pPlutoLogger->Write(LV_CRITICAL, "NOTHING to paint (current layer: %p)", CurrentLayer);		
		return false;
	}

	GL.SetClearColor(.0f, .0f, 0.0f);
	GL.ClearScreen(true, false);
		
	GL.EnableZBuffer(false);

	Compose->Paint();

	//g_pPlutoLogger->Write(LV_WARNING, "OpenGL3DEngine::Paint before highlight");
	if(AnimationDatagrid.size())
	{
		std::vector<AnimationScrollDatagrid*>::iterator Item ;
		for(Item = AnimationDatagrid.begin(); Item != AnimationDatagrid.end(); )
		{
			AnimationScrollDatagrid *pThing = *Item;
			if (pThing->Update())
			{
				RemoveMeshFrameFromDesktop(pThing->BeforeGrid);
				pThing->BeforeGrid->CleanUp();
				delete pThing;
				Item = AnimationDatagrid.erase(Item);
			}
			else
				++Item;
		}
	}
	else
	{
		if(HighLightPopup)
		{
			//g_pPlutoLogger->Write(LV_WARNING, "OpenGL3DEngine::Paint after highlight");
			Point3D Color;
			Color.X = 1.0f;
			Color.Y = 1.0f;
			Color.Z = (GetTick() / 2 % 512) / 255.0f*Simulator::GetInstance()->m_iMilisecondsHighLight/300;
			Color.Z = abs(Color.Z - 1.0f)/2.0f+ 0.5f;
			Color.X = Color.Z;
			Color.Y = Color.Z;

			CurrentLayer->RemoveChild(HighLightPopup);
			CurrentLayer->AddChild(HighLightPopup);
			HighLightPopup->SetColor(Color);
		}

		if(HighLightFrame)
		{
			//g_pPlutoLogger->Write(LV_WARNING, "OpenGL3DEngine::Paint after highlight");
			Point3D Color;
			Color.X = 1.0f;
			Color.Y = 1.0f;
			Color.Z = (GetTick() / 2 % 512) / 255.0f*Simulator::GetInstance()->m_iMilisecondsHighLight/300;
			Color.Z = abs(Color.Z - 1.0f)/2.0f+ 0.5f;
			Color.X = Color.Z;
			Color.Y = Color.Z;
			HighlightCurrentLayer->RemoveChild(HighLightFrame);
			HighlightCurrentLayer->AddChild(HighLightFrame);
			HighLightFrame->SetColor(Color);
		}
	}
	//glEnable(GL_CULL_FACE);

	GL.Flip();


	bool Status = NULL != Compose && Compose->HasEffects() && AnimationRemain;
	if (Compose->HasEffects() == false)
		AnimationRemain = false;

	return Status;
}

/*virtual*/ int OpenGL3DEngine::GetTick()
{
	return SDL_GetTicks();
}

void OpenGL3DEngine::NewScreen()
{
	UnHighlight();
	UnSelect();

	PLUTO_SAFETY_LOCK(sm, SceneMutex);
	
	if(NULL != OldLayer)
	{
		Desktop.RemoveChild(OldLayer);
		//OldLayer->CleanUp();
		delete OldLayer;
	}
	OldLayer = CurrentLayer;
	
	StartTick = GetTick();

	CurrentLayerObjects_.clear();
	CurrentLayer = new MeshFrame();
	HighlightCurrentLayer = CurrentLayer;

#ifdef SCENE_DEBUG
	MeshTransform transform;
	transform.ApplyRotateY(5.0f);
	CurrentLayer->ApplyTransform(transform);
#endif
	
	if(NULL != Compose)
		Compose->UpdateLayers(CurrentLayer, OldLayer);
	
	Desktop.AddChild(CurrentLayer);
	
	Desktop.RemoveChild(OldLayer);
	Desktop.AddChild(OldLayer);

	//g_pPlutoLogger->Write(LV_CRITICAL, "Current layer is now %p, size %d", 
	//	CurrentLayer, Desktop.Children.size());
}

void OpenGL3DEngine::Setup()
{
	GL.Setup();
	Compose = GLEffect2D::LayersCompose::Instance();
	Compose->Setup(GL.Width, GL.Height);
	CurrentLayer = new MeshFrame();
}

void OpenGL3DEngine::AddMeshFrameToDesktop(string ParentObjectID, string ObjectID, MeshFrame* Frame)
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);

	DumpScene();

	if(NULL == CurrentLayer)
	{
		Frame->CleanUp();
		delete Frame;
		return;
	}

	g_pPlutoLogger->Write(LV_WARNING, "sss Adding child object %s to parent %s, frame %p, size is now %d", 
		ObjectID.c_str(), ParentObjectID.c_str(), Frame, CurrentLayer->Children.size());

	MeshFrame* ParentFrame = CurrentLayer;
	string InternalParentID = ParentObjectID;

	if(ObjectID != "")
	{
		std::map<string, std::pair<MeshFrame *, std::string> >::iterator it = CurrentLayerObjects_.find(ObjectID);
		if(it != CurrentLayerObjects_.end())
		{
			InternalParentID = it->second.second;
			MeshFrame* OldFrame = it->second.first;

			if(InternalParentID != "")
			{
				std::map<string, std::pair<MeshFrame *, std::string> >::iterator it_parent = CurrentLayerObjects_.find(InternalParentID);
				if(it_parent != CurrentLayerObjects_.end())
					ParentFrame = it_parent->second.first;
			}

			g_pPlutoLogger->Write(LV_CRITICAL, "sss Replacing child %p, object %s of layer %p", 
				OldFrame, ObjectID.c_str(), ParentFrame);

			ParentFrame->RemoveChild(OldFrame);

			DumpScene();
		}
	}

#ifdef SCENE_DEBUG
	static float bubu = 0;

	MeshTransform trans;
	trans.Translate(0.0f, 0.0f, bubu);
	bubu = bubu + 1.0f;
	Frame->ApplyTransform(trans);
#endif

	if(InternalParentID == "")
		InternalParentID = CurrentLayerName;

	ParentFrame->AddChild(Frame);
	if(ObjectID != "")
	{
		CurrentLayerObjects_[ObjectID] = std::make_pair(Frame, InternalParentID);
	}

	g_pPlutoLogger->Write(LV_WARNING, "sss Added child %p, object %s to layer %p, main current layer size is now %d", 
		Frame, ObjectID.c_str(), ParentFrame, CurrentLayer->Children.size());

	DumpScene();
}

inline void OpenGL3DEngine::DumpScene()
{
#ifdef SCENE_DEBUG
	g_pPlutoLogger->Write(LV_WARNING, "DUMPING SCENE: current layer %p, size %d",
		CurrentLayer, CurrentLayer->Children.size());

	for(std::map<string, std::pair<MeshFrame *, std::string> >::iterator it = CurrentLayerObjects_.begin();
		it != CurrentLayerObjects_.end();++it)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Object %s, mesh %p with parent %s",
			it->first.c_str(), it->second.first, it->second.second.c_str());
	}
#endif
}

/*virtual*/ void OpenGL3DEngine::Select(PlutoRectangle* SelectedArea)
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);
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
	SelectedFrame = new MeshFrame();
	SelectedFrame->SetMeshContainer(MB.End());
	CurrentLayer->AddChild(SelectedFrame);
}

/*virtual*/ void OpenGL3DEngine::Highlight(PlutoRectangle* HightlightArea, OpenGLGraphic* HighSurface)
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);
	
	if(NULL == CurrentLayer)
		return;
	if(NULL == HightlightArea)
		return;

	Compose->UpdateLayers(CurrentLayer, OldLayer);
	MeshTransform Transform;
	//g_pPlutoLogger->Write(LV_WARNING, "OpenGL3DEngine::Highlight-Step2");
	//Compose->PaintScreen3D();
	//Compose->NewScreen->RenderFrameToGraphic();
	//MB.SetTexture(Compose->NewScreen->GetRenderGraphic());
	//g_pPlutoLogger->Write(LV_WARNING, "OpenGL3DEngine::Highlight-Step3");


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

	//g_pPlutoLogger->Write(LV_WARNING, "OpenGL3DEngine::Highlight: %d %d %d %d",
	//	HightlightArea->Left(), HightlightArea->Top(), 
	//	HightlightArea->Width, HightlightArea->Height);

	HighLightFrame = new MeshFrame();

	HighLightFrame->SetTransform(Transform);
	HighLightFrame->SetMeshContainer(Container);
}

/*virtual*/ void OpenGL3DEngine::UnHighlight()
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);

	if(NULL != HighLightFrame)
	{
		//g_pPlutoLogger->Write(LV_WARNING, "OpenGL3DEngine::Unhighlight");
		CurrentLayer->RemoveChild(HighLightFrame);
		HighLightFrame->CleanUp();

		delete HighLightFrame;
		HighLightFrame = NULL;
	}
}

/*virtual*/ void OpenGL3DEngine::UnSelect()
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);

	if(NULL != SelectedFrame)
	{
		CurrentLayer->RemoveChild(SelectedFrame);
		SelectedFrame->CleanUp();

		delete SelectedFrame;
		SelectedFrame = NULL;
	}
}

bool OpenGL3DEngine::NeedUpdateScreen()
{
	return Compose->HasEffects() || HighLightFrame != NULL;
}

MeshFrame* OpenGL3DEngine::GetMeshFrameFromDesktop(string ObjectID)
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);
	std::map<string, std::pair<MeshFrame *, std::string> >::iterator it = CurrentLayerObjects_.find(ObjectID);
	if(it == CurrentLayerObjects_.end())
		return NULL;
	return it->second.first;
}

void OpenGL3DEngine::RemoveMeshFrameFromDesktop(MeshFrame* Frame)
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);
	if(NULL == CurrentLayer)
	{
		//g_pPlutoLogger->Write(LV_CRITICAL, "RemoveMeshFrameFromDesktop: NULL CurrentLayer");
		return;
	}

	CurrentLayer->RemoveChild(Frame);

	g_pPlutoLogger->Write(LV_STATUS, "RemoveMeshFrameFromDesktop: removed object %p, size is not %d", 
		Frame, CurrentLayer->Children.size());
}

void OpenGL3DEngine::StartFrameDrawing(std::string ObjectHash)
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);
	g_pPlutoLogger->Write(LV_WARNING, "OpenGL3DEngine::StartFrameDrawing!");
	if(NULL != FrameBuilder)
		EndFrameDrawing("");
	FrameBuilder = CurrentLayer;
	CurrentLayer = new MeshFrame();
	CurrentLayerName = ObjectHash;
}

void OpenGL3DEngine::StartDatagridDrawing()
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);
	g_pPlutoLogger->Write(LV_WARNING, "OpenGL3DEngine::StartFrameDrawing!");
	if(NULL != FrameDatagrid)
		EndDatagridDrawing();
	FrameDatagrid = CurrentLayer;
	CurrentLayer = new MeshFrame();
}

MeshFrame* OpenGL3DEngine::EndDatagridDrawing()
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);

	g_pPlutoLogger->Write(LV_WARNING, "OpenGL3DEngine::EndFrameDrawing!");

	MeshFrame* Result = CurrentLayer;
	CurrentLayer = FrameDatagrid;
	if(!CurrentLayer)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Current layer = NULL, that should not happend!");
	}
	else
		CurrentLayer->AddChild(Result);

	FrameDatagrid = NULL;
	return Result;
}

/**
*	Return as result the popup
*/
MeshFrame* OpenGL3DEngine::EndFrameDrawing(string sObjectHash)
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);

	g_pPlutoLogger->Write(LV_WARNING, "OpenGL3DEngine::EndFrameDrawing!");

	MeshFrame* Result = CurrentLayer;

	CurrentLayer = FrameBuilder;
	CurrentLayerName = "";

	if(!CurrentLayer)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Current layer = NULL, that should not happend!");
	}
	else
	{
		AddMeshFrameToDesktop("", sObjectHash, Result);
	}

	FrameBuilder = NULL;
	return Result;
}

void OpenGL3DEngine::CubeAnimateDatagridFrames(string ObjectID, MeshFrame *BeforeGrid, MeshFrame *AfterGrid,
		int MilisecondTime, int Direction, float fMaxAlphaLevel)
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);

	AnimationScrollDatagrid* Animation = new AnimationScrollDatagrid("", this, BeforeGrid, AfterGrid, MilisecondTime, Direction, fMaxAlphaLevel); 
	AnimationDatagrid.push_back(Animation);

	Animation->StartAnimation();
}

void OpenGL3DEngine::ShowHighlightRectangle(PlutoRectangle Rect)
{
	return;
	PLUTO_SAFETY_LOCK(sm, SceneMutex);

	MeshFrame * LeftBar = new MeshFrame();
	MeshFrame * TopBar = new MeshFrame();
	MeshFrame * RightBar = new MeshFrame();
	MeshFrame * BottomBar = new MeshFrame();
	
	PlutoRectangle Original (Rect);

	Rect.Width = 2;
	LeftBar->SetMeshContainer(
		MeshBuilder::BuildRectangle(&Rect, NULL)
		);
	Rect = Original;
	Rect.Height = 2;
	TopBar->SetMeshContainer(
		MeshBuilder::BuildRectangle(&Rect, NULL)
		);

	Rect = Original;
	Rect.Y += Rect.Width;
	Rect.Width = 2;
	RightBar->SetMeshContainer(
		MeshBuilder::BuildRectangle(&Rect, NULL)
		);

	Rect = Original;
	
	Rect.Y += Rect.Height;
	Rect.Height = 2;
	BottomBar->SetMeshContainer(
		MeshBuilder::BuildRectangle(&Rect, NULL)
		);

	HideHighlightRectangle();

	if(HighLightPopup)
	{
		//HighLightPopup->CleanUp();
//		delete HighLightPopup;
		//HighLightPopup = NULL;
	}
	HighLightPopup = new MeshFrame();
	HighLightPopup->AddChild(LeftBar);
	HighLightPopup->AddChild(TopBar);
	HighLightPopup->AddChild(RightBar);
	HighLightPopup->AddChild(BottomBar);


	//g_pPlutoLogger->Write(LV_WARNING, "OpenGL3DEngine::Highlight: %d %d %d %d",
	//	HightlightArea->Left(), HightlightArea->Top(), 
	//	HightlightArea->Width, HightlightArea->Height);


}

void OpenGL3DEngine::HideHighlightRectangle()
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);

	if(NULL != HighLightFrame)
	{
		//g_pPlutoLogger->Write(LV_WARNING, "OpenGL3DEngine::Unhighlight");
		CurrentLayer->RemoveChild(HighLightFrame);
		HighLightFrame->CleanUp();

		delete HighLightFrame;
		HighLightFrame = NULL;
	}
}

void OpenGL3DEngine::RemoveMeshFrameFromDesktopFromID(std::string ObjectID)
{
	DumpScene();

	if(ObjectID == "")
		return;

	for(std::map<string, std::pair<MeshFrame *, std::string> >::iterator it = CurrentLayerObjects_.begin();
		it != CurrentLayerObjects_.end();)
	{
		string ObjectHash = it->first;

		if(ObjectHash.find(ObjectID) != string::npos && ObjectHash.find(ObjectID + ".") == string::npos)
		{
			MeshFrame* pFrame = it->second.first;
			RemoveMeshFrameFromDesktop(pFrame);

			it = CurrentLayerObjects_.erase(it);

			g_pPlutoLogger->Write(LV_CRITICAL, "Removed object %s, frame %p, size is now %d", 
				ObjectHash.c_str(), pFrame, CurrentLayerObjects_.size());
		}
		else
			++it;
	}

	DumpScene();
}
