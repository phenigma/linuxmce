#include "OpenGLGraphic.h"

#include <SDL_image.h>

#include "GLMathUtils.h"
#include "DCE/Logger.h"
#include "../OrbiterRenderer.h"
#include "../Orbiter.h"

#include <iostream>

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
static Uint32 rmask = 0xff000000;
static Uint32 gmask = 0x00ff0000;
static Uint32 bmask = 0x0000ff00;
static Uint32 amask = 0x000000ff;
#else
static Uint32 rmask = 0x000000ff;
static Uint32 gmask = 0x0000ff00;
static Uint32 bmask = 0x00ff0000;
static Uint32 amask = 0xff000000;
#endif 


OpenGLGraphic::OpenGLGraphic() : 
	PlutoGraphic(), 
	Mutex("OpenGLGraphicMutex")
{
	Initialize();
}

OpenGLGraphic::OpenGLGraphic(OrbiterRenderer *pOrbiterRenderer) : 
	PlutoGraphic(pOrbiterRenderer), 
	Mutex("OpenGLGraphicMutex")
{
	Initialize();
}

OpenGLGraphic::OpenGLGraphic(string Filename, eGraphicManagement GraphicManagement, OrbiterRenderer *pOrbiterRenderer) : 
	PlutoGraphic(Filename, GraphicManagement, pOrbiterRenderer), 
	Mutex("OpenGLGraphicMutex")
{
	Initialize();
}
//Warning: That function must run in OpenGL thread
OpenGLGraphic::OpenGLGraphic(int Width, int Height) : 
	PlutoGraphic(), 
	Mutex("OpenGLGraphicMutex")
{
	Initialize();

	int NewWidth = GLMathUtils::MinPowerOf2(Width);
	MaxU = float(Width) / NewWidth;
	this->Width = NewWidth;
	int NewHeight = GLMathUtils::MinPowerOf2(Width);
	MaxU = float(Width) / NewHeight ;
	this->Height = NewHeight;

	LocalSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, Width, Height, 
		32, 
		rmask, gmask, bmask, amask);
	Prepare();
}

OpenGLGraphic::~OpenGLGraphic()
{
	
	PLUTO_SAFETY_LOCK(oglMutex, Mutex);

	if(NULL != LocalSurface)
	{
		SDL_FreeSurface(LocalSurface);
		LocalSurface = NULL;
	}

	oglMutex.Release();

	TextureManager::Instance()->RemoveFromConvertQueue(this);
	TextureManager::Instance()->PrepareRelease(Texture);	

	pthread_mutex_destroy(&Mutex.mutex);

}

void OpenGLGraphic::Initialize()
{
	pthread_mutexattr_t m_MutexAttr;
	pthread_mutexattr_init(&m_MutexAttr);
	pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
	Mutex.Init(&m_MutexAttr);
	pthread_mutexattr_destroy(&m_MutexAttr);


	MaxU = 1.0f;
	MaxV = 1.0f;
	LocalSurface = NULL;
	Texture = 0;
	Width = 0;
	Height = 0;
}

bool OpenGLGraphic::SetupFromImage(std::string FileName)
{
	PLUTO_SAFETY_LOCK(oglMutex, Mutex);
	SDL_Surface *LocalSurface = IMG_Load(FileName.c_str());

	Prepare();
	
	return true;
}


void OpenGLGraphic::Prepare()
{    
	PLUTO_SAFETY_LOCK(oglMutex, Mutex);
	if(LocalSurface == NULL)
		return;

	if(TextureManager::Instance()->SupportTextureNonPowerOfTwo())
		return;

	SDL_Surface* Surface = LocalSurface;
	Width = GLMathUtils::MinPowerOf2(Surface->w);
	Height = GLMathUtils::MinPowerOf2(Surface->h);

	if (Width == Surface->w && Height == Surface->h)
		return;
	
	MaxU = ((float)Surface->w)/Width;
	MaxV = ((float)Surface->h)/Height;
	
	/* Create a 32-bit surface with the bytes of each pixel in R,G,B,A order,
	as expected by OpenGL for textures */
	
	LocalSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, Width, Height, 
		Surface->format->BitsPerPixel, 
		Surface->format->Rmask, Surface->format->Gmask, Surface->format->Bmask,
		Surface->format->Amask);
		
	if(LocalSurface == NULL) {
		fprintf(stderr, "CreateRGBSurface failed: %s", SDL_GetError());
		return;
	}
	
	//TODO: check if LocalSurface is transparent after blit
	SDL_SetAlpha(Surface, SDL_SRCALPHA | SDL_RLEACCEL , SDL_ALPHA_TRANSPARENT);

	SDL_BlitSurface(Surface, NULL, LocalSurface, NULL);

	SDL_FreeSurface(Surface); 
}

