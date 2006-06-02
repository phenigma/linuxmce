#include <iostream>
#include <SDL.h>

#include "OpenGL3DEngine.h"
#include "MeshBuilder.h"
#include "MeshFrame.h"

#include <SDL_image.h>

#include "OpenGLGraphic.h"

#ifndef WIN32
#include <unistd.h>
#endif

#include "GLFontTextureList.h"

#define DEFAULT_FONT_NAME "/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf"
//#define DEFAULT_FONT_NAME "/usr/share/fonts/truetype/msttcorefonts/Georgia.ttf"

#ifdef WIN32
int __stdcall WinMain(int, int, int, int)
#else
int main()
#endif
{
	OpenGL3DEngine* Engine = new OpenGL3DEngine();

	

	if(!Engine->InitVideoMode(640, 480, 0, false))
	{
		std::cout<<"Wrong init!"<<std::endl;
		return 0;	
	}
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
	glEnable(GL_TEXTURE_2D);
		
	glDisable(GL_CULL_FACE);
	//glScalef(1, 1, -1);
	
	bool Quit = false;
	SDL_Event Event;
	SDL_Surface* Surface = IMG_Load("/home/ciplogic/avatar.png");
	OpenGLGraphic Texture;
	Texture.Convert(Surface);

	MeshContainer* Container;
	MeshBuilder MB;

	MB.Begin(MBMODE_TRIANGLE_STRIP);
	MB.SetColor(1.0f, 1.0f, 1.0f);
	MB.SetTexture(Texture.Texture);

	MB.SetTexture2D(0.0f, 0.0f);
	MB.AddVertexFloat(-64, -48, 44);
	MB.SetTexture2D(Texture.MaxU, 0.0f);
	MB.AddVertexFloat( 64, -48, 44);
	MB.SetTexture2D(0.0f, Texture.MaxV);
	MB.AddVertexFloat( -64, 48, 44);
	MB.SetTexture2D(Texture.MaxU, Texture.MaxV);
	MB.AddVertexFloat(64, 48, 44);
	
	Container = MB.End();

	MeshFrame* Frame = new MeshFrame();
	MeshTransform Transform;
	
	Frame->SetMeshContainer(Container);
	Frame->SetTransform(Transform);
	Transform.SetIdentity();
	//glRotatef(180, 0, 1, 0);
	//Transform.Scale(0.32, -0.32, -1.32);
	glScalef(1.0, -1.0, -1.0);

	GLFontTextureList Font;
	std::string FontName = DEFAULT_FONT_NAME;
	Font.MapFont(FontName, 16, 0);

	while(!Quit)
	{
		while(SDL_PollEvent(&Event))
		{
			if(Event.type == SDL_QUIT)
				Quit = true;
		}		
		Engine->GL.SetClearColor(.0f, .50f, 0.0f);
		Engine->GL.ClearScreen(true, true);

		//Transform.ApplyTranslate(0.0, 0.0, 0.20);
		//Transform.ApplyRotateX(0.52);
		//Transform.ApplyRotateZ(0.52);
		
		Engine->GL.EnableZBuffer(false);
		//Font.TextOut(-640, -478, "Hello world!");
		glDisable(GL_BLEND);
		Frame->Paint(Transform);
		glEnable(GL_BLEND);
		Engine->GL.ClearScreen(false, true);
		Font.TextOut(-640, -478, "Hello world!");
//		Engine->GL.EnableZBuffer(false);
		Engine->GL.Flip();
		
		usleep(10000);
	}
	SDL_Quit();
	return 0;
}
