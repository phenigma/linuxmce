#include "GLRenderScreenToGraphic.h"
#include "../Texture/TextureManager.h"

#include "../GLMathUtils.h"

namespace GLEffect2D 
{

GLRenderScreenToGraphic::GLRenderScreenToGraphic(int Width, int Height)
: RenderGraphic(NULL)
{
	this->Width = Width;
	this->Height = Height;
	CreateRenderTexture();
}

GLRenderScreenToGraphic::~GLRenderScreenToGraphic(void)
{
	delete RenderGraphic;
}

OpenGLGraphic* GLRenderScreenToGraphic::GetRenderGraphic()
{
	return this->RenderGraphic;
}

void GLRenderScreenToGraphic::RenderFrameToGraphic()
{
	glBindTexture(GL_TEXTURE_2D, RenderGraphic->Texture);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, 
		MathUtils::MinPowerOf2(Width),
		MathUtils::MinPowerOf2(Height),
		0); 
}

void GLRenderScreenToGraphic::CreateRenderTexture()
{
	if(RenderGraphic)
		return;
	RenderGraphic = new OpenGLGraphic(Width, Height);
	TextureManager::Instance()->PrepareConvert(RenderGraphic);
}

}