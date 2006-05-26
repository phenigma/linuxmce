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
	static OrbiterSDL_Win32* m_pInstance; //the one and only instance of OrbiterSDL_Win32

	OrbiterSDL_Win32(Orbiter *pOrbiter);

public:

	static OrbiterSDL_Win32 *GetInstance();

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

	void BuildOrbiter(Orbiter *pOrbiter);
	virtual int PromptUser(string sPrompt,int iTimeoutSeconds=10,map<int,string> *p_mapPrompts=NULL);
    bool DisplayProgress(string sMessage, int nProgress);
};
//-----------------------------------------------------------------------------------------------------
#endif //__ORBITER_SDL_WIN_H__