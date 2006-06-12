#include "TextureManager.h"

#include <GL/gl.h>

#include "OpenGLGraphic.h"

/*static*/ TextureManager* TextureManager::_Instance = NULL;

TextureManager* TextureManager::Instance() 
{
	if(_Instance == NULL)
		_Instance = new TextureManager();
	return _Instance;
}

TextureManager::TextureManager(void)
	: LastTexture(0), TextureEnable_(false)
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
	
	if(0 != Texture)
	{
		if(!TextureEnable_)
		{
			glEnable(GL_TEXTURE_2D);
			TextureEnable_ = true;
		}
		
		glBindTexture(GL_TEXTURE_2D, Texture);
	}
	else
	{
		if(TextureEnable_)
		{
			glDisable(GL_TEXTURE_2D);
			TextureEnable_ = false;
		}
	}
	
	glBegin(GL_TRIANGLES);
	LastTexture = Texture;
}

void TextureManager::PrepareImage(OpenGLGraphic* TextureGraphic)
{
	if(!TextureGraphic)
		return;
	WaitForConvert.push(TextureGraphic);
}

void TextureManager::ConvertImagesToTextures()
{
	OpenGLGraphic* Item;
	while (!WaitForConvert.empty())
	{
		Item = WaitForConvert.front();
		WaitForConvert.pop();
		if (Item)
			Item->Convert();
	};
}

