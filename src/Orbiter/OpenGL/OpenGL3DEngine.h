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
	MeshFrame* OldLayer;
	MeshFrame* SelectedFrame;
	MeshFrame* HighLightFrame;

	MeshFrame* FrameBuilder, *FrameDatagrid;

	virtual void UnSelect();

	std::vector<AnimationScrollDatagrid*> AnimationDatagrid;


	std::map<string, MeshFrame *> CurrentLayerObjects_;

	void ShowAnimationTextures();

public:
	GLEffect2D::LayersCompose* Compose;
	bool Quit;
	ExtensionManager GL;
	OpenGL3DEngine(void);
	virtual ~OpenGL3DEngine();
	virtual void Finalize(void);

	bool NeedUpdateScreen();
	
	void AddMeshFrameToDesktop(string ObjectID, MeshFrame* Frame);
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
	void StartFrameDrawing();
	MeshFrame* EndFrameDrawing();

	void CubeAnimateDatagridFrames(MeshFrame *BeforeGrid, MeshFrame *AfterGrid,
		int MilisecondTime, int Direction);


};

#endif
