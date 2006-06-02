#ifndef OPENGLGRAPHIC_H_
#define OPENGLGRAPHIC_H_

#include<string>

#include "TextureManager.h"

class OpenGLGraphic
{
	OpenGLTexture Texture;
public:
	OpenGLGraphic();
	virtual ~OpenGLGraphic();
	
	bool SetupFromImage(std::string FileName);
	
	
};

#endif /*OPENGLGRAPHIC_H_*/
