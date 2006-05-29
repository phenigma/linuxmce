#include "TextureManager.h"

#include <GL/gl.h>

/*static*/ TextureManager* TextureManager::_Instance = NULL;

TextureManager* TextureManager::Instance()
{
	if(_Instance == NULL)
		_Instance = new TextureManager();
	return _Instance;
}

TextureManager::TextureManager(void)
	: LastTexture(0)
{
}

TextureManager::~TextureManager(void)
{
}

/*static*/ void TextureManager::SetupTexture(OpenGLTexture Texture)
{
	if(Texture == LastTexture)
		return;
	glEnd();
	glBindTexture(GL_TEXTURE_2D, Texture);
	glBegin(GL_TRIANGLES);
	LastTexture = Texture;
}

