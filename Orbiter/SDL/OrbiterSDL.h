#ifndef __OrbiterSDL_H__
#define __OrbiterSDL_H__

//-----------------------------------------------------------------------------------------------------
#include <iostream> 
using namespace std; 

#include <SDL.h>

//-----------------------------------------------------------------------------------------------------
#include "../Orbiter.h"
#include "../DesignObj_Orbiter.h"
//-----------------------------------------------------------------------------------------------------
namespace DCE
{

class OrbiterSDL : public Orbiter
{
	SDL_Surface * m_pScreenImage;
	
protected: // (mtoader) I want access to them in the OrbiterLinuxDesktop
	int m_nImageWidth, m_nImageHeight;
	int m_bFullScreen;

public:
	OrbiterSDL(int DeviceID, string ServerAddress, string sLocalDirectory, bool bLocalMode, 
		int nImageWidth, int nImageHeight, bool bFullScreen = false);
	virtual ~OrbiterSDL();

	// Public virtual methods

	// Drawing routines
	virtual void SolidRectangle(int x, int y, int width, int height, PlutoColor color, int Opacity = 100);
	virtual void HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color);
	virtual void DrawLine(int x, int y, int width, int height, PlutoColor color, int Opacity = 100) {};
	virtual void ReplaceColorInRectangle(int x, int y, int width, int height, PlutoColor ColorToReplace, PlutoColor ReplacementColor) {};
	virtual void FloodFill(int x, int y, PlutoColor ColorToReplace, PlutoColor ReplacementColor) {};
	virtual void RenderGraphic(class DesignObj_Orbiter *pObj, PlutoRectangle rectTotal, bool bDisableAspectRatio=false);
	virtual void RenderText(class DesignObjText *Text,class TextStyle *pTextStyle);
	virtual void SaveBackgroundForDeselect(DesignObj_Orbiter *pObj);

	// Rendering
	virtual void RenderScreen();
	virtual void RedrawObjects();
	virtual void DisplayImageOnScreen(SDL_Surface *m_pScreenImage);

	// Other
	virtual void Initialize(GraphicType Type);
	virtual void SetTime(char *ServerTimeString) {};
	void putpixel(SDL_Surface * pSDL_Surface, int x, int y, Uint32 pixel_color);
	Uint32 getpixel(SDL_Surface * pSDL_Surface, int x, int y);

protected:

	// Protected abstract methods

	// Protected virtual methods

	SDL_Surface * Screen;
};

}
//-----------------------------------------------------------------------------------------------------
#endif //__OrbiterSDL_H__
