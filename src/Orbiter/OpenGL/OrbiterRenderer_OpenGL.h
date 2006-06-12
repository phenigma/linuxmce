#ifndef __ORBITERRENDERER_OPENGL_H__
#define __ORBITERRENDERER_OPENGL_H__
//-----------------------------------------------------------------------------------------------------
#include "../OrbiterRenderer.h"
#include "../DesignObj_Orbiter.h"

class OpenGL3DEngine;
//-----------------------------------------------------------------------------------------------------

namespace DCE
{
	class OrbiterRenderer_OpenGL : public OrbiterRenderer
	{
	protected:
		pthread_t GLThread;
		bool NeedToUpdateScreen_;

	public:

		OrbiterRenderer_OpenGL(Orbiter *pOrbiter);
		virtual ~OrbiterRenderer_OpenGL();

		virtual void Configure();
		virtual void EventLoop(); 
		virtual void Destroy();

		virtual bool NeedToUpdateScreen();
		virtual void ScreenUpdated();

		virtual void GetWindowPosition(PlutoPoint& point);
		virtual void UpdateScreen();

		// Drawing routines
		virtual void SolidRectangle(int x, int y, int width, int height, PlutoColor color);
		virtual void HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color);
		virtual void DrawLine(int x, int y, int width, int height, PlutoColor color) {};
		virtual void ReplaceColorInRectangle(int x, int y, int width, int height, PlutoColor ColorToReplace, PlutoColor ReplacementColor);
		virtual void FloodFill(int x, int y, PlutoColor ColorToReplace, PlutoColor ReplacementColor) {};
		virtual void RenderText(string &sTextToDisplay,class DesignObjText *Text,class TextStyle *pTextStyle, PlutoPoint point = PlutoPoint(0, 0));
		virtual void SaveBackgroundForDeselect(DesignObj_Orbiter *pObj, PlutoPoint point);
		virtual PlutoGraphic *GetBackground( PlutoRectangle &rect ) ;
		virtual PlutoGraphic *CreateGraphic();

		// Rendering
		virtual void RenderScreen(bool bRenderGraphicsOnly);

		virtual void RenderGraphic(class PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal, bool bDisableAspectRatio, PlutoPoint point = PlutoPoint(0, 0));

		virtual void BeginPaint();
		virtual void EndPaint();
		virtual void UpdateRect(PlutoRectangle rect, PlutoPoint point=PlutoPoint(0,0));

		virtual void AdjustWindowSize(int iWidth, int iHeight);
		virtual void DoHighlightObject();
		virtual void SelectObject( DesignObj_Orbiter *pObj, PlutoPoint point );

		virtual void RenderObjectAsync(DesignObj_Orbiter *pObj);

		virtual void ShowProgress(int nPercent);

		void WakeupFromCondWait();
		void OnIdle();


		virtual void OnQuit();

		OpenGL3DEngine* Engine;		
		pthread_cond_t Condition;
		pluto_pthread_mutex_t Mutex; 
	};
}

#endif //__ORBITERRENDERER_OPENGL_H__