void OpenGLGraphic::Convert()
{
	PLUTO_SAFETY_LOCK(oglMutex, Mutex);

	if( NULL == LocalSurface || NULL == LocalSurface->format)
		return;

	ReleaseTexture();

	/* Typical Texture Generation Using Data From The Bitmap */
	glGenTextures( 1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);

	if(LocalSurface->format->BytesPerPixel == 4)
		// Generate The Texture 
		glTexImage2D( GL_TEXTURE_2D, 
		0, 4, 
		LocalSurface->w, LocalSurface->h, 
		0, GL_RGBA,
		GL_UNSIGNED_BYTE, 
		LocalSurface->pixels );
	else
		// Generate The Texture 
		glTexImage2D( GL_TEXTURE_2D, 
			0, 3, 
			LocalSurface->w, LocalSurface->h, 
			0, GL_RGB,
			GL_UNSIGNED_BYTE, 
			LocalSurface->pixels );

	DCE::g_pPlutoLogger->Write(LV_STATUS, "Freeing surface %p" , LocalSurface);

	SDL_FreeSurface(LocalSurface);
	LocalSurface = NULL;
		
	/* Linear Filtering */
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

PlutoGraphic* OpenGLGraphic::Clone()
{
	return NULL;
}

GraphicType OpenGLGraphic::GraphicType_get()
{
	return gtOpenGLGraphic;
}

bool OpenGLGraphic::IsEmpty()
{
	return LocalSurface == NULL;
}

bool OpenGLGraphic::LoadGraphic(char *pData, size_t iSize,int iRotation)
{
	Clear();

	if(m_GraphicFormat == GR_OCG)
	{
		string sErrorMessage = 
			"Cannot load OCG files in Orbiter. "
			"Please uncheck 'Use OCG' device data for this device (" + 
			StringUtils::ltos(m_pOrbiterRenderer->OrbiterLogic()->m_dwPK_Device) + ").";

		g_pPlutoLogger->Write(LV_CRITICAL, sErrorMessage.c_str());
		m_pOrbiterRenderer->PromptUser(sErrorMessage.c_str(), 100);
		exit(1);
		return false;
	}
	else
	{
		SDL_RWops * rw = SDL_RWFromMem(pData, int(iSize));
		LocalSurface = IMG_Load_RW(rw, 1); // rw is freed here
	}

	if(!LocalSurface)
		return false;

	Width = LocalSurface->w;
	Height = LocalSurface->h;
	Prepare();

	return LocalSurface != NULL;
}

void OpenGLGraphic::Clear()
{
	PLUTO_SAFETY_LOCK(oglMutex, Mutex);

	TextureManager::Instance()->PrepareRelease(Texture);
	if(LocalSurface != NULL)
	{
		SDL_FreeSurface(LocalSurface);
		LocalSurface = NULL;
	}
}

bool OpenGLGraphic::GetInMemoryBitmap(char*& pRawBitmapData, size_t& ulSize)
{
	PLUTO_SAFETY_LOCK(oglMutex, Mutex);

	pRawBitmapData = NULL;
	ulSize = 0;
	return false;
}

void OpenGLGraphic::ReleaseTexture()
{
	PLUTO_SAFETY_LOCK(oglMutex, Mutex);

	if (TextureManager::Instance() == NULL)
		return;

	
	if(Texture)
	{
		glDeleteTextures(1, &Texture);
		Texture = 0;
	}
}
