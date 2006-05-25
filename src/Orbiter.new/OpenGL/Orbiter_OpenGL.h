#ifndef __ORBITER_OPENGL_H__
#define __ORBITER_OPENGL_H__
//-----------------------------------------------------------------------------------------------------
#include "../Orbiter.h"
#include "../DesignObj_Orbiter.h"

#include <SDL.h>
class OrbiterGL3D;
//-----------------------------------------------------------------------------------------------------

namespace DCE
{
	class Orbiter_OpenGL : public Orbiter
	{
	protected:
		static Orbiter_OpenGL* m_pInstance; //the one and only instance of OrbiterSDL_Win32

		bool m_bUpdating;
		bool m_bFullScreen;

	public:

		Orbiter_OpenGL(int DeviceID, int PK_DeviceTemplate, string ServerAddress, string sLocalDirectory, bool bLocalMode, 
			int nImageWidth, int nImageHeight, bool bFullScreen = false, pluto_pthread_mutex_t *pExternalScreenMutex = NULL);
		virtual ~Orbiter_OpenGL();

		virtual bool GetConfig();

		virtual void GetWindowPosition(PlutoPoint& point);

		//from socket class
		virtual void PingFailed();
		virtual void UpdateScreen();

		// Drawing routines
		virtual void SolidRectangle(int x, int y, int width, int height, PlutoColor color);
		virtual void HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color);
		virtual void DrawLine(int x, int y, int width, int height, PlutoColor color) {};
		virtual void ReplaceColorInRectangle(int x, int y, int width, int height, PlutoColor ColorToReplace, PlutoColor ReplacementColor);
		virtual void FloodFill(int x, int y, PlutoColor ColorToReplace, PlutoColor ReplacementColor) {};
		virtual void RenderText(string &sTextToDisplay,class DesignObjText *Text,class TextStyle *pTextStyle, PlutoPoint point = PlutoPoint(0, 0));
		virtual void RenderText(HDC hdc,string &sTextToDisplay,PlutoRectangle &rPosition,int iPK_HorizAlignment,int iPK_VertAlignment,
			string &sFont,PlutoColor &ForeColor,int iPixelHeight,bool bBold,bool bItalic,bool bUnderline, PlutoPoint point, RECT &rectLocation);
		virtual void SaveBackgroundForDeselect(DesignObj_Orbiter *pObj, PlutoPoint point);
		virtual PlutoGraphic *GetBackground( PlutoRectangle &rect ) ;
		virtual PlutoGraphic *CreateGraphic();

		// Rendering
		virtual void RenderScreen( bool bRenderGraphicsOnly );
		virtual void RedrawObjects();

		virtual void RenderGraphic(class PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal, bool bDisableAspectRatio, PlutoPoint point = PlutoPoint(0, 0));

		virtual void BeginPaint();
		virtual void EndPaint();
		virtual void UpdateRect(PlutoRectangle rect, PlutoPoint point=PlutoPoint(0,0));

		virtual void ShowProgress(int nPercent);
		virtual void OnQuit();

		virtual int PromptUser(string sPrompt,int iTimeoutSeconds=10,map<int,string> *p_mapPrompts=NULL);
		virtual bool DisplayProgress(string sMessage, int nProgress);

		virtual void OnUnexpectedDisconnect();
		virtual void AdjustWindowSize(int iWidth, int iHeight);
		virtual void DoHighlightObject();
		virtual void SelectObject( DesignObj_Orbiter *pObj, PlutoPoint point );

		virtual void Initialize(GraphicType Type, int iPK_Room=0, int iPK_EntertainArea=0);
		virtual void SetTime(char *ServerTimeString) {};

		static void Cleanup();
		static void BuildOrbiter(int DeviceID, int PK_DeviceTemplate, string ServerAddress, string sLocalDirectory, bool bLocalMode, 
			int nImageWidth, int nImageHeight, bool bFullScreen = false, bool bUseOpenGL = false, pluto_pthread_mutex_t *pExternalScreenMutex = NULL);
		static Orbiter_OpenGL *GetInstance();

		void WriteStatusOutput(const char* pMessage);
		void CalcTextRectangle(RECT &rectLocation,PlutoRectangle &rPosition,int iRotation,int iHeight,int iVertAlignment);
		bool SelfUpdate();
		void WakeupFromCondWait();
		void OnIdle();

		bool m_bConnectionLost;

		bool PaintDesktopGL;
		pthread_cond_t m_GLThreadCond;
		OrbiterGL3D *m_Desktop;
		pluto_pthread_mutex_t* m_GLThreadMutex;
		pthread_t OpenGLThreadId;
	};
}

#endif