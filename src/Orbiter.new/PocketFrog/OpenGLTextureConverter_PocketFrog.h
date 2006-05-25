#pragma once

#include "../OpenGL/OpenGLTextureConverter.h"
#include "../OpenGL/math3dutils.h"

class OpenGLTextureConverter_PocketFrog : public OpenGLTextureConverter
{
public:
	OpenGLTextureConverter_PocketFrog(PlutoGraphic *pGraphic);
	~OpenGLTextureConverter_PocketFrog();

	OpenGLTexture Convert();
};
