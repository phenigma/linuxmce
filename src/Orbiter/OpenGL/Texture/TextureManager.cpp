#include "TextureManager.h"
#include "../ExtensionManager.h"

#include <GL/gl.h>

#include "../OpenGLGraphic.h"

#include "DCE/Logger.h"
using namespace DCE;

/*static*/ TextureManager* TextureManager::Instance_ = NULL;

TextureManager* TextureManager::Instance() 
{
	if(Instance_ == NULL)
		Instance_ = new TextureManager();
	return Instance_;
}

TextureManager::TextureManager(void)
	: LastTexture(0), TextureEnable_(false), SupportTextureNonPowerOfTwo_(-1)
{
}

TextureManager::~TextureManager(void)
{
}

/*static*/ void TextureManager::SetupTexture(OpenGLTexture Texture)
{
	//if(Texture == LastTexture)
	//	return;

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

void TextureManager::CleanUp()
{
	delete Instance_;
	Instance_ = NULL;
}

void TextureManager::PrepareConvert(OpenGLGraphic* TextureGraphic)
{
	if(!TextureGraphic)
		return;
	WaitForConvert.push(TextureGraphic);
}

void TextureManager::PrepareRelease(OpenGLGraphic* TextureGraphic)
{
	if(!TextureGraphic)
		return;
	WaitForRelease.push(TextureGraphic);
}

void TextureManager::ConvertImagesToTextures()
{
	if(WaitForConvert.size())
		g_pPlutoLogger->Write(LV_CRITICAL, "TextureManager::ConvertImagesToTextures size %d", WaitForConvert.size());
	
	OpenGLGraphic* Item;
	while (!WaitForConvert.empty())
	{
		Item = WaitForConvert.front();
		WaitForConvert.pop();
		if (Item)
			Item->Convert();
	};
}

void TextureManager::ReleaseTextures()
{
	if(WaitForRelease.size())
		g_pPlutoLogger->Write(LV_CRITICAL, "TextureManager::ReleaseTextures size %d", WaitForRelease.size());

	OpenGLGraphic* Item;
	while (!WaitForRelease.empty())
	{
		Item = WaitForRelease.front();
		WaitForRelease.pop();
		if (Item)
			Item->ReleaseTexture();
	};
}

bool TextureManager::SupportTextureNonPowerOfTwo()
{
	if(SupportTextureNonPowerOfTwo_ == -1)
	{
		SupportTextureNonPowerOfTwo_ = 
			ExtensionManager::CheckExtension("ARB_texture_rectangle") || 
			ExtensionManager::CheckExtension("ARB_texture_non_power_of_two");
	}

	return SupportTextureNonPowerOfTwo_ > 0;
}

