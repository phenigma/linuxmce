#ifndef __ORBITER_RENDERER_SDL_WIN32_H__
#define __ORBITER_RENDERER_SDL_WIN32_H__

#include "../OrbiterRenderer_SDL.h"

namespace DCE
{
	class Orbiter;

	class OrbiterRenderer_SDL_Win32 : public OrbiterRenderer_SDL
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
	};
}
#endif //__ORBITER_RENDERER_SDL_WIN32_H__