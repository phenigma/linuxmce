#include "GraphicImage.h"

#include "SDL_image.h"
#include "MathUtils.h"

#include "DCE/Logger.h"
using namespace DCE;

GraphicImage::GraphicImage()
: LocalSurface(NULL), Texture(0)
{

}
GraphicImage::~GraphicImage()
{
	if(LocalSurface)
		SDL_FreeSurface(LocalSurface);
	ReleaseTexture();
}

bool GraphicImage::Load(string FileName)
{
	LocalSurface = IMG_Load(FileName.c_str()); 
	if( LocalSurface )
	{
		if( LocalSurface->w * LocalSurface->h > 3240000 )  // 1800x1800 is a reasonable max size
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Aborting load of %s because size w: %d h: %d is too big",FileName.c_str(),LocalSurface->w,LocalSurface->h);
			SDL_FreeSurface(LocalSurface);
			LocalSurface=NULL;
		}
		else
			g_pPlutoLogger->Write(LV_STATUS,"Loaded %s w: %d h: %d",FileName.c_str(),LocalSurface->w,LocalSurface->h);
	}
	else
		g_pPlutoLogger->Write(LV_CRITICAL,"Failed to load %s",FileName.c_str());
	return NULL != LocalSurface;
}

void GraphicImage::Prepare(int nScreenWidth, int nScreenHeight)
{
	if(SupportTextureNonPowerOfTwo())
	{
		MaxU = 1;
		MaxV = 1;
	}
	else
		ScaleImage(nScreenWidth, nScreenHeight);

	Convert();
}

void GraphicImage::Convert(void)
{
	if(LocalSurface == NULL)
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

	SDL_FreeSurface(LocalSurface);
	LocalSurface = NULL;

	/* Linear Filtering */
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); 
}

void GraphicImage::ReleaseTexture(void)
{
	glDeleteTextures(1, &Texture);
	Texture = 0;
}

void GraphicImage::ScaleImage(int nScreenWidth, int nScreenHeight)
{
	if(LocalSurface == NULL)
		return;

	SDL_Surface* Surface = LocalSurface;
	Width = MinPowerOf2(Surface->w);
	Height = MinPowerOf2(Surface->h);

	//TODO: use screen dim. to create a surface with optimal size

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
	//SDL_SetAlpha(Surface, SDL_SRCALPHA | SDL_RLEACCEL , SDL_ALPHA_TRANSPARENT);

	SDL_BlitSurface(Surface, NULL, LocalSurface, NULL);
	SDL_FreeSurface(Surface);  
}

bool GraphicImage::CheckExtension(const char* checkFor)
{
	const GLubyte* extensions = glGetString(GL_EXTENSIONS);
	string sExtensions(reinterpret_cast<const char *>(extensions));
	return sExtensions.find(checkFor) != string::npos;
}

bool GraphicImage::SupportTextureNonPowerOfTwo()
{
	return CheckExtension("ARB_texture_rectangle") || CheckExtension("ARB_texture_non_power_of_two");
}

