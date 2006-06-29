#ifndef __ORBITER_RENDERER_SDL_WIN32_H__
#define __ORBITER_RENDERER_SDL_WIN32_H__

#ifdef ORBITER_OPENGL
#include "../../OpenGL/OrbiterRenderer_OpenGL.h"
#else
#include "../OrbiterRenderer_SDL.h"
#endif

namespace DCE
{
	class Orbiter;

	class OrbiterRenderer_SDL_Win32 : public 
#ifdef ORBITER_OPENGL
		OrbiterRenderer_OpenGL
#else
		OrbiterRenderer_SDL
#endif
	{
	private:
		friend class OrbiterRendererFactory;
		OrbiterRenderer_SDL_Win32(Orbiter *pOrbiter);

	public:

		~OrbiterRenderer_SDL_Win32();

		void HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam);

		bool DisplayProgress(string sMessage, int nProgress);
		int PromptUser(string sPrompt,int iTimeoutSeconds,map<int,string> *p_mapPrompts);

		WNDPROC m_pfOldSDLWindowProc;
		HWND m_hSDLWindow;

		virtual void SetupWindow();
	};
}
#endif //__ORBITER_RENDERER_SDL_WIN32_H__