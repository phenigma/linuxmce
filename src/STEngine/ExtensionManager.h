#ifndef ExtensionManager_H_
#define ExtensionManager_H_

#include <SDL.h>

class ExtensionManager
{
	SDL_Surface* Screen;
	float CleanRed, CleanGreen, CleanBlue;
public:
	int Width, Height;
	ExtensionManager(void);
	virtual ~ExtensionManager(void);
	
	virtual void Flip();
	virtual void SetClearColor(float Red, float Green, float Blue);
	virtual void ClearScreen(bool CleanColor = true, bool CleanZBuffer = true);
	virtual void EnableZBuffer(bool Enable);
	virtual void EnableBlended(bool Enable);

	virtual void Resize(int Width, int Height);

	virtual bool InitVideoMode(int Width, int Height, int Bpp, bool FullScreen, bool UseComposite);
	virtual void Setup();

	static bool CheckExtension(const char* checkFor);

	virtual void DrawArea(int X, int Y, int Width, int Height);
};


#endif
