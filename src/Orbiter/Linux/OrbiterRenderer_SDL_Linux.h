#ifndef __ORBITER_RENDERER_LINUX_H__
#define __ORBITER_RENDERER_LINUX_H__

#include "../SDL/OrbiterRenderer_SDL.h"

class OrbiterLinux;

namespace DCE
{
	class Orbiter;

	class OrbiterRenderer_SDL_Linux : public OrbiterRenderer_SDL
	{
	private:
		friend class OrbiterRendererFactory;
		OrbiterRenderer_SDL_Linux(Orbiter *pOrbiter);

	public:

		~OrbiterRenderer_SDL_Linux();

		void RenderScreen(bool bRenderGraphicsOnly);

		void InitializeAfterSetVideoMode();
		void InitializeAfterRelatives();

		bool DisplayProgress(string sMessage, int nProgress);
		bool DisplayProgress(string sMessage, const map<string, bool> &mapChildDevices, int nProgress);
		int PromptUser(string sPrompt,int iTimeoutSeconds=10,map<int,string> *p_mapPrompts=NULL);

		void LockDisplay();
		void UnlockDisplay();
	};
}
#endif //__ORBITER_RENDERER_SDL_LINUX_H__
