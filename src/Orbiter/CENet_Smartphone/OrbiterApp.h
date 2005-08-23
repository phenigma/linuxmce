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
//---------------------------------------------------------------------------------------------------------
class OrbiterApp: public PlutoGame, public RenderMenu
{
private:
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
	int  m_nSignalStrength;
	bool m_bScrollUp;
	bool m_bSendKeyStrokes;

	//misc
	bool m_bRender_SignalStrengthOnly;
	vector<int> m_vectRepeatedKeys;
	class BDCommandProcessor_Smartphone_Bluetooth *m_pBDCommandProcessor;

	//image
	int m_nImageType;

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

public:
	OrbiterApp();
	virtual ~OrbiterApp();

	static OrbiterApp *GetInstance() { return m_pInstance; }

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

	// - outgoing
	void SendKey(int nKeyCode, int nEventType);

	//edit flags
	int m_nLastKeyCode;
	int m_nLastTick;
	int m_nRepeatStep;

};
//---------------------------------------------------------------------------------------------------------
#endif