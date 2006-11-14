#ifndef __ORBITER_RENDERER_LINUX_H__
#define __ORBITER_RENDERER_LINUX_H__

#ifdef ORBITER_OPENGL
	#include "../OpenGL/OrbiterRenderer_OpenGL.h"
#else
	#include "../SDL/OrbiterRenderer_SDL.h"
#endif

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

	public:

		~OrbiterRenderer_Linux();

		void RenderScreen(bool bRenderGraphicsOnly);
        // bUseMask : true  => use the mask
        // bUseMask : false => reset the mask
		bool RenderScreen_ApplyMask(bool bUseMask);
		void EventLoop();

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
