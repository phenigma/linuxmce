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
//---------------------------------------------------------------------------------------------------------
class OrbiterApp: public PlutoGame
{
private:
	static OrbiterApp *m_pInstance;

	int m_iImageWidth, m_iImageHeight;

	pthread_mutexattr_t m_MutexAttr; 
	pthread_cond_t m_MessageQueueCond; 
	pluto_pthread_mutex_t m_ScreenMutex;

	bool m_bQuit;
	bool m_bSignalStrengthScreen;
	int  m_uSignalStrength;

	vector<int> m_vectRepeatedKeys;

	class BDCommandProcessor_Smartphone_Bluetooth *m_pBDCommandProcessor;

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


	//this
	void OnQuit();

	int PlutoKey2VirtualKey(int nPlutoKey);
	int VirtualKey2PlutoKey(int nVirtualKey, bool bLongKey);
	bool IsRepeatedKey(int nVirtualKey);

	//bd communications proxy functions:
	// - incoming
	void SetSignalStrength(int nSignalStrength);
	void SetSignalStrengthScreen(bool bSignalStrengthScreen);
	void InterceptRepeatedKeys(int nKeysListSize, char *pRepeatedKeysList);
	void ShowImage(int nImageType, int nSize, char *pData);
	// - outgoing
	void SendKey(int nKeyCode, int nEventType);

};
//---------------------------------------------------------------------------------------------------------
#endif