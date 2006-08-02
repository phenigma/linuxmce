#include "TextureManager.h"

#include <GL/gl.h>
#include <GL/glext.h>

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

bool TextureManager::CheckExtension(const GLubyte* extensions, const char* checkFor)
{
	//char *endOfStr;	
	string sExtensions(reinterpret_cast<const char *>(extensions));
	return sExtensions.find(checkFor) != string::npos;
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
			CheckExtension(glGetString(GL_EXTENSIONS), "ARB_texture_rectangle") || 
			CheckExtension(glGetString(GL_EXTENSIONS), "ARB_texture_non_power_of_two");
	}

	return SupportTextureNonPowerOfTwo_ > 0;
}