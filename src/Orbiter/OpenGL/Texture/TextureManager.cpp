#include "TextureManager.h"
#include "../ExtensionManager.h"
#include "../Mesh/MeshFrame.h"

#include <GL/gl.h>

#include "../OpenGLGraphic.h"

#include "../OpenGL3DEngine.h"


#include "DCE/Logger.h"
//using namespace DCE;

/*static*/ TextureManager* TextureManager::Instance_ = NULL;

TextureManager* TextureManager::Instance() 
{
	if(Instance_ == NULL)
		Instance_ = new TextureManager();
	return Instance_;
}

TextureManager::TextureManager(void) 
	: LastTexture(0), 
	TextureEnable_(false),
	SupportTextureNonPowerOfTwo_(-1),
	Engine(NULL),
	TextureLock("texture mutex"),
	ReleaseTextureSuspended(0)
{
	TextureLock.Init(NULL);
}

TextureManager::~TextureManager(void)
{
	pthread_mutex_destroy(&TextureLock.mutex);
}

void TextureManager::Setup(OpenGL3DEngine *Engine)
{
	this->Engine = Engine;
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

	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, TextureLock);
	WaitForConvert.push_back(TextureGraphic);
}

void TextureManager::RemoveFromConvertQueue(OpenGLGraphic* TextureGraphic)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, TextureLock);
	WaitForConvert.remove(TextureGraphic);
}

void TextureManager::PrepareRelease(OpenGLTexture TextureGraphic)
{
	if(!TextureGraphic)
		return;

	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, TextureLock);
	WaitForRelease.push_back(TextureGraphic);
}

void TextureManager::ConvertImagesToTextures()
{
	//if(WaitForConvert.size())
	//	g_pPlutoLogger->Write(LV_CRITICAL, "TextureManager::ConvertImagesToTextures size %d", WaitForConvert.size());
	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, TextureLock);
	std::list <OpenGLGraphic*>::iterator Item, End = WaitForConvert.end();
	for(Item = WaitForConvert.begin(); Item != End; ++Item)
	{
		if (*Item)
			(*Item)->Convert();
	}
	WaitForConvert.clear();
}

void TextureManager::ReleaseTextures()
{
	if(ReleaseTextureSuspended > 0)
		return;

	if(WaitForRelease.size())
		DCE::g_pPlutoLogger->Write(LV_CRITICAL, "TextureManager::ReleaseTextures size %d", WaitForRelease.size());

	PLUTO_SAFETY_LOCK_ERRORSONLY(sm, TextureLock);
	std::list <OpenGLTexture>::iterator Item, End = WaitForRelease.end();
	for(Item = WaitForRelease.begin(); Item != End; ++Item)
	{
		if (*Item)
		{
			for(std::list <OpenGLGraphic*>::iterator it = WaitForConvert.begin(), 
				end = WaitForConvert.end();	it != end; ++it)
			{
				//we'll handle the release
				if((*it)->Texture == *Item)
					(*it)->Texture = 0;
			}

			glDeleteTextures(1, &(*Item));
		}
	}
	WaitForRelease.clear();
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


MeshFrame* TextureManager::GetCacheItem(std::string ObjectHash)
{
	std::map<std::string, MeshFrame*>::iterator Item = Graphics.find(ObjectHash);

	if(Item == Graphics.end())
		return NULL;

	
	return Item->second;
}

void TextureManager::AddCacheItem(std::string ObjectHash, MeshFrame* Frame)
{
	if(ExistInCache(ObjectHash))
		return;
	Graphics[ObjectHash] = Frame;
}

bool TextureManager::ExistInCache(std::string ObjectHash)
{
	bool Result = Graphics.find(ObjectHash) != Graphics.end();
	return Result;
}

bool TextureManager::ExistInCache(MeshFrame *pFrame)
{
	for(std::map<std::string, MeshFrame*>::const_iterator it = Graphics.begin(),
		end = Graphics.end(); it != end; ++it)
	{
		if(it->second == pFrame)
			return true;
	}

	return false;
}

void TextureManager::AttachToScene(string ParentObjectID, MeshFrame* Frame)
{
	if(NULL != Engine)
	{
		MeshFrame *pFrameFromDesktop = Engine->AddMeshFrameToDesktop(ParentObjectID, Frame);
		if(NULL != pFrameFromDesktop && pFrameFromDesktop != Frame)
			Graphics[pFrameFromDesktop->Name()]	= pFrameFromDesktop;
	}
}