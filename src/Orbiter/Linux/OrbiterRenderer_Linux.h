#ifndef __ORBITER_RENDERER_LINUX_H__
#define __ORBITER_RENDERER_LINUX_H__

#ifdef ORBITER_OPENGL
	#include "../OpenGL/OrbiterRenderer_OpenGL.h"
#else
	#include "../SDL/OrbiterRenderer_SDL.h"
#endif

#include <X11/X.h>
#include <X11/Xlib.h>

class OrbiterLinux;
class OrbiterRendererFactory;

namespace DCE
{
	class Orbiter;

#ifdef ORBITER_OPENGL
	class OrbiterRenderer_Linux : public OrbiterRenderer_OpenGL	
#else
	class OrbiterRenderer_Linux : public OrbiterRenderer_SDL
#endif
	{
	private:
		friend class ::OrbiterRendererFactory;
		OrbiterRenderer_Linux(Orbiter *pOrbiter);

		void ApplyMasks();

	public:

		~OrbiterRenderer_Linux();

		void RenderScreen(bool bRenderGraphicsOnly);
       	void EventLoop();

		void InitializeAfterSetVideoMode();
		void InitializeAfterRelatives();

		bool DisplayProgress(string sMessage, int nProgress);
		bool DisplayProgress(string sMessage, const map<string, bool> &mapChildDevices, int nProgress);
		int PromptUser(string sPrompt,int iTimeoutSeconds=10,map<int,string> *p_mapPrompts=NULL);

		void LockDisplay();
		void UnlockDisplay();

		bool HandleShowPopup(PlutoPopup* Popup, PlutoPoint Position, int EffectID);
		bool HandleHidePopup(PlutoPopup* Popup);

		void ObjectRendered(DesignObj_Orbiter *pObj_Screen, PlutoPoint point);

	private:

		Pixmap m_screenMaskObjects; //"layer for object non-popup
		Pixmap m_screenMaskPopups;  //"layer" for popups
		Pixmap m_screenMaskCurrent; //the mask to apply
		bool m_bHasPopups;
	};
}
#endif //__ORBITER_RENDERER_SDL_LINUX_H__
