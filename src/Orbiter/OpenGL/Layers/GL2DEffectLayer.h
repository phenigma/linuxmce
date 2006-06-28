#ifndef GL2DEffectLayer_H_
#define GL2DEffectLayer_H_

#include "../Effects/gl2deffectfactory.h"

#include "../Mesh/MeshFrame.h"
#include "../OpenGLGraphic.h"

namespace GLEffect2D 
{

class Layer
{
	int Width, Height;
	EffectFactory* Effects;
	DrawingWidgetsEngine* Widgets;

	MeshFrame* RenderFrame;

	void CreateRenderTexture();
public:
	OpenGLGraphic* RenderGraphic;

	Layer(int Width, int Height);
	virtual ~Layer(void);

	virtual void Paint();

	Effect* CreateEffect(int IDEffect, int StartAfter, int TimeForComplete);
	TBaseWidget* CreateWidget(int WidgetType, int Top, int Left, int Width, int Height, char* Text);

	bool HasEffects();

	
	void Resize(int Width, int Height);

	/**
	 *	Render-To-Texture a Render MeshFrame	
	 */
	void SetRenderFrame(MeshFrame* RenderFrame);
	MeshFrame* GetRenderFrame();

	OpenGLGraphic* GetRenderGraphic();

	void RenderFrameToGraphic(MeshTransform& Transform);
};

}

#endif
