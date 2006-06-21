#include "GLRenderScreenToGraphic.h"

namespace GLEffect2D 
{

GLRenderScreenToGraphic::GLRenderScreenToGraphic(int Width, int Height)
: RenderFrame(NULL)
{
	this->Width = Width;
	this->Height = Height;
	CreateRenderTexture();
}

GLRenderScreenToGraphic::~GLRenderScreenToGraphic(void)
{
}

void GLRenderScreenToGraphic::SetRenderFrame(MeshFrame* RenderFrame)
{
	this->RenderFrame = RenderFrame;
}

MeshFrame* GLRenderScreenToGraphic::GetRenderFrame()
{
	return RenderFrame;
}

OpenGLGraphic* GLRenderScreenToGraphic::GetRenderGraphic()
{
	if(!RenderFrame)
		return NULL;

	return NULL;
}

void GLRenderScreenToGraphic::RenderFrameToGraphic(MeshTransform& Transform)
{
	if(!RenderFrame)
		return;

	//Transform.ApplyTranslate(-Width/2.0f, -Height/2.0f, 0);
	RenderFrame->Paint(Transform);
}

void GLRenderScreenToGraphic::CreateRenderTexture()
{
	if(!RenderFrame)
	{
		//if is nothing to render, there is no need for RenderToTexture
		//there is no need to create a render texture
		return;
	}
	if(!RenderGraphic)
		RenderGraphic = new OpenGLGraphic(Width, Height);

	//RenderFrame->Paint();	

	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, Width, Height, 0); 
}

}
