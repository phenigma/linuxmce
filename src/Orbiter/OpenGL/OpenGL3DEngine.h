#ifndef OpenGL3DEngine_H_
#define OpenGL3DEngine_H_

#include <map>

#include "ExtensionManager.h"
#include "Texture/GLFontRenderer.h"
#include "Mesh/MeshFrame.h"

#include "PlutoUtils/MultiThreadIncludes.h"

#include "Layers/GL2DEffectLayersCompose.h"

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

	MeshFrame* PopupMode;

	virtual void UnSelect();


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

	void StartPopupDrawing();
	/**
	 *	Return as result the popup
	 */
	MeshFrame* EndPopupMode();
};

#endif
