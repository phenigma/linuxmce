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

class OrbiterGL3D;

namespace DCE
{

class OrbiterSDL : public Orbiter
{
protected: // (mtoader) I want access to them in the OrbiterLinuxDesktop
	/**
	 * OpenGL internal variable which say if is used OpenGL code at the moment of runtime
	 */
	bool EnableOpenGL;

public:
	/**
	 *    Default Constructor for Orbiter SDL
	 * @param DeviceID
	 * @param PK_DeviceTemplate
	 * @param ServerAddress
	 * @param sLocalDirectory
	 * @param bLocalMode
	 * @param nImageWidth Image width size in pixels
	 * @param nImageHeight Image height size in pixels
	 * @param bFullScreen Say if the application should be fullscreen or doesn't
	 * @param pExternalScreenMutex
	 * @param UseOpenGL Boolean variable that say if it should be used OpenGL code or doesn't
	 */
	OrbiterSDL(int DeviceID, int PK_DeviceTemplate, string ServerAddress, string sLocalDirectory,
        bool bLocalMode, int nImageWidth, int nImageHeight, bool bFullScreen = false,
        pluto_pthread_mutex_t *pExternalScreenMutex = NULL, bool UseOpenGL = false);
	virtual ~OrbiterSDL();
    virtual bool GetConfig();

	// Public virtual methods

	// Drawing routines
	virtual void SolidRectangle(int x, int y, int width, int height, PlutoColor color, int Opacity = 100);
	virtual void HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color);
	virtual void DrawLine(int x, int y, int width, int height, PlutoColor color, int Opacity = 100) {};
	virtual void ReplaceColorInRectangle(int x, int y, int width, int height, PlutoColor ColorToReplace, PlutoColor ReplacementColor);
	virtual void FloodFill(int x, int y, PlutoColor ColorToReplace, PlutoColor ReplacementColor) {};
	virtual void RenderText(string &sTextToDisplay,class DesignObjText *Text,class TextStyle *pTextStyle, PlutoPoint point = PlutoPoint(0, 0));
	virtual void SaveBackgroundForDeselect(DesignObj_Orbiter *pObj, PlutoPoint point);
	virtual PlutoGraphic *GetBackground( PlutoRectangle &rect );
	virtual PlutoGraphic *CreateGraphic();

	// Rendering
	virtual void RenderScreen();
	virtual void RedrawObjects();
	virtual void DisplayImageOnScreen(SDL_Surface *m_pScreenImage);

	virtual void RenderGraphic(class PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal, bool bDisableAspectRatio, PlutoPoint point = PlutoPoint(0, 0));

	virtual void BeginPaint();
	virtual void EndPaint();
	virtual void UpdateRect(PlutoRectangle rect, PlutoPoint point=PlutoPoint(0,0));

	virtual void OnQuit();

	// Other
	virtual void Initialize(GraphicType Type, int iPK_Room=0, int iPK_EntertainArea=0);
	virtual void SetTime(char *ServerTimeString) {};

	/**
	 *	Override the event of hilight area
	 */
	virtual void DoHighlightObject();
	virtual void DoHighlightObjectOpenGL();
	void SelectObject( DesignObj_Orbiter *pObj, PlutoPoint point );
	void OnIdle();

	void OpenGLUpdateScreen(void* Data);

	/**
	 *    That function wakes up the idle status for OpenGL operations
	 */
	void WakeupFromCondWait();

public:

	/// Used to stop the animation if is no effec pending but to draw once the background 
	///(safeing drawing)
	bool PaintDesktopGL;

	pthread_cond_t m_GLThreadCond;
	OrbiterGL3D *m_Desktop;
	pluto_pthread_mutex_t* m_GLThreadMutex;
	///
	SDL_Surface * m_pScreenImage;

	/// After and before render of screen frames
	auto_ptr<PlutoGraphic> m_spBeforeGraphic;
	auto_ptr<PlutoGraphic> m_spAfterGraphic;

	SDL_Surface * Screen;
	bool m_bFullScreen;
protected:
	pthread_t SDLGLthread;
};

}
//-----------------------------------------------------------------------------------------------------
#endif //__OrbiterSDL_H__
