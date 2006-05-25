#ifndef __ORBITER_SDL_WIN_H__
#define __ORBITER_SDL_WIN_H__

//-----------------------------------------------------------------------------------------------------

#include "../SDL/OrbiterSDL.h"
#include "../OrbiterRendererFactory.h"
//-----------------------------------------------------------------------------------------------------
class OrbiterSDL_Win32 : public OrbiterSDL
{
	friend class OrbiterRendererFactory;

private:
	OrbiterSDL_Win32();

public:

	bool m_bConnectionLost;

	WNDPROC OldSDLWindowProc;
	HWND hSDLWindow;

	~OrbiterSDL_Win32();

	//base public methods
	void OnQuit();
	void RenderScreen( bool bRenderGraphicsOnly );
	bool SelfUpdate();

	void WriteStatusOutput(const char* pMessage);

	//OrbiterSDL_Win32 public methods
	void HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual int PromptUser(string sPrompt,int iTimeoutSeconds=10,map<int,string> *p_mapPrompts=NULL);
    bool DisplayProgress(string sMessage, int nProgress);
};
//-----------------------------------------------------------------------------------------------------
#endif //__ORBITER_SDL_WIN_H__