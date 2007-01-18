/*
Orbiter

Copyright (C) 2006 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

@author: "Ciprian Mustiata" ciprian.m at plutohome dot com, "Cristian Miron" chris.m at plutohome dot com 

*/
#include "OpenGLGraphic.h"

#include <SDL_image.h>

#include "GLMathUtils.h"
#include "DCE/Logger.h"
#include "../OrbiterRenderer.h"
#include "../Orbiter.h"
#include "SDL_Helpers/SDL_Defs.h"

#include <iostream>

OpenGLGraphic::OpenGLGraphic() : 
	PlutoGraphic(), 
	m_OpenGlMutex("OpenGLGraphicMutex")
{
	Initialize();
}

OpenGLGraphic::OpenGLGraphic(OrbiterRenderer *pOrbiterRenderer) : 
	PlutoGraphic(pOrbiterRenderer), 
	m_OpenGlMutex("OpenGLGraphicMutex")
{
	Initialize();
}

OpenGLGraphic::OpenGLGraphic(string Filename, eGraphicManagement GraphicManagement, OrbiterRenderer *pOrbiterRenderer) : 
	PlutoGraphic(Filename, GraphicManagement, pOrbiterRenderer), 
	m_OpenGlMutex("OpenGLGraphicMutex")
{
	Initialize();
}
//Warning: That function must run in OpenGL thread
OpenGLGraphic::OpenGLGraphic(int Width, int Height) : 
	PlutoGraphic(), 
	m_OpenGlMutex("OpenGLGraphicMutex")
{
	Initialize();

	int MPWidth = MathUtils::MinPowerOf2(Width);
	int MPHeight = MathUtils::MinPowerOf2(Height);
	MaxU = float(Width) / MPWidth;
	MaxV = float(Height) / MPHeight;
	this->Width = Width;
	this->Height = Height;

	LocalSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, MPWidth, MPHeight, 
		32, 
		rmask, gmask, bmask, amask);
	Prepare();
}

OpenGLGraphic::OpenGLGraphic(SDL_Surface *pSurface) : PlutoGraphic(), 
	m_OpenGlMutex("OpenGLGraphicMutex"), MaxU(1.0f), MaxV(1.0f)
{
	Initialize();
	LocalSurface = pSurface;

	Width = pSurface->w;
	Height = pSurface->h;

	Prepare();
}

OpenGLGraphic::~OpenGLGraphic()
{
	
	PLUTO_SAFETY_LOCK(oglMutex, m_OpenGlMutex);

	if(NULL != LocalSurface)
	{
		SDL_FreeSurface(LocalSurface);
		LocalSurface = NULL;
	}

	oglMutex.Release();

	TextureManager::Instance()->RemoveFromConvertQueue(this);
	TextureManager::Instance()->PrepareRelease(Texture);	

	pthread_mutex_destroy(&m_OpenGlMutex.mutex);

#if defined(DETECT_LEAKS)
	LeaksDetector::Instance().DestroyedGraphicObject();
#endif
}

void OpenGLGraphic::Initialize()
{
#if defined(DETECT_LEAKS)
	LeaksDetector::Instance().NewGraphicObject();
#endif

	pthread_mutexattr_t m_MutexAttr;
	pthread_mutexattr_init(&m_MutexAttr);
	pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
	m_OpenGlMutex.Init(&m_MutexAttr);
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
	PLUTO_SAFETY_LOCK(oglMutex, m_OpenGlMutex);
	SDL_Surface *LocalSurface = IMG_Load(FileName.c_str());

	Prepare();
	
	return true;
}


