#ifndef __ORBITERRENDERER_SDL_H__
#define __ORBITERRENDERER_SDL_H__

//-----------------------------------------------------------------------------------------------------
#include <iostream>
using namespace std;
//-----------------------------------------------------------------------------------------------------
#include <SDL.h>
//-----------------------------------------------------------------------------------------------------
#include "../OrbiterRenderer.h"
#include "../DesignObj_Orbiter.h"
#include "PlutoUtils/MultiThreadIncludes.h"
//-----------------------------------------------------------------------------------------------------
namespace DCE
{
class Orbiter;

class OrbiterRenderer_SDL : public OrbiterRenderer
{
private:

protected: 

	friend class OrbiterRendererFactory;
	OrbiterRenderer_SDL(Orbiter *pOrbiter);

public:

	virtual ~OrbiterRenderer_SDL();

    virtual void Configure();
	virtual void EventLoop();

	// Public methods

	// Drawing routines
	virtual void SolidRectangle(int x, int y, int width, int height, PlutoColor color);
	virtual void HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color);
	virtual void DrawLine(int x, int y, int width, int height, PlutoColor color) {};
	virtual void ReplaceColorInRectangle(int x, int y, int width, int height, PlutoColor ColorToReplace, PlutoColor ReplacementColor);
	virtual void FloodFill(int x, int y, PlutoColor ColorToReplace, PlutoColor ReplacementColor) {};
	virtual void RenderText(string &sTextToDisplay,class DesignObjText *Text,class TextStyle *pTextStyle, PlutoPoint point = PlutoPoint(0, 0));
	virtual void SaveBackgroundForDeselect(DesignObj_Orbiter *pObj, PlutoPoint point);
	virtual PlutoGraphic *GetBackground( PlutoRectangle &rect );
	virtual PlutoGraphic *CreateGraphic();

	// Rendering
	virtual void RenderScreen( bool bRenderGraphicsOnly );
	virtual void DisplayImageOnScreen(SDL_Surface *m_pScreenImage);
	virtual void RenderGraphic(class PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal, bool bDisableAspectRatio, PlutoPoint point = PlutoPoint(0, 0));

	virtual void BeginPaint();
	virtual void EndPaint();
	virtual void UpdateRect(PlutoRectangle rect, PlutoPoint point=PlutoPoint(0,0));

	virtual void OnQuit();

	// Other
	virtual void SetTime(char *ServerTimeString) {};

    /**
     * @brief initialize other classes, after the video mode was changed
     */
    virtual void InitializeAfterSetVideoMode() {};

    /**
     * @brief initialize other classes, after WaitForRelativesIfOSD
     */
    virtual void InitializeAfterRelatives() {};

	virtual void OnIdle();

public:

	SDL_Surface * m_pScreenImage;
	SDL_Surface * Screen;
	bool m_bFullScreen;

	virtual void LockDisplay() {} //no-op for windows
	virtual void UnlockDisplay() {}

	bool m_bRelativeMode;
	void CaptureRelativeMovements();
	void ReleaseRelativeMovements();
};

}
//-----------------------------------------------------------------------------------------------------
#endif //__ORBITERRENDERER_SDL_H__
