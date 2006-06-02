#include "OpenGLTextureConverter.h"

#ifdef POCKETFROG
	#include "../PocketFrog/OpenGLTextureConverter_PocketFrog.h"
#else
	#include "../SDL/OpenGLTextureConverter_SDL.h"
#endif 

OpenGLTexture OpenGLTextureConverter::GenerateTexture(PlutoGraphic *pGraphic)
{
	auto_ptr<OpenGLTextureConverter> converter( 
#ifdef POCKETFROG
		new OpenGLTextureConverter_PocketFrog(pGraphic)
#else
		new OpenGLTextureConverter_SDL(pGraphic)
#endif 		
		);
	return converter->Convert();
}

OpenGLTextureConverter::OpenGLTextureConverter(PlutoGraphic *pGraphic)
{
	m_spPlutoGraphic.reset(pGraphic);
}
	
OpenGLTextureConverter::~OpenGLTextureConverter()
{
	m_spPlutoGraphic.release();
}
