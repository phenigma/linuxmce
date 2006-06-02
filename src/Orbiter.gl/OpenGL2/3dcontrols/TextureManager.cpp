#include "TextureManager.h"

#include <GL/gl.h>

TextureManager::TextureManager(void)
{
}

TextureManager::~TextureManager(void)
{
}

/*static*/ void TextureManager::SetupTexture(OpenGLTexture Texture)
{
	glEnd();
	glBindTexture(GL_TEXTURE_2D, Texture);
	glBegin(GL_TRIANGLES);		
}

