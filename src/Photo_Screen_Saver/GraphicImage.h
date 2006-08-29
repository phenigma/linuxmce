#ifndef GraphicImage_H_
#define GraphicImage_H_

#include <SDL.h>
#ifdef WIN32
#define LEAN_AND_MEAN
#include <windows.h>
#endif
#include <GL/gl.h>
#include <string>

using namespace std;

class GraphicImage
{
	SDL_Surface *LocalSurface;
public:
	GraphicImage(void);
	~GraphicImage(void);
	bool Load(string FileName);
	void Convert(void);
public:
	GLuint Texture;
	float MaxU;
	float MaxV;
	int Width;
	int Height;
public:
	void ReleaseTexture(void);
	void ScaleImage(void);
};

#endif
