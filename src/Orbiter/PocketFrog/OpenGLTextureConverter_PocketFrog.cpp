#include "OpenGLTextureConverter_PocketFrog.h"
#include "Orbiter_PocketFrog.h"

//open gl
#include <GL/gl.h>

//pocketfrog
#include <PocketFrog.h>
using namespace Frog;
using namespace Frog::Internal;
#include <src/internal/graphicbuffer.h>
#include <src/rasterizer.h>
#include <src/internal/raster2d.h>

OpenGLTextureConverter_PocketFrog::OpenGLTextureConverter_PocketFrog(PlutoGraphic *pGraphic) 
	: OpenGLTextureConverter(pGraphic)
{
}

OpenGLTextureConverter_PocketFrog::~OpenGLTextureConverter_PocketFrog()
{
}

OpenGLTexture OpenGLTextureConverter_PocketFrog::Convert()
{
	DisplayDevice *pDisplayDevice = Orbiter_PocketFrog::GetInstance()->GetOrbiterDisplay();
	Surface *pSourceSurface = dynamic_cast<PocketFrogGraphic *>(m_spPlutoGraphic.get())->m_pSurface;

	auto_ptr<Surface> spTextureSurface(
		pDisplayDevice->CreateSurface(
			MathUtils::MinPowerOf2(pSourceSurface->GetWidth()), 
			MathUtils::MinPowerOf2(pSourceSurface->GetHeight())
		)
	);
	auto_ptr<Rasterizer> spRasterizer(pDisplayDevice->CreateRasterizer(spTextureSurface.get()));
	spRasterizer->Blit(Point(0, 0), pSourceSurface);

	OpenGLTexture TextureID;
	glGenTextures(1, &TextureID);

	//Typical Texture Generation Using Data From The Bitmap
	glBindTexture(GL_TEXTURE_2D, TextureID);

	int nPixelsNumber = spTextureSurface->GetWidth() * spTextureSurface->GetHeight();
	GLubyte *pGLData = new GLubyte[nPixelsNumber * 3];
	memset(pGLData, 0, nPixelsNumber * 3);

	Pixel *pPixel = (Pixel *)spTextureSurface->m_buffer->GetPixels();
	GLubyte *pDestPixel = pGLData;
	for(int i = 0; i < nPixelsNumber; ++i, ++pPixel, pDestPixel += 3)
	{
		*(pDestPixel)     = (*pPixel & RED_MASK)   >> (RED_SHIFT - (8 - RED_BITS));
		*(pDestPixel + 1) = (*pPixel & GREEN_MASK) >> (GREEN_SHIFT - (8 - GREEN_BITS));
		*(pDestPixel + 2) = (*pPixel & BLUE_MASK)  << (8 - BLUE_BITS); 
	}
	//todo : convert 16 -> 24

	//Generate The Texture
	glTexImage2D(GL_TEXTURE_2D, 
		0, 3, 
		spTextureSurface->GetWidth(), spTextureSurface->GetHeight(), 
		0, GL_RGB,
		GL_UNSIGNED_BYTE, 
		pGLData);

	delete []pGLData;

	//Typical Texture Generation Using Data From The Bitmap
	glBindTexture(GL_TEXTURE_2D, TextureID);

	//Linear Filtering
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	return TextureID; 
}
