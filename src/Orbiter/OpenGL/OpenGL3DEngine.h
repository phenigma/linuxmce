#ifndef OpenGL3DEngine_H_
#define OpenGL3DEngine_H_

#include <map>

#include "ExtensionManager.h"
#include "Texture/GLFontRenderer.h"
#include "Mesh/MeshFrame.h"

#include "PlutoUtils/MultiThreadIncludes.h"

#include "Layers/GL2DEffectLayersCompose.h"

class AnimationScrollDatagrid;

class OpenGL3DEngine
{
	pluto_pthread_mutex_t SceneMutex;
	MeshFrame Desktop;
	int StartTick;
	bool AnimationRemain;

	MeshFrame* CurrentLayer;
	MeshFrame* HighlightCurrentLayer; //alias for CurrentLayer
	MeshFrame* OldLayer;
	MeshFrame* SelectedFrame;
	MeshFrame* HighLightFrame;
	MeshFrame* HighLightPopup;

	MeshFrame* FrameBuilder, *FrameDatagrid;

	std::string CurrentLayerName;

	virtual void UnSelect();

	std::vector<AnimationScrollDatagrid*> AnimationDatagrid;


	std::map<string, std::pair<MeshFrame *, std::string> > CurrentLayerObjects_;

	void ShowAnimationTextures();

	void DumpScene();

public:
	GLEffect2D::LayersCompose* Compose;
	bool Quit;
	ExtensionManager GL;
	OpenGL3DEngine(void);
	virtual ~OpenGL3DEngine();
	virtual void Finalize(void);

	bool NeedUpdateScreen();
	
	void AddMeshFrameToDesktop(string ParentObjectID, string ObjectID, MeshFrame* Frame);
	void RemoveMeshFrameFromDesktopFromID(std::string ObjectID);
	MeshFrame* GetMeshFrameFromDesktop(string ObjectID);
	
	virtual void NewScreen();
	virtual bool Paint();

	virtual void Select(PlutoRectangle* SelectedArea);
	virtual void Highlight(PlutoRectangle* HightlightArea, OpenGLGraphic* HighSurface);

	virtual void UnHighlight();

	void RemoveMeshFrameFromDesktop(MeshFrame* Frame);

	virtual int GetTick();
	void Setup();

	/**
	 *	Store the MeshFrame which keeps the Datagrid	
	 */
	void StartDatagridDrawing();
	MeshFrame* EndDatagridDrawing();

	/**
	 *	Store the current frame, for instance a popup
	 */
	void StartFrameDrawing(std::string ObjectHash);
	MeshFrame* EndFrameDrawing(std::string sObjectHash);

	void CubeAnimateDatagridFrames(string ObjectID, MeshFrame *BeforeGrid, MeshFrame *AfterGrid,
		int MilisecondTime, int Direction, float fMaxAlphaLevel);

	void ShowHighlightRectangle(PlutoRectangle Rect);
	void HideHighlightRectangle();


};

#endif
