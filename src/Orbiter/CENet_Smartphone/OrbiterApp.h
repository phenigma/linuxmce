#ifndef _ORBTER_APP_H__
#define _ORBTER_APP_H__
//---------------------------------------------------------------------------------------------------------
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
	virtual bool PocketFrogButtonDown(int button);
	virtual bool PocketFrogButtonUp(int button); 
	virtual void HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual void TryToUpdate();


	//this
	void ShowImage(int iImageType, int iSize, char *pData);
	void SendKey(int iKeyCode, int iEventType);
	void OnQuit();
};
//---------------------------------------------------------------------------------------------------------
#endif