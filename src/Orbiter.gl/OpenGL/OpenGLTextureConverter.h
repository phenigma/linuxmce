#ifndef OpenGLTextureConverter_H
#define OpenGLTextureConverter_H

#include "../DesignObj_Orbiter.h"
#include "GLDefs.h"

#include <GL/gl.h>

class OpenGLTextureConverter
{
public:
	OpenGLTextureConverter(PlutoGraphic *pGraphic);
	
	virtual ~OpenGLTextureConverter();
	virtual OpenGLTexture Convert() = 0;

	static OpenGLTexture GenerateTexture(PlutoGraphic *pGraphic);

protected:
	auto_ptr<PlutoGraphic> m_spPlutoGraphic;
};

#endif
