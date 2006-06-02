#include "TextureManager.h"

#include <GL/gl.h>

#include "../../SDL/SDLGraphic.h"

#include "OpenGLGraphic.h"

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
	NewScreen = new OpenGLGraphic();
	OldScreen = new OpenGLGraphic();
}

TextureManager::~TextureManager(void)
{
	delete NewScreen;
	NewScreen = NULL;
	delete OldScreen;
	OldScreen = NULL;
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

void TextureManager::ConfigureNextScreen(SDL_Surface* NextScreen)
{
	NewScreen->ReleaseTexture();
	NewScreen->Convert(NextScreen);
	SetupTexture(NewScreen->Texture);
}
