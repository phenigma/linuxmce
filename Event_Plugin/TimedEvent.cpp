#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "Logger.h"

#include "TimedEvent.h"

#include "pluto_main/Table_Criteria.h"
#include "pluto_main/Table_CriteriaParm.h"
#include "pluto_main/Table_CriteriaParmNesting.h"
#include "pluto_main/Table_CriteriaParmList.h"
#include "pluto_main/Table_EventHandler.h"

#define	INTERVAL_EVENT	1
#define DAY_OF_WEEK		2
#define DAY_OF_MONTH	3
#define ABSOLUTE_TIME	4

TimedEvent::TimedEvent(Row_EventHandler *pRow_EventHandler)
{
	m_tTime=0;
	m_pRow_EventHandler=pRow_EventHandler;
	m_iTimedEventType=m_pRow_EventHandler->TimedEvent_get();
	m_pCommandGroup=NULL;
	vector<Row_CriteriaParm *> vectRow_CriteriaParm;
	pRow_EventHandler->FK_Criteria_getrow()->FK_CriteriaParmNesting_getrow()->CriteriaParm_FK_CriteriaParmNesting_getrows(&vectRow_CriteriaParm);
	for(size_t s=0;s<vectRow_CriteriaParm.size();++s)
	{
		Row_CriteriaParm *pRow_CriteriaParm = vectRow_CriteriaParm[s];
		if( pRow_CriteriaParm->FK_CriteriaParmList_get()==CRITERIAPARMLIST_Day_Of_Week_CONST )
			m_sDaysOfWeek=pRow_CriteriaParm->Value_get();
		else if( pRow_CriteriaParm->FK_CriteriaParmList_get()==CRITERIAPARMLIST_Time_of_day_CONST )
			m_sTimes=StringUtils::ToUpper(pRow_CriteriaParm->Value_get());
		else if( pRow_CriteriaParm->FK_CriteriaParmList_get()==CRITERIAPARMLIST_Day_Of_Month_CONST )
			m_sDaysOfMonth=pRow_CriteriaParm->Value_get();
		else if( pRow_CriteriaParm->FK_CriteriaParmList_get()==CRITERIAPARMLIST_Specific_Date_CONST )
			m_tTime=StringUtils::StringToDate(pRow_CriteriaParm->Value_get());
	}
	
	CalcNextTime();
}

void TimedEvent::CalcNextTime()
{
	switch(m_iTimedEventType)
	{
	case INTERVAL_EVENT:
		{
			if( m_tTime && m_tTime>time(NULL) )
				return; // It's valid, it just hasn't been tripped yet
			int i = atoi(m_sTimes.c_str());
			if( i )
			{
				time_t tCurrent = m_tTime==0 ? time(NULL) : m_tTime;  // Relative to the current time if we've not set it before
				if( m_sTimes.find('H')!=string::npos )
					m_tTime = tCurrent + i * 3600; // 3600 seconds per hour
				else
					m_tTime = tCurrent + i * 60; // 60 seconds per minute
			}
			else
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"Interval timer %s has no time: %s",
					m_pRow_EventHandler->Description_get().c_str(),m_sTimes.c_str());
				m_tTime=0;
				return;
			}
g_pPlutoLogger->Write(LV_STATUS,"Added interval timer %s at %d now %d seconds %d",
	m_pRow_EventHandler->Description_get().c_str(),(int) m_tTime,(int) time(NULL), (int) m_tTime-time(NULL));
		}
		break;
	case DAY_OF_WEEK:
		{
			time_t t=time(NULL);
			struct tm *tm_Now = localtime(&t);
			int CurrentDow = tm_Now->tm_wday==0 ? 7 : tm_Now->tm_wday; // We use M=1, Sunday=7
			if( (m_sDaysOfWeek.length() && !m_sDaysOfWeek.find(StringUtils::itos(CurrentDow))) ||
				!SetNextTime(tm_Now,tm_Now,m_sTimes) )
			{
				// Either the current day is not in the list, or there are no times in m_sTimes that are after tm_Now.
				// We need to find the next day that matches this
				int iDowFirst=8,iDowNext=8;
				
				string::size_type pos=0;
				while(pos<m_sDaysOfWeek.length())
				{
					int Dow = atoi( StringUtils::Tokenize(m_sDaysOfWeek,",",pos).c_str() );
					if( Dow>0 && Dow<iDowFirst )
						iDowFirst = Dow;
					if( Dow>0 && Dow>CurrentDow && Dow<iDowNext )
						iDowNext = Dow;
				}
				if( iDowNext<8 )
					tm_Now->tm_mday += iDowNext-CurrentDow;
				else if( iDowFirst<8 )
					tm_Now->tm_mday += (7-CurrentDow+iDowFirst);
				else
				{
					m_tTime=0;
					g_pPlutoLogger->Write(LV_CRITICAL,"Day of week timer %s has no day of week: %s",
						m_pRow_EventHandler->Description_get().c_str(),m_sDaysOfWeek.c_str());
					return;
				}

				if( !SetNextTime(NULL,tm_Now,m_sTimes) )
				{
					m_tTime=0;
					g_pPlutoLogger->Write(LV_CRITICAL,"Day of week timer %s has no next time: %s",
						m_pRow_EventHandler->Description_get().c_str(),m_sTimes.c_str());
					return;
				}
			}
			m_tTime = mktime(tm_Now);
		}
		break;
	case DAY_OF_MONTH:
		{
			time_t t=time(NULL);
			struct tm *tm_Now = localtime(&t);
			if( (m_sDaysOfMonth.length() && !m_sDaysOfMonth.find(StringUtils::itos(tm_Now->tm_mday+1))) ||
				!SetNextTime(tm_Now,tm_Now,m_sTimes) )
			{
				// Either the current day is not in the list, or there are no times in m_sTimes that are after tm_Now.
				// We need to find the next day that matches this
				int iDomFirst=99,iDomNext=99;
				string::size_type pos=0;
				while(pos<m_sDaysOfMonth.length())
				{
					int Dom = atoi( StringUtils::Tokenize(m_sDaysOfMonth,",",pos).c_str() );
					if( Dom<iDomFirst )
						iDomFirst = Dom;
					if( Dom>tm_Now->tm_mday+1 && Dom<iDomNext )
						iDomNext = Dom;
				}
				if( iDomNext<99 )
					tm_Now->tm_mday = iDomNext;
				else if( iDomFirst<99 )
				{
					tm_Now->tm_mday = iDomFirst;
					tm_Now->tm_mon++;
				}
				else
				{
					m_tTime=0;
					g_pPlutoLogger->Write(LV_CRITICAL,"Day of week timer %s has no day of week: %s",
						m_pRow_EventHandler->Description_get().c_str(),m_sDaysOfMonth.c_str());
					return;
				}

				if( !SetNextTime(NULL,tm_Now,m_sTimes) )  // First time that day
				{
					m_tTime=0;
					g_pPlutoLogger->Write(LV_CRITICAL,"Day of week timer %s has no next time: %s",
						m_pRow_EventHandler->Description_get().c_str(),m_sTimes.c_str());
					return;
				}
			}
			m_tTime = mktime(tm_Now);
		}
		break;
	case ABSOLUTE_TIME:
		return; // Nothing to do here
	}
	if( m_tTime && m_tTime < time(NULL) )
		m_tTime = time(NULL);
struct tm *tmLocal = localtime(&m_tTime);
g_pPlutoLogger->Write(LV_STATUS,"Timer: %s set for %d/%d/%d %d:%d:%d in %d seconds",
	m_pRow_EventHandler->Description_get().c_str(),tmLocal->tm_mon+1,tmLocal->tm_mday,tmLocal->tm_year,tmLocal->tm_hour,tmLocal->tm_min,tmLocal->tm_sec,m_tTime - time(NULL));
}

class TimeOfDay
{
public:
	int m_H,m_M,m_S;
	TimeOfDay() { m_H=m_M=m_S=-1; };
	TimeOfDay(string sTime) 
	{
		string::size_type pos=0;
		m_H = atoi(StringUtils::Tokenize(sTime,":",pos).c_str());
		m_M = atoi(StringUtils::Tokenize(sTime,":",pos).c_str());
		m_S = atoi(StringUtils::Tokenize(sTime,":",pos).c_str());

		if( sTime.find('p')!=string::npos || sTime.find('P')!=string::npos )
		{
			m_H += 12;
			if( m_H==24 )
				m_H=0;
		}
	};

	bool operator < (TimeOfDay &time1) 
	{ 
		if( time1.m_H!=m_H )
			return m_H<time1.m_H;
		if( time1.m_M!=m_M )
			return m_M<time1.m_M;
		return m_S<time1.m_S;
	}
	bool operator >= (struct tm *tm) 
	{
		if( tm->tm_hour!=m_H )
			return m_H>=tm->tm_hour;
		if( tm->tm_hour!=m_M )
			return m_M>=tm->tm_hour;
		return m_S>=tm->tm_hour;
	};
};

bool TimedEvent::SetNextTime(tm *tmAfter,tm *tmOutput,string sTimes)
{
  	TimeOfDay tod;

	string::size_type pos=0;
	while(pos<sTimes.length())
	{
		TimeOfDay tod2(StringUtils::Tokenize(sTimes,",",pos));
		if( (tod.m_H==-1 || tod2<tod) && (tmAfter==NULL || tod2>=tmAfter) )
			tod = tod2;
	}

	if( tod.m_H==-1 )
		return false; // We didn't find a valid time

	tmOutput->tm_hour = tod.m_H;
	tmOutput->tm_min = tod.m_M;
	tmOutput->tm_sec = tod.m_S;

	return true;
}
