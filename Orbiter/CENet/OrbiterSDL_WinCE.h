#ifndef __ORBITER_SDL_WIN_CE_H__
#define __ORBITER_SDL_WIN_CE_H__
//-----------------------------------------------------------------------------------------------------
#include "../SDL/OrbiterSDL.h"
//-----------------------------------------------------------------------------------------------------
struct TextWinCEObject
{
	TextStyle *m_pTextStyle;
	string m_sText;
	RECT m_rectLocation;

	TextWinCEObject(string sText, RECT rectLocation, TextStyle *pTextStyle)
	{
		m_sText = sText;	
		m_rectLocation = rectLocation;
		m_pTextStyle = pTextStyle;
	}
};
//-----------------------------------------------------------------------------------------------------
class OrbiterSDL_WinCE : public OrbiterSDL
{
private:
	
    bool m_bShiftDown, m_bControlDown, m_bAltDown, m_bRepeat, m_bCapsLock;
    clock_t m_cKeyDown;

	static OrbiterSDL_WinCE* m_pInstance; //the one and only instance of OrbiterSDL_WinCE
	
	OrbiterSDL_WinCE(int DeviceID, string ServerAddress, string sLocalDirectory, bool bLocalMode, 
		int nImageWidth, int nImageHeight, bool bFullScreen = false);

	void RenderTextWinCE(HDC hDC, TextWinCEObject *pTextWinCEObject);
	void ClearWinCETextObjectsList();


public:

	list<TextWinCEObject *> m_listTextWinCEObject;

	WNDPROC OldSDLWindowProc;
	HWND hSDLWindow;

	~OrbiterSDL_WinCE();

	//OrbiterSDL public methods
	void RenderScreen();
	void RenderText(DesignObjText *Text,TextStyle *pTextStyle);

	//OrbiterSDL_WinCE public methods
	void RenderTextObjectsWinCE(HWND hWnd);
	void HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam);

	static void BuildOrbiterSDL_WinCE(int DeviceID, string ServerAddress, string sLocalDirectory, bool bLocalMode, 
		int nImageWidth, int nImageHeight, bool bFullScreen = false);
	static OrbiterSDL_WinCE *GetInstance();
};
//-----------------------------------------------------------------------------------------------------
#endif //__ORBITER_SDL_WIN_CE_H__