#ifndef GLRenderScreenToGraphic_H_
#define GLRenderScreenToGraphic_H_

#include "../Mesh/MeshFrame.h"

namespace GLEffect2D 
{

class GLRenderScreenToGraphic
{
	int Width, Height;
	MeshFrame* RenderFrame;
	OpenGLGraphic* RenderGraphic;
public:
	GLRenderScreenToGraphic(int Width, int Height);
	virtual ~GLRenderScreenToGraphic(void);

	void SetRenderFrame(MeshFrame* RenderFrame);
	MeshFrame* GetRenderFrame();
	void CreateRenderTexture();
	OpenGLGraphic* GetRenderGraphic();
	void RenderFrameToGraphic(MeshTransform& Transform);
};

}

#endif

