#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "VFD_LCD_Base.h"

void *VL_Thread(void *p)
{
	VFD_LCD_Base *pVFD_LCD_Base = (VFD_LCD_Base *) p;
	pVFD_LCD_Base->m_bVL_ThreadRunning=true;
	pVFD_LCD_Base->RunThread();
	pVFD_LCD_Base->m_bVL_ThreadRunning=false;
	return NULL;
}

VFD_LCD_Base::VFD_LCD_Base(int iNumColumns,int iNumLines) : m_VL_MessageMutex("m_VL_MessageMutex")
{
	m_iNumLines=iNumLines;
	m_iNumColumns=iNumColumns;
	m_bQuit_VL=m_bVL_ThreadRunning=false;

	pthread_cond_init(&m_VL_MessageCond, NULL);
	m_VL_MessageMutex.Init(NULL,&m_VL_MessageCond);

	pthread_create(&m_ptVL_Thread, NULL, VL_Thread, (void*)this);
}

VFD_LCD_Base::~VFD_LCD_Base()
{
	while( m_bVL_ThreadRunning )
	{
		pthread_cond_broadcast(&m_VL_MessageCond);
		Sleep(100);
	}
}

void VFD_LCD_Base::RunThread()
{
	PLUTO_SAFETY_LOCK(vl,m_VL_MessageMutex);
	while( !m_bQuit_VL )
	{
		vl.Release();
		int iSecondsToSleep=UpdateDisplay();
		vl.Relock();
		vl.TimedCondWait(iSecondsToSleep,0);
	}
}

void VFD_LCD_Base::NewMessage(int iMessageType,string sName,string sMessage,int ExpiresSeconds)
{
	MapMessages *pMapMessages = m_mapMessages_Get(iMessageType);
	if( sMessage.size() )
	{
		MapMessages::iterator it=pMapMessages->find(sName);
		if( it!=pMapMessages->end() )
			delete it->second;
		(*pMapMessages)[sName]=new VFD_LCD_Message(sMessage,ExpiresSeconds);
	}
	else
		pMapMessages->erase(sName);
}

int VFD_LCD_Base::UpdateDisplay()
{
	int SecondsToRedisplay;
	if(	SecondsToRedisplay=DisplayErrorMessages() )
		return SecondsToRedisplay;

	if(	SecondsToRedisplay=DisplayNoticesMessages() )
		return SecondsToRedisplay;

	if(	SecondsToRedisplay=DisplayNowPlayingRippingMessages() )
		return SecondsToRedisplay;

	if(	SecondsToRedisplay=DisplayStatusMessages() )
		return SecondsToRedisplay;

	DisplayDate();
	return 60;
}

int VFD_LCD_Base::DisplayErrorMessages()
{
	return 0;
}

int VFD_LCD_Base::DisplayNoticesMessages()
{
	return 0;
}

int VFD_LCD_Base::DisplayNowPlayingRippingMessages()
{
	return 0;
}

int VFD_LCD_Base::DisplayStatusMessages()
{
	return 0;
}

void VFD_LCD_Base::DisplayDate()
{
}
