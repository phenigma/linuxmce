#ifndef GLRenderScreenToGraphic_H_
#define GLRenderScreenToGraphic_H_

#include "../Mesh/MeshFrame.h"

namespace GLEffect2D 
{

class GLRenderScreenToGraphic
{
	int Width, Height;
	OpenGLGraphic* RenderGraphic;
public:
	GLRenderScreenToGraphic(int Width, int Height);
	virtual ~GLRenderScreenToGraphic(void);

	void CreateRenderTexture();
	OpenGLGraphic* GetRenderGraphic();
	void RenderFrameToGraphic();
};

}

#endif

