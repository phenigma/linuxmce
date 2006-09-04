#ifndef OpenGL3DEngine_H_
#define OpenGL3DEngine_H_

#include <map>

#include "ExtensionManager.h"
#include "Texture/GLFontRenderer.h"
#include "Mesh/MeshFrame.h"

#include "PlutoUtils/MultiThreadIncludes.h"
#include "Layers/GL2DEffectLayersCompose.h"

class AnimationScrollDatagrid;
class DatagridAnimationManager;

class OpenGL3DEngine
{
	int StartTick;
	bool AnimationRemain;
	pluto_pthread_mutex_t SceneMutex;

	MeshFrame* CurrentLayer;
	MeshFrame* HighlightCurrentLayer; //alias for CurrentLayer
	MeshFrame* OldLayer;
	MeshFrame* SelectedFrame;
	MeshFrame* HighLightFrame;
	MeshFrame* HighLightPopup;
	MeshFrame* FrameBuilder, *FrameDatagrid;

	virtual void UnSelect();

	map<string, string> TopMostObjects;
	void UpdateTopMostObjects();

	void ShowAnimationTextures();
	void DumpScene();

	auto_ptr<DatagridAnimationManager> m_spDatagridAnimationManager;

	/**
	 *	Block painting because there is a need of bigger geometry painting
	 */
	int ModifyGeometry;

public:
	bool ForceReleaseTextures;

	GLEffect2D::LayersCompose* Compose;
	bool Quit;
	ExtensionManager GL;
	OpenGL3DEngine(void);
	virtual ~OpenGL3DEngine();
	virtual void Finalize(void);

	bool NeedUpdateScreen();
	/**
	 *	returns the MeshFrame that keeps the name of the target frame in the scene 	
	 */
	MeshFrame* AddMeshFrameToDesktop(string ParentObjectID, MeshFrame* Frame);
	void RemoveMeshFrameFromDesktopForID(std::string ObjectID);
	void RemoveMeshFrameFromDesktop(MeshFrame* Frame);
	MeshFrame* GetMeshFrameFromDesktop(string ObjectID);
	
	virtual void NewScreen(string Name);
	virtual bool Paint();

	virtual void Select(PlutoRectangle* SelectedArea);
	virtual void Highlight(PlutoRectangle* HightlightArea, OpenGLGraphic* HighSurface);

	virtual void UnHighlight();
	virtual int GetTick();
	void Setup();

	/**
	 *	Store the MeshFrame which keeps the Datagrid	
	 */
	void StartDatagridDrawing(std::string ObjectHash);
	MeshFrame* EndDatagridDrawing(std::string ObjectHash);

	/**
	 *	Store the current frame, for instance a popup
	 */
	void StartFrameDrawing(std::string ObjectHash);
	MeshFrame* EndFrameDrawing(std::string sObjectHash);

	void ShowHighlightRectangle(PlutoRectangle Rect);
	void HideHighlightRectangle();

	void BeginModifyGeometry();
	void EndModifyGeometry();

	void AddTopMostObject(string ObjectID);
	void RemoveTopMostObject(string ObjectID);

	void ReplaceMeshInAnimations(MeshFrame *OldFrame, MeshFrame *NewFrame);

	DatagridAnimationManager *GetDatagridAnimationManager() { return m_spDatagridAnimationManager.get(); }

	pluto_pthread_mutex_t& GetSceneMutex() { return SceneMutex; }
};

#endif
