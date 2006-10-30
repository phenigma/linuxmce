#ifndef _ORBTER_APP_H__
#define _ORBTER_APP_H__
//---------------------------------------------------------------------------------------------------------
#include <vector>
using namespace std;

#include <PocketFrog.h>
using namespace Frog;
//---------------------------------------------------------------------------------------------------------
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Orbiter/PocketFrog/PocketFrogWrapper.h"
#include "VIPShared/RenderMenu.h"
#include "VIPShared/MenuData.h"
//---------------------------------------------------------------------------------------------------------
class LRMenu;

#include <stdlib.h>

#define CONFIG_FILENAME "PlutoMO.cfg"
#define DEFAULT_LINK_NAME "Pluto"
#define ALERTS_LINK_NAME "Pluto Alerts"
#define ALERTS_LINK_SUBSTR "security=0"
#define MAX_URL_LENGTH 512

#if defined(_VC80_UPGRADE)		//--- CHANGED4WM5 ----//	
	#define DisplayDevice Display
#endif



class CSmartphone2003Favorites;

class OrbiterApp: public PlutoGame, public RenderMenu
{
private:
//#ifdef _LOCAL_RENDERED_OBJECTS_
    
  LRMenu* m_pMainMenu;
  bool m_bMainMenuRepaint;
  
//#endif

	//orbiter static instance
	static OrbiterApp *m_pInstance;

	//screen size
	int m_nImageWidth, m_nImageHeight;

	//mutexes for protection
	pthread_mutexattr_t m_MutexAttr; 
	pthread_cond_t m_MessageQueueCond; 
	pluto_pthread_mutex_t m_ScreenMutex;

	//flags
	bool m_bNeedRefresh;
	bool m_bRepeated;
	bool m_bDeleteLastKey;
	bool m_bDataKeys;
	bool m_bQuit;
	bool m_bSignalStrengthScreen;
	bool m_bImageQualityScreen;
	int  m_nSignalStrength;
	bool m_bScrollUp;
	bool m_bSendKeyStrokes;

	//misc
	bool m_bRender_SignalStrengthOnly;
	vector<int> m_vectRepeatedKeys;
	class BDCommandProcessor_Smartphone_Bluetooth *m_pBDCommandProcessor;

	//image
	int m_nImageType;
	unsigned long m_ulImageQuality;

	//background
	unsigned char m_nBkgndImageType;
	unsigned long m_pBkgndImage_Size;
	char *m_pBkgndImage_Data;

	//datagrid
	bool m_bGridExists;
	bool m_bGridSendSelectedOnMove;
	unsigned long m_ulGridX;
	unsigned long m_ulGridY;
	unsigned long m_ulGridWidth;
	unsigned long m_ulGridHeight;
	unsigned long m_ulGridSelectedItem;
	unsigned long m_ulGridTopItem;
	vector<string> m_vectDataGrid;

	//capture keyboard
	CaptureKeyboardParam m_CaptureKeyboardParam;

	//rendering
	bool m_bRedrawOnlyGrid;
	bool m_bRedrawOnlyEdit;

	void RenderImage(int nImageType, int nSize, char *pData, int nX, int nY, int nWidth, int nHeight);
	void RenderDataGrid(unsigned long ulX, unsigned long ulY, unsigned long ulWidth, 
		unsigned long ulHeight, vector<string> &vectGridList);
	void RenderText(string sText, unsigned long ulX, unsigned long ulY, unsigned long ulWidth, 
		unsigned long ulHeight, COLORREF color);
	void RenderSignalStrength(int nSignalStrength);
	void RenderImageQuality();
	void RenderEditBox();

	//this
	void OnQuit();
	bool HandleDataGridKeys(int nPlutoKey, bool bKeyUp); //handles up, down and enter for data grid
	bool HandleAutomaticDataGridScrolling(int nPlutoKey, bool bKeyUp); //handles data grid automatic scroll
	bool HandleCaptureKeyboardKeys(int nPlutoKey, bool bKeyUp, bool bLongKey);
	void LocalKeyPressed(int nKeyCode);

	//misc
	int PlutoKey2VirtualKey(int nPlutoKey);
	int VirtualKey2PlutoKey(int nVirtualKey, bool bLongKey);
	bool IsRepeatedKey(int nVirtualKey);
	
	bool SelectCurrentItem();
	void AutomaticallyScrollDataGrid(bool bKeyUp);

	bool SetVariable();
	bool IsNumberKey(int KeyCode);
	bool ClearEdit();
	bool ClearAllEdit();
	char GetKeyChar(int KeyCode);
	bool ScrollListPartialMatch();
	int NumberKeyIndex(int KeyCode);