void OpenGLGraphic::Prepare()
{    
	PLUTO_SAFETY_LOCK(oglMutex, m_OpenGlMutex);
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

	Uint32 saved_flags = Surface->flags&(SDL_SRCALPHA|SDL_RLEACCELOK);
	Uint8  saved_alpha = Surface->format->alpha;

	if ((saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA) 
		SDL_SetAlpha(Surface, 0, 0);
	
	SDL_BlitSurface(Surface, NULL, LocalSurface, NULL);

	if ((saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA) 
		SDL_SetAlpha(LocalSurface, saved_flags, saved_alpha);

	SDL_FreeSurface(Surface); 
}

void OpenGLGraphic::Convert()
{
	PLUTO_SAFETY_LOCK(oglMutex, m_OpenGlMutex);

	if( NULL == LocalSurface || NULL == LocalSurface->format)
		return;

	ReleaseTexture();

	/* Typical Texture Generation Using Data From The Bitmap */
	glGenTextures( 1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);

	if(LocalSurface->format->BytesPerPixel == 4)
	{
		// Generate The Texture 
		glTexImage2D( GL_TEXTURE_2D, 
		0, 4, 
		LocalSurface->w, LocalSurface->h, 
		0, GL_RGBA,
		GL_UNSIGNED_BYTE, 
		LocalSurface->pixels );
	}
	else if(LocalSurface->format->BytesPerPixel == 3)
	{
		// Generate The Texture 
		glTexImage2D( GL_TEXTURE_2D, 
			0, 3, 
			LocalSurface->w, LocalSurface->h, 
			0, GL_RGB,
			GL_UNSIGNED_BYTE, 
			LocalSurface->pixels );
	}
	else 
	{
		SDL_PixelFormat RGBAFormat;
		RGBAFormat.palette = 0; RGBAFormat.colorkey = 0; RGBAFormat.alpha = 255;
		RGBAFormat.BitsPerPixel = 24; RGBAFormat.BytesPerPixel = 3;
		RGBAFormat.Rloss = 0; RGBAFormat.Gloss = 0; RGBAFormat.Bloss = 0; RGBAFormat.Aloss = 0; 
		RGBAFormat.Rshift = 0; RGBAFormat.Gshift = 8; RGBAFormat.Bshift = 16; RGBAFormat.Ashift = 24; 
		RGBAFormat.Rmask = 0; RGBAFormat.Gmask = 1 << 8; RGBAFormat.Bmask = 1 << 16; RGBAFormat.Amask = 1 << 24; 

		SDL_Surface *LocalSurfaceConverted = SDL_ConvertSurface(LocalSurface, &RGBAFormat, SDL_SWSURFACE);
		SDL_SetAlpha(LocalSurfaceConverted, 0, 0);

		// Generate The Texture 
		glTexImage2D( GL_TEXTURE_2D, 
			0, LocalSurfaceConverted->format->BytesPerPixel, 
			LocalSurfaceConverted->w, LocalSurface->h, 
			0, GL_RGB,
			GL_UNSIGNED_BYTE, 
			LocalSurfaceConverted->pixels );   

		SDL_FreeSurface(LocalSurface);
		LocalSurface = LocalSurfaceConverted;
	}

#ifdef DEBUG
	DCE::g_pPlutoLogger->Write(LV_STATUS, "Freeing surface %p" , LocalSurface);
#endif
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
	return LocalSurface == NULL && Texture == 0;
}

//-----------------------------------------------------------------------------------------------------
Uint32 OpenGLGraphic::getpixel(SDL_Surface *pSDL_Surface,int x, int y)
{
    // all pixels outside the pSDL_Surface are black
    if (x < 0 || x >= pSDL_Surface->w || y < 0 || y >= pSDL_Surface->h)
        return SDL_MapRGB(pSDL_Surface->format, 0, 0, 0);

    int bpp = pSDL_Surface->format->BytesPerPixel;
    Uint8 * pixel = (Uint8 *) pSDL_Surface->pixels + y * pSDL_Surface->pitch + x * bpp;

    switch(bpp)
    {
    case 1:
        return * pixel;

    case 2:
        return * (Uint16 *) pixel;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return pixel[0] << 16 | pixel[1] << 8 | pixel[2];
        else
            return pixel[0] | pixel[1] << 8 | pixel[2] << 16;

    case 4:
        return * (Uint32 *) pixel;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}
//-----------------------------------------------------------------------------------------------------
void OpenGLGraphic::putpixel(SDL_Surface *pSDL_Surface,int x, int y, Uint32 pixel_color)
{
    // don't try to put a pixel outside the pSDL_Surface
    if (x < 0 || x >= pSDL_Surface->w || y < 0 || y >= pSDL_Surface->h)
        return;

    int bpp = pSDL_Surface->format->BytesPerPixel;
    Uint8 * pixel = (Uint8 *) pSDL_Surface->pixels + y * pSDL_Surface->pitch + x * bpp;

    switch(bpp)
    {
    case 1:
        * pixel = pixel_color;
        break;

    case 2:
        * (Uint16 *) pixel = pixel_color;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
        {
            pixel[0] = (pixel_color >> 16) & 0xff;
            pixel[1] = (pixel_color >> 8) & 0xff;
            pixel[2] = pixel_color & 0xff;
        }
        else
        {
            pixel[0] = pixel_color & 0xff;
            pixel[1] = (pixel_color >> 8) & 0xff;
            pixel[2] = (pixel_color >> 16) & 0xff;
        }
        break;

    case 4:
        * (Uint32 *) pixel = pixel_color | 0xFF000000; //opaque
        break;
    }
}

bool OpenGLGraphic::LoadGraphic(char *pData, size_t iSize,int iRotation)
{
g_PlutoProfiler->Start("OpenGLGraphic::LoadGraphic");
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
	{
g_PlutoProfiler->Stop("OpenGLGraphic::LoadGraphic");
		return false;
	}

	Width = LocalSurface->w;
	Height = LocalSurface->h;
	Prepare();
g_PlutoProfiler->Stop("OpenGLGraphic::LoadGraphic");

	return LocalSurface != NULL;
}

void OpenGLGraphic::Clear()
{
	if(Texture)
	{
		TextureManager::Instance()->PrepareRelease(Texture);
		Texture = 0;
	}

	PLUTO_SAFETY_LOCK(oglMutex, m_OpenGlMutex);
	if(LocalSurface != NULL)
	{
		SDL_FreeSurface(LocalSurface);
		LocalSurface = NULL;
	}
}

bool OpenGLGraphic::GetInMemoryBitmap(char*& pRawBitmapData, size_t& ulSize)
{
	PLUTO_SAFETY_LOCK(oglMutex, m_OpenGlMutex);

	pRawBitmapData = NULL;
	ulSize = 0;
	return false;
}

void OpenGLGraphic::ReleaseTexture()
{
	PLUTO_SAFETY_LOCK(oglMutex, m_OpenGlMutex);

	if(Texture)
	{
		glDeleteTextures(1, &Texture);
		Texture = 0;
	}
}

OpenGLGraphic* OpenGLGraphic::ReplaceColorInRectangle(PlutoRectangle Area, PlutoColor ColorToReplace, PlutoColor ReplacementColor)
{
	PLUTO_SAFETY_LOCK(oglMutex, m_OpenGlMutex);

	if(!LocalSurface)
		return NULL;

	//PLUTO_SAFETY_LOCK(SceneMutex, LockMutex);
	
	SDL_PixelFormat * PF = LocalSurface->format;
	Uint32 PlutoPixelDest, PlutoPixelSrc, Pixel;

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS, "ReplaceColor: %u %u %u : %u %u %u",
		ColorToReplace.R(), ColorToReplace.G(), ColorToReplace.B(),
		ReplacementColor.R(), ReplacementColor.G(), ReplacementColor.B());
#endif

	int x = Area.Left();
	int y = Area.Top();
	int width = Area.Width;
	int height = Area.Height;


	OpenGLGraphic* Result = new OpenGLGraphic(Area.Width, Area.Height);
	SDL_SetAlpha(Result->LocalSurface, 0, 0);

	SDL_Rect SDLArea;
	SDLArea.x = Area.X;
	SDLArea.y = Area.Y;
	SDLArea.w = Area.Width;
	SDLArea.h = Area.Height;

	SDL_BlitSurface(LocalSurface, &SDLArea, Result->LocalSurface, NULL);

	PlutoPixelSrc = (ColorToReplace.R() << PF->Rshift) | (ColorToReplace.G() << PF->Gshift) | (ColorToReplace.B() << PF->Bshift) | (ColorToReplace.A() << PF->Ashift);
	unsigned char *Source = (unsigned char *) &PlutoPixelSrc;
	PlutoPixelDest = ReplacementColor.R() << PF->Rshift | ReplacementColor.G() << PF->Gshift | ReplacementColor.B() << PF->Bshift;//  TODO -- this should work | ReplacementColor.A() << PF->Ashift;

	Source[3] = 0;
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			// we may need locking on the surface
			Pixel = getpixel(Result->LocalSurface,i, j);
			unsigned char *pPixel = (unsigned char *) &Pixel;
			const int max_diff = 3;
			if ( abs(Source[0]-pPixel[0])<max_diff && abs(Source[1]-pPixel[1])<max_diff && abs(Source[2]-pPixel[2])<max_diff && abs(Source[3]-pPixel[3])<max_diff )
			{
				pPixel[3]= 255;
				putpixel(Result->LocalSurface,i, j, PlutoPixelDest);
			}

		}
	}

	oglMutex.Release();
	TextureManager::Instance()->PrepareConvert(Result);
	return Result;
}

