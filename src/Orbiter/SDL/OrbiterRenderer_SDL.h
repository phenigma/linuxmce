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
#include "../OpenGL/PendingGLEffects.h"
#include "PlutoUtils/MultiThreadIncludes.h"
//-----------------------------------------------------------------------------------------------------
//forward declarations
class OrbiterGL3D;
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

	/**
	 * OpenGL internal variable which say if is used OpenGL code at the moment of runtime
	 */
	bool EnableOpenGL;
	std::auto_ptr<PendingGLEffects> m_spPendingGLEffects;

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

	virtual void ObjectOnScreen(VectDesignObj_Orbiter *pVectDesignObj_Orbiter, DesignObj_Orbiter *pObj, PlutoPoint *ptPopup = NULL);
	virtual void ObjectOffScreen(DesignObj_Orbiter *pObj);

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

	/**
	 *	Override the event of hilight area
	 */
	virtual void DoHighlightObject();
	virtual void DoHighlightObjectOpenGL();
	virtual void SelectObject( DesignObj_Orbiter *pObj, PlutoPoint point );
	virtual void OnIdle();

	//virtual void OpenGLUpdateScreen(void* Data);

	/**
	 *    That function wakes up the idle status for OpenGL operations
	 */
	virtual void WakeupFromCondWait();

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

	virtual void LockDisplay() {} //no-op for windows
	virtual void UnlockDisplay() {}

	bool m_bRelativeMode;
	void CaptureRelativeMovements();
	void ReleaseRelativeMovements();

	virtual void RenderFrame(void *);

	virtual bool DisplayProgress(string sMessage, int nProgress);
	virtual bool DisplayProgress(string sMessage, const map<string, bool> &mapChildDevices, int nProgress);
	virtual int PromptUser(string sPrompt,int iTimeoutSeconds=10,map<int,string> *p_mapPrompts = NULL);

protected:
    pthread_t SDLGLthread;
};

}
//-----------------------------------------------------------------------------------------------------
#endif //__ORBITERRENDERER_SDL_H__
