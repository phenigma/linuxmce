#ifndef __ORBITER_SDL_WIN_CE_H__
#define __ORBITER_SDL_WIN_CE_H__
//-----------------------------------------------------------------------------------------------------
#include "../SDL/OrbiterSDL.h"

//-----------------------------------------------------------------------------------------------------
struct GDI_FontInfo {
     HBITMAP bitmap;
     int bmpWidth;
     int bmpHeight;
     LPSTR bmpBits;
};
//-----------------------------------------------------------------------------------------------------
#define WIDTHBYTES(i)       ((i+31)/32*4)
#define PALETTE_SIZE        2
#define RMASK 0xFF000000
#define GMASK 0x00FF0000
#define BMASK 0x0000FF00
#define AMASK 0x000000FF
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

	void RenderTextWinCE(TextWinCEObject *pTextWinCEObject);

public:

	bool m_bConnectionLost;

	WNDPROC OldSDLWindowProc;
	HWND hSDLWindow;

	~OrbiterSDL_WinCE();

	//base public methods
	void OnQuit();
	void RenderText(DesignObjText *Text,TextStyle *pTextStyle);
	void WriteStatusOutput(const char* pMessage);
	bool SelfUpdate();


	//OrbiterSDL_WinCE public methods
	void RenderTextObjectsWinCE(HWND hWnd);
	void HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam);

	static void Cleanup();
	static void BuildOrbiterSDL_WinCE(int DeviceID, string ServerAddress, string sLocalDirectory, bool bLocalMode, 
		int nImageWidth, int nImageHeight, bool bFullScreen = false);
	static OrbiterSDL_WinCE *GetInstance();
};
//-----------------------------------------------------------------------------------------------------
#endif //__ORBITER_SDL_WIN_CE_H__