	#if defined(SMARTPHONE2005)		//--- CHANGED4WM5 ----//
		// Map virtual key codes from Palm Treo to Smartphone 2003
		void PreTranslateVirtualKey( UINT uMsg, WPARAM* wParam, bool *bLongKey );
		// Hook keyboard to intercept Red button
		int m_hHook;
		void SetKeybdHook( bool bClear = false );
		friend LRESULT CALLBACK HookProcedure(int nCode, WPARAM wParam, LPARAM lParam);
	#endif

public:
	OrbiterApp(HINSTANCE hInstance);
	virtual ~OrbiterApp();
	HINSTANCE m_hInstance;

	static OrbiterApp *GetInstance() { return m_pInstance; }

	virtual void Reset();

	// Public virtual methods (from Game)
	virtual void GameSuspend(); 
	virtual void GameResume(); 
	virtual bool IsFullScreen();
	virtual int GetWindowWidth();
	virtual int GetWindowHeight();
    virtual bool GameInit();
    virtual void GameEnd();
    virtual void GameLoop();
    virtual void StylusDown( Point stylus );
    virtual void StylusUp( Point stylus );
    virtual void StylusMove( Point stylus );
	virtual bool PocketFrogButtonDown(int nButton);
	virtual bool PocketFrogButtonUp(int nButton); 
	virtual void HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void TryToUpdate();

	//public virtual methods (from RenderMenu)
	virtual void Redraw();
	virtual void MessageBox(const char *Message,const char *Title);
	virtual bool DrawImage(const char *Filename,int X,int Y,int Width,int Height);
	virtual bool DrawImage(unsigned char ImageType,void *pGraphic,int GraphicSize,int X,int Y,int Width,int Height);
	virtual bool Draw3dRect(MyRect &r);
	virtual bool DrawText(const char *Text, MyRect &r);
	virtual void SetTextProperties(int Size,const char *Font,int R, int G, int B);
	virtual void SwitchToMenu(VIPMenu *pMenu);
	virtual void OpenProgram(string ProgramName);

	//rendering
	void ShowDisconnected();
	void RefreshScreen();

	void Hide();
	void Show();

	//datagrid automatic scroll
	bool ScrollListUp();
	bool ScrollListDown();

	//bd communications proxy functions:
	// - incoming
	void SetSignalStrength(int nSignalStrength);
	void SetImageQualityScreen(bool bImageQualityScreen);
	void SetSignalStrengthScreen(bool bSignalStrengthScreen);
	void InterceptRepeatedKeys(int nKeysListSize, char *pRepeatedKeysList);
	void ShowImage(int nImageType, int nSize, char *pData);
	void ShowList(unsigned long ulX, unsigned long ulY, unsigned long ulWidth, 
		unsigned long ulHeight, vector<string> vectDataGrid, bool bSendSelectedOnMove, 
		bool bTurnOn, int nSelectedIndex);
	bool IsScrollingUp() { return m_bScrollUp; } 
	void SetCurrentSignalStrength(int nSignalStrength);
	void SendKeyStrokes(bool bValue);
	void SaveFile(unsigned long ulFileNameSize, char *pFileName, unsigned long ulFileDataSize, char *pFileData);
	void SetCaptureKeyboard(bool bOnOff, bool bDataGrid, bool bReset, int nEditType, int nVariable, string sText);
	void SetImageQuality(unsigned long ulImageQuality);
	void SetBkgndImage( unsigned char nImageType, int nSize, char *pData );
	void SetMenuData( MenuData& data );
	void SendConfiguration( void );
	void SendSelectedItem( string sItemId );

	// - outgoing
	void SendKey(int nKeyCode, int nEventType);

	//edit flags
	int m_nLastKeyCode;
	int m_nLastTick;
	int m_nRepeatStep;

	// Bookmarks
	bool IsConfigFileName( char* szFileName ) {
		return _stricmp( szFileName, CONFIG_FILENAME )==0?true:false;
	}
	void CheckBookmarks( void );

	DisplayDevice* GetDisplay() { return PlutoGame::GetDisplay(); }


	class IncomingCallNotifier *m_pIncomingCallNotifier;

};
//---------------------------------------------------------------------------------------------------------

class CSmartphone2003Favorites {
private:
	static bool MakeLinkFileName(LPCTSTR pszName, LPTSTR pszBuf);
	static TCHAR m_szBuffer[MAX_PATH];
public:
	static bool CheckURLName( LPCTSTR pszURLName );
	static bool AddLinkToFavorites(LPCTSTR pszName, LPCTSTR pszURL, BOOL bFailIfExists);	
	static bool DelLinkFromFavorites(LPCTSTR pszName);
};

#endif