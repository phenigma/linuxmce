#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "EPG.h"

#include <iostream>
using namespace std;
using namespace DCE;
using namespace VDREPG;

EPG::~EPG()
{
	for(size_t s=0;s<m_vectChannel.size();++s)
		delete m_vectChannel[s];
	for(map<string,Program *>::iterator it=m_mapProgram.begin();it!=m_mapProgram.end();++it)
		delete it->second;
}

bool EPG::ReadFromFile(string sFile)
{
	size_t size;
	char *pBuffer = FileUtils::ReadFileIntoBuffer(sFile,size);
	g_pPlutoLogger->Write(LV_STATUS,"PG::ReadFromFile %d bytes",(int) size);
	if( !pBuffer )
		return false;

	m_pChannel_Reading=NULL;
	m_pEvent_Reading=NULL;
	m_pProgram_Reading=NULL;

	char *pPtr=pBuffer,*pEnd=pBuffer+size,*pPtrLine=pBuffer;
	while( pPtr<pEnd )
	{
		while( pPtr<pEnd && *pPtr!='\n' )
			pPtr++;

		if( *pPtr=='\n' )
		{
			*pPtr=0; // Terminate the line
			ProcessLine(pPtrLine);
			pPtr++;
			pPtrLine=pPtr;
		}
	}
	return true;
}

void EPG::ProcessLine(char *szLine)
{
	if( *szLine=='C' )
	{
		m_pChannel_Reading = new Channel(szLine+2);
		m_vectChannel.push_back(m_pChannel_Reading);
		m_pEvent_Reading = NULL;
		m_pProgram_Reading = NULL;
	}
	else if( *szLine=='E' )
	{
		m_pEvent_Reading = new Event(szLine+2,m_pChannel_Reading);
		m_mapEvent[m_pEvent_Reading->m_EventID] = m_pEvent_Reading;
		m_pEvent_Reading->m_pProgram = m_pProgram_Reading;  // If the program comes after the event, this will be NULL and we'll catch it below
		if( m_pEvent_Reading->m_tStartTime>m_tTime_First )
			m_tTime_First = m_pEvent_Reading->m_tStartTime;
		if( m_pEvent_Reading->m_tStopTime>m_tTime_Last )
			m_tTime_Last = m_pEvent_Reading->m_tStopTime;
		if( m_pChannel_Reading && (!m_pChannel_Reading->m_pEvent_First || m_pChannel_Reading->m_pEvent_First->m_tStartTime>m_pEvent_Reading->m_tStartTime) )
		{
			if( m_pChannel_Reading->m_pEvent_First )
				m_pChannel_Reading->m_pEvent_First->m_pEvent_Prior = m_pEvent_Reading;
			m_pEvent_Reading->m_pEvent_Next = m_pChannel_Reading->m_pEvent_First;
			m_pChannel_Reading->m_pEvent_First = m_pEvent_Reading;  // This is now the first, and will point to the last
			if( !m_pChannel_Reading->m_pEvent_Last )
				m_pChannel_Reading->m_pEvent_Last = m_pEvent_Reading;  // This was totally empty, so this is now the last too
		}
		else if( m_pChannel_Reading->m_pEvent_Last->m_tStartTime < m_pEvent_Reading->m_tStartTime )
		{
			m_pChannel_Reading->m_pEvent_Last->m_pEvent_Next = m_pEvent_Reading;
			m_pChannel_Reading->m_pEvent_Last = m_pEvent_Reading;
		}
		else
		{
			// I don't think this should happen, but we're not reading events in order.  Insert this in the middle somewhere
			Event *pEvent_Next = m_pChannel_Reading->m_pEvent_First;
			while( pEvent_Next->m_tStartTime < m_pEvent_Reading->m_tStartTime )  // pEvent_Next will point to the show right after us
				pEvent_Next = pEvent_Next->m_pEvent_Next;
			m_pEvent_Reading->m_pEvent_Next = pEvent_Next;
			m_pEvent_Reading->m_pEvent_Prior = pEvent_Next->m_pEvent_Prior;
			m_pEvent_Reading->m_pEvent_Prior->m_pEvent_Next = m_pEvent_Reading;
			m_pEvent_Reading->m_pEvent_Prior->m_pEvent_Prior = m_pEvent_Reading;
		}
	}
	else if( *szLine=='T' )
	{
		m_pProgram_Reading = m_mapProgram_Find(szLine+2);
		if( !m_pProgram_Reading )
		{
			m_pProgram_Reading = new Program(szLine+2);
			m_mapProgram[ m_pProgram_Reading->m_sTitle ] = m_pProgram_Reading;
		}
		if( m_pEvent_Reading )
			m_pEvent_Reading->m_pProgram = m_pProgram_Reading;  
	}
	else if( *szLine=='S' && m_pEvent_Reading )  // m_pEvent_Reading should always be non-null here
		m_pEvent_Reading->m_sDescription_Short = szLine+2;
	else if( *szLine=='D' && m_pEvent_Reading )  // m_pEvent_Reading should always be non-null here
		m_pEvent_Reading->m_sDescription_Long = szLine+2;
	else if( *szLine=='X' && m_pEvent_Reading )  // m_pEvent_Reading should always be non-null here
		m_pEvent_Reading->m_listStream.push_back( new Stream(szLine+2) );
	else if( *szLine=='e' ) 
	{
		m_pEvent_Reading = NULL;
		m_pProgram_Reading = NULL;
	}
	else if( *szLine=='c' )
	{
		if( !m_pChannel_Reading )
			g_pPlutoLogger->Write(LV_CRITICAL,"got a c, but not reading a channel");
		else
			g_pPlutoLogger->Write(LV_STATUS,"Read channel %s with events from %d-%d",
				m_pChannel_Reading->m_sChannelName,
				(m_pChannel_Reading->m_pEvent_First ? m_pChannel_Reading->m_pEvent_First->m_tStartTime : 0),
				(m_pChannel_Reading->m_pEvent_Last ? m_pChannel_Reading->m_pEvent_Last->m_tStopTime : 0));

		m_pChannel_Reading = NULL;
	}
}


Event::Event(char *szLine,Channel *pChannel)
{ 
	m_pChannel=pChannel; m_pProgram=NULL; m_pEvent_Next=NULL; m_pEvent_Prior=NULL; 
	char *pDuration,*pTableID,*pStartTime;
	if( (pStartTime=strchr(szLine,' '))!=NULL && (pDuration=strchr(pStartTime+1,' '))!=NULL && (pTableID=strchr(pDuration+1,' '))!=NULL )
	{
		*pDuration=*pTableID=*pStartTime=0;
		pDuration++; pTableID++; pStartTime++;
		m_EventID = atoi(szLine);
		m_tStartTime = atoi(pStartTime);
		m_tStopTime = m_tStartTime + atoi(pDuration);
	}
	else
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Event::Event EPG data is malformed");
		return;
	}
}

Event::~Event()
{
	for(list<Stream *>::iterator it=m_listStream.begin();it!=m_listStream.end();++it)
		delete *it;
}

Channel::Channel(char *szLine)
{
	char *pSpace = strchr(szLine,' ');
	if( !pSpace )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Channel::Channel EPG data is malformed");
		return;
	}
	*pSpace=0;
	m_sChannelName = pSpace+1;
	m_sFrequency = szLine;
	m_pEvent_First = NULL;
	m_pEvent_Last = NULL;
}

Channel::~Channel()
{
	Event *pEvent=m_pEvent_First;
	while(pEvent)
	{
		Event *pEvent_Next = pEvent->m_pEvent_Next;
		delete pEvent;
		pEvent = pEvent_Next;
	}

}

Event *Channel::GetCurrentEvent()
{
	time_t t = time(NULL);
	Event *pEvent = m_pEvent_First;
	while(pEvent && pEvent->m_tStopTime<t)
		pEvent = pEvent->m_pEvent_Next;
	return pEvent;
}

string Event::GetProgram()
{ 
	return m_pProgram ? m_pProgram->m_sTitle : ""; 
}