#ifdef DETECT_LEAKS

LeaksDetector LeaksDetector::m_Instance;

LeaksDetector::~LeaksDetector()
{
	FileLogger *pLeaksDetectorLogger = new FileLogger("/var/log/pluto/orbiter_detected_leaks.log");

	pLeaksDetectorLogger->Write(LV_STATUS, "Dumping leaks:");

	pLeaksDetectorLogger->Write(LV_STATUS, "Mesh frames: %d", m_listMeshFrames.size());
	for(list<string>::iterator it = m_listMeshFrames.begin(); it != m_listMeshFrames.end(); ++it)
		pLeaksDetectorLogger->Write(LV_STATUS, "\t- mesh frame: %s", (*it).c_str());

	pLeaksDetectorLogger->Write(LV_STATUS, "Clones: %d", m_listClones.size());
	for(list<string>::iterator it = m_listClones.begin(); it != m_listClones.end(); ++it)
		pLeaksDetectorLogger->Write(LV_STATUS, "\t- clones: %s", (*it).c_str());

	pLeaksDetectorLogger->Write(LV_STATUS, "Graphic objects/textures: %d", m_nGraphicObjectsCounter);

	delete pLeaksDetectorLogger;
	pLeaksDetectorLogger = NULL;
}
void LeaksDetector::NewMeshFrame(string sName)
{
	m_listMeshFrames.push_back(sName);
}
void LeaksDetector::NewClone(string sName)
{
	m_listClones.push_back(sName);
}
void LeaksDetector::NewGraphicObject()
{
	++m_nGraphicObjectsCounter;
}
void LeaksDetector::DestroyedMeshFrame(string sName)
{
	list<string>::iterator it = find(m_listMeshFrames.begin(), m_listMeshFrames.end(), sName);
	if(it != m_listMeshFrames.end())
		m_listMeshFrames.erase(it);
}
void LeaksDetector::DestroyedClone(string sName)
{
	list<string>::iterator it = find(m_listClones.begin(), m_listClones.end(), sName);
	if(it != m_listClones.end())
		m_listClones.erase(it);
}
void LeaksDetector::DestroyedGraphicObject()
{
	--m_nGraphicObjectsCounter;
}
#endif