#ifndef GLDEFS_H
#define GLDEFS_H

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <WINDOWS.H>
#endif

#include <GL/gl.h>

struct FloatColor 
{
	float Red, Green, Blue, Alpha;

	FloatColor(float ValueRed, float ValueGreen, float ValueBlue, 
		float ValueAlpha)
		: Red(ValueRed),
		Green(ValueGreen),
		Blue(ValueBlue),
		Alpha(ValueAlpha)
	{

	}
};

typedef struct FloatColor ColorRGB;  
typedef GLuint OpenGLTexture; 

#endif
