#ifndef ORBITERRENDERERBASE_H
#define ORBITERRENDERERBASE_H

#include <string>

#include "DesignObj_Orbiter.h"
#include "PlutoGraphic.h"
#include "SerializeClass/ShapesColors.h"

class OrbiterRenderer
{
	friend class OrbiterRendererFactory;

protected:

	OrbiterRenderer() {}

public:

	virtual ~OrbiterRenderer() {}

	virtual void SolidRectangle(int x, int y, int width, int height, PlutoColor color)=0;
	virtual void SolidRectangleAlpha(int x, int y, int width, int height, PlutoColor color, int alpha = 128) { SolidRectangle(x,y, width, height, color); };
	virtual void HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color)=0;
	virtual void DrawLine(int x, int y, int width, int height, PlutoColor color, int Opacity = 100) {};
	virtual void ReplaceColorInRectangle(int x, int y, int width, int height, PlutoColor ColorToReplace, PlutoColor ReplacementColor)=0;
	virtual void FloodFill(int x, int y, PlutoColor ColorToReplace, PlutoColor ReplacementColor) {};
	virtual void RenderText(string &sTextToDisplay,class DesignObjText *Text,class TextStyle *pTextStyle, PlutoPoint point = PlutoPoint(0, 0))=0;
	virtual void SaveBackgroundForDeselect(DesignObj_Orbiter *pObj, PlutoPoint point)=0;
	virtual PlutoGraphic *GetBackground( PlutoRectangle &rect )=0;
	virtual PlutoGraphic *CreateGraphic()=0;

	virtual void ObjectOnScreen(VectDesignObj_Orbiter *pVectDesignObj_Orbiter, DesignObj_Orbiter *pObj, PlutoPoint *ptPopup = NULL) {};
	virtual void ObjectOffScreen(DesignObj_Orbiter *pObj) {};

	// Rendering
	virtual void RenderScreen( bool bRenderGraphicsOnly ) ;
	virtual void RedrawObjects() {};
	virtual void RenderGraphic(class PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal, bool bDisableAspectRatio= false, PlutoPoint point = PlutoPoint(0, 0))=0;

	virtual void BeginPaint() { };
	virtual void EndPaint() { };
	virtual void UpdateRect(PlutoRectangle rect, PlutoPoint point=PlutoPoint(0,0)) { };

	// Other
	virtual void Initialize(GraphicType Type, int iPK_Room, int iPK_EntertainArea) = 0;
	virtual void SetTime(char *ServerTimeString) {};

	/**
	 *	Override the event of hilight area
	 */
	virtual void DoHighlightObject();
	virtual void UnHighlightObject( bool bDeleteOnly=false );
	virtual void DoHighlightObjectOpenGL() {} ;
	virtual void SelectObject( class DesignObj_Orbiter *pObj, PlutoPoint point = PlutoPoint(0, 0) );
	void OnIdle() {};

	void OpenGLUpdateScreen(void* Data) {};

	/**
	 *    That function wakes up the idle status for OpenGL operations
	 */
	void WakeupFromCondWait() {};
	virtual void RunEventLoop() {};
			/**
		* @brief locks the display, implemented in the Linux-derived class
		*/
     virtual void X_LockDisplay() {};
		/**
		* @brief unlocks the display, implemented in the Linux-derived class
		*/
     virtual void X_UnlockDisplay() {};

	virtual int HandleNotOKStatus(string sStatus,string sRegenStatus,int iRegenPercent);
	virtual bool RouterNeedsReload();
	virtual int DeviceIdInvalid();
	virtual int PickOrbiterDeviceID();
	virtual int SetupNewOrbiter();
	virtual int MonitorRegen(int PK_Device);
	virtual int PromptFor(string sToken);
	virtual int PromptUser(string sPrompt,int iTimeoutSeconds=10,map<int,string> *p_mapPrompts=NULL);
	virtual bool DisplayProgress(string sMessage, int nProgress) { return false; };
	virtual bool DisplayProgress(string sMessage, const map<string, bool> &mapChildDevices, int nProgress) { return false; }
	virtual bool RegenOrbiter(); // Send Orbiter plugin a command to regen this orbiter


	bool m_bFullScreen;
};

#endif

