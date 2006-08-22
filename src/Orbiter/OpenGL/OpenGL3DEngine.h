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
	int StartTick;
	bool AnimationRemain;

	MeshFrame* CurrentLayer;
	MeshFrame* HighlightCurrentLayer; //alias for CurrentLayer
	MeshFrame* OldLayer;
	MeshFrame* SelectedFrame;
	MeshFrame* HighLightFrame;
	MeshFrame* HighLightPopup;
	MeshFrame* FrameBuilder, *FrameDatagrid;

	virtual void UnSelect();

	std::vector<AnimationScrollDatagrid*> AnimationDatagrid;
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
	
	void AddMeshFrameToDesktop(string ParentObjectID, MeshFrame* Frame);
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

	void CubeAnimateDatagridFrames(string ObjectID, MeshFrame *BeforeGrid, MeshFrame *AfterGrid,
		int MilisecondTime, int Direction, float fMaxAlphaLevel);

	void ShowHighlightRectangle(PlutoRectangle Rect);
	void HideHighlightRectangle();

	bool IsCubeAnimatedDatagrid(string ObjectID);


};

#endif
