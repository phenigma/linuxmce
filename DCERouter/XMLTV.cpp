#ifdef WIN32
#include "afx.h"
#include <WinSock.h>
#else
#include <dirent.h>
#endif

#include <mysql.h>
#include <vector>
#include <sstream>
#include <time.h>
#include <string.h>

#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "DataGrid.h"
#include "DataGridDevice.h"
#include "PlutoServerCore.h"
#include "PlutoEvents.h"
#include "PlutoUtils/MySqlHelper.h"
#include "XMLTV.h"

#define BUFFER_SIZE		10000

int XMLTVcounter_PK_Schedule=0;
int XMLTVcounter_PK_Program=0;
int XMLTVcounter_PK_Station=0;

XMLTVStation::XMLTVStation(class XMLTVProvider *pProvider) : 
		m_pFirstSchedule(NULL), m_pNextStation(NULL), 
		m_pProvider(pProvider), m_iPK_Station(++XMLTVcounter_PK_Station),
		m_pEPGChannel(NULL), m_Channel(0)
{
	m_PK_Provider_Station = m_pProvider->m_iPK_Provider * 1000000 + m_iPK_Station;
}

XMLTVProvider::~XMLTVProvider()
{
	map<string,XMLTVStation *>::iterator itXTVS;
	for(itXTVS=mapXMLTVStationsByID.begin();itXTVS!=mapXMLTVStationsByID.end();++itXTVS)
	{
		XMLTVStation *s = (*itXTVS).second;
		delete s;
	}

	map<string,XMLTVProgram *>::iterator itXMTP;
	for(itXMTP=mapXMLTVPrograms.begin();itXMTP!=mapXMLTVPrograms.end();++itXMTP)
	{
		XMLTVProgram *p = (*itXMTP).second;
		delete p;
	}
}
XMLTVProgram::~XMLTVProgram()
{
	list<XMLTVSchedule *>::iterator lXS;
	for(lXS=m_listSchedule.begin();lXS!=m_listSchedule.end();++lXS)
	{
		delete (*lXS);
	}
}


void XMLTVProvider::ProcessStation(string *s,list<string *> &listStation,list<string *>::iterator &itLines)
{
	string::size_type startpos = s->find("id=");
	string::size_type endpos = startpos!=string::npos ? s->find("\"",startpos+4) : 0;
	if( startpos==string::npos || endpos==string::npos )
	{
		++itLines;
		g_pPlutoLogger->Write(LV_CRITICAL,"process Station cannot find id: %s",s->c_str());
		return;
	}

	int Channel=0;
	string ID = s->substr(startpos+4,endpos-startpos-4);
	string DisplayName;
	while( ++itLines!=listStation.end() )
	{
		s = (*itLines);
		const char *ptr = s->c_str();
		while( *ptr && *ptr==' ' )
			ptr++;

		// We're only concerned with lines that start with <channel or <programme
		if( strncmp(ptr,"</channel",9)==0 )
		{
			if( DisplayName.length()!=0 && ID.length()!=0 )
			{
				XMLTVStation *pStation = new XMLTVStation(this);

				// See if the DisplayName also contains the channel
				if( Channel!=0 )
				{
					if( atoi(DisplayName.c_str())==Channel )
					{
						string::size_type pos = DisplayName.find(" ");
						if( pos!=string::npos )
							DisplayName = DisplayName.substr(pos+1);
					}
				}

				string::size_type pos = DisplayName.find(".");
				if( pos!=string::npos )
					DisplayName = DisplayName.substr(pos+1);

				while( (pos=DisplayName.find("%20"))!=string::npos )
					DisplayName.replace(pos,3," ");

				pStation->m_sDescription = DisplayName;
				pStation->m_sID = ID;
				pStation->m_Channel = Channel;

				PlutoSqlResult result_set;
				MYSQL_ROW row=NULL;

				string query = string("SELECT Channel,Description FROM EPG_ChannelOverride WHERE XMLID='") + ID + "'" + 
					" AND FK_Provider=" + StringUtils::itos(m_iPK_Provider);

				// Don't use the normal thread safe functions--this is only run at startup
//				if( mysql_query(&m_pCore->m_MySQL,query.c_str())!=0 && (result_set.r=mysql_store_result(&m_pCore->m_MySQL)) && (row=mysql_fetch_row(result_set.r)) )
				if( (result_set.r=m_pCore->mysql_query_result(query.c_str()))!=NULL && (row=mysql_fetch_row(result_set.r)) )
				{
					int Channel = row[0] ? atoi(row[0]) : 0;
					string Description = row[1] ? row[1] : "";

					if( Channel )
						pStation->m_Channel = Channel;
					if( Description.size()>0 )
						pStation->m_sDescription = Description;
				}
				else
				{
					query = "INSERT INTO EPG_ChannelOverride (XMLID,FK_Provider,Description) VALUES('" + ID + "'," + StringUtils::itos(m_iPK_Provider) + ",'" + StringUtils::SQLEscape(DisplayName) + "')";
					int result = m_pCore->threaded_mysql_query(query.c_str()); // we don't care about the result
//					int result = mysql_query(&m_pCore->m_MySQL,query.c_str()); // we don't care about the result
g_pPlutoLogger->Write(LV_STATUS,"doing (%d-%s): %s",result,mysql_error(&m_pCore->m_MySQL),query.c_str());
				}

				query = string("SELECT * FROM EPG_Station_Ranking WHERE XMLID='") + ID + "'";

				// Don't use the normal thread safe functions--this is only run at startup
//				if( mysql_query(&m_pCore->m_MySQL,query.c_str())!=0 && (result_set.r=mysql_store_result(&m_pCore->m_MySQL)) && (row=mysql_fetch_row(result_set.r)) )
				if( (result_set.r=m_pCore->mysql_query_result(query.c_str()))==NULL || !(row=mysql_fetch_row(result_set.r)) )
				{
					query = "INSERT INTO EPG_Station_Ranking (XMLID) VALUES('" + ID + "')";
					int result=m_pCore->threaded_mysql_query(query.c_str()); // we don't care about the result
if( result!=0 )
{
	int k=2;
}
g_pPlutoLogger->Write(LV_STATUS,"doing (%d): %s",result,query.c_str());
				}

				if( !pStation->m_Channel )
				{
					delete pStation;
					return;
				}
				if( !m_pFirstStation )
					pTempStation = m_pFirstStation = pStation;
				else
				{
					pTempStation->m_pNextStation = pStation;
					pTempStation = pStation;
				}
				mapXMLTVStationsByID[ID] = pStation;
			}
			return;
		}
		else if( strncmp(ptr,"<display-name",13)==0 && DisplayName.length()!=0 )  // Sometimes there's more than 1 display name, the second is usually just the channel
		{
			startpos = s->find(">");
			endpos = startpos!=string::npos ? s->find("<",startpos) : 0;
			if( startpos==string::npos || endpos==string::npos )
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"process channel display name cannot find id: %s",s->c_str());
				continue;
			}
			
			if( atoi((s->substr(startpos+1,endpos-startpos-1)).c_str())!=0 )
				Channel = atoi((s->substr(startpos+1,endpos-startpos-1)).c_str());
		}
		else if( strncmp(ptr,"<display-name",13)==0 && DisplayName.length()==0 )  // Sometimes there's more than 1 display name, take the first
		{
			startpos = s->find(">");
			endpos = startpos!=string::npos ? s->find("<",startpos) : 0;
			if( startpos==string::npos || endpos==string::npos )
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"process channel display name cannot find id: %s",s->c_str());
				continue;
			}
			DisplayName = s->substr(startpos+1,endpos-startpos-1);
			if( atoi(DisplayName.c_str())!=0 )
				Channel = atoi(DisplayName.c_str());
		}
	}
}

static int iterations=0;
void XMLTVProvider::ProcessProgram(string *s,list<string *> &listStation,list<string *>::iterator &itLines)
{
const char *p1 = s->c_str();
	iterations++;
	string::size_type startpos = s->find("start=");
	string::size_type endpos = startpos!=string::npos ? s->find("\"",startpos+7) : 0;
	if( startpos==string::npos || endpos==string::npos )
	{
		++itLines;
		g_pPlutoLogger->Write(LV_CRITICAL,"process program cannot find start: %s",s->c_str());
		return;
	}

	string Start = s->substr(startpos+7,endpos-startpos-7);

	startpos = s->find("stop=",endpos);
	endpos = startpos!=string::npos ? s->find("\"",startpos+6) : 0;
	string Stop = startpos!=string::npos && endpos!=string::npos ? s->substr(startpos+6,endpos-startpos-6) : "";

	startpos = s->find("channel=",endpos);
	endpos = startpos!=string::npos ? s->find("\"",startpos+9) : 0;
	if( startpos==string::npos || endpos==string::npos )
	{
		++itLines;
		g_pPlutoLogger->Write(LV_CRITICAL,"process program cannot find Station: %s",s->c_str());
		return;
	}
	string Station = s->substr(startpos+9,endpos-startpos-9);

	string Title,Subtitle,Description,Category;
	while( ++itLines!=listStation.end() )
	{
		s = (*itLines);
		const char *ptr = s->c_str();
		while( *ptr && *ptr==' ' )
			ptr++;

		// We're only concerned with lines that start with <channel or <programme
		if( strncmp(ptr,"</programme>",12)==0 )
		{
			if( Start.length()!=0 && Station.length()!=0 && Title.length()!=0 )
			{
				XMLTVStation *pStation = mapXMLTVStationsByID[Station];
				if( !pStation )
				{
					g_pPlutoLogger->Write(LV_STATUS,"process program cannot find channel in map (maybe it's 0): %s",Station.c_str());
				}
				else
				{
					XMLTVProgram *pProgram = mapXMLTVPrograms[StringUtils::ToUpper(Title)];
					if( !pProgram )
					{
						pProgram = new XMLTVProgram();
						pProgram->m_sTitle = Title;
						mapXMLTVPrograms[StringUtils::ToUpper(Title)] = pProgram;
					}
					XMLTVSchedule *pSchedule = new XMLTVSchedule();
					pSchedule->m_pProgram = pProgram;
					pSchedule->m_pStation = pStation;
					CalculateDates(pSchedule,Start,Stop);
					pSchedule->m_sDescription = Description;
					pSchedule->m_sSubtitle = Subtitle;

					if( !pStation->m_pFirstSchedule )
						pStation->m_pFirstSchedule = pSchedule;
					else
					{
						XMLTVSchedule *ptmpSchedule = pStation->m_pFirstSchedule;
						while( ptmpSchedule->m_pNextSchedule )
							ptmpSchedule = ptmpSchedule->m_pNextSchedule;

                        ptmpSchedule->m_pNextSchedule = pSchedule;
					}
					pProgram->m_listSchedule.push_back(pSchedule);
				}
			}
			return;
		}
		else if( strncmp(ptr,"<title",6)==0 )
		{
			startpos = s->find(">");
			endpos = startpos!=string::npos ? s->find("<",startpos) : 0;
			if( startpos==string::npos || endpos==string::npos )
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"process channel display name cannot find title: %s",s->c_str());
				continue;
			}
			Title = s->substr(startpos+1,endpos-startpos-1);
		}
		else if( strncmp(ptr,"<sub-title",10)==0 )
		{
			startpos = s->find(">");
			endpos = startpos!=string::npos ? s->find("<",startpos) : 0;
			if( startpos==string::npos || endpos==string::npos )
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"process Station display name cannot find sub title: %s",s->c_str());
				continue;
			}
			Subtitle = s->substr(startpos+1,endpos-startpos-1);
		}
		else if( strncmp(ptr,"<desc",5)==0 )
		{
			startpos = s->find(">");
			endpos = startpos!=string::npos ? s->find("<",startpos) : 0;
			if( startpos==string::npos || endpos==string::npos )
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"process Station display name cannot find desc: %s",s->c_str());
				continue;
			}
			Description = s->substr(startpos+1,endpos-startpos-1);
		}
		else if( strncmp(ptr,"<category",9)==0 )
		{
			startpos = s->find(">");
			endpos = startpos!=string::npos ? s->find("<",startpos) : 0;
			if( startpos==string::npos || endpos==string::npos )
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"process Station display name cannot find category: %s",s->c_str());
				continue;
			}
			Category = s->substr(startpos+1,endpos-startpos-1);
		}
	}
}

void XMLTVProvider::CalculateDates(XMLTVSchedule *pSchedule,string Start,string Stop)
{
	pSchedule->tStartDate = CalculateDate(Start);
	if( Stop.length()==0 )
	{
		// The stop date must be within the start date
		// Or maybe it's not specified.  We'll have to set it for the prior show
		pSchedule->tEndDate = 0;
	}
	else
		pSchedule->tEndDate = CalculateDate(Stop);
}

time_t XMLTVProvider::CalculateDate(string Date)
{
	int Year = atoi(Date.substr(0,4).c_str());
	int Month = atoi(Date.substr(4,2).c_str());
	int Day = atoi(Date.substr(6,2).c_str());
	int Hour = atoi(Date.substr(8,2).c_str());
	int Minute = atoi(Date.substr(10,2).c_str());
	int Second = atoi(Date.substr(12,2).c_str());

	// See if the Date has a GMT indicator (ie +0100)
	string::size_type posGMT = Date.find("+");

	struct tm tmTime;
	tmTime.tm_year = Year-1900;
	tmTime.tm_mon = Month-1;
	tmTime.tm_mday = Day;
	tmTime.tm_hour = Hour;
	tmTime.tm_min = Minute;
	tmTime.tm_sec = Second;
	tmTime.tm_isdst = -1; // Calculate dst based on the US

	time_t tt_Time =  mktime(&tmTime);
	return tt_Time;
}

XMLTVProvider::XMLTVProvider(int PK_Provider,string Filename,class PlutoServerCore *pCore)
{
	m_pFirstStation=NULL;
	m_bValid=false;
	m_iPK_Provider=PK_Provider;
	m_pCore = pCore;

	g_pPlutoLogger->Write(LV_STATUS,"Building XML data for file: %s",Filename.c_str());

	char buffer[BUFFER_SIZE+1];
	list<string *> listLines;

	FILE *file = fopen(Filename.c_str(),"rb");
	if( file )
	{
		string *line=NULL;
		while(true)
		{
			size_t read = fread(buffer,1,BUFFER_SIZE,file);
			if( read==0 )
				break;

			buffer[read]=0;
			char *ptr=buffer;
			char *ptrStartOfLine=ptr;

			while(true)
			{
				while(*ptr && *ptr!='\r' && *ptr!='\n')
					ptr++;

				bool bEnd = !(*ptr);

				*ptr=0;
				if( !bEnd )
				{
					ptr++;
					while( *ptr && (*ptr=='\r' || *ptr=='\n') )
						ptr++;
				}

				// We've already got a partial line going
				if( line )
					(*line) += ptrStartOfLine;
				else
					line = new string(ptrStartOfLine);

				ptrStartOfLine = ptr;
				if( *ptr || !bEnd ) // We're not at the end of the buffer or it ended on a complete line
				{
					listLines.push_back(line);
					line = NULL;
					if( !*ptr )
						break;
				}
				else
				{
					if( line && read!=BUFFER_SIZE )
						listLines.push_back(line);
					break;
				}
			};
			if( read!=BUFFER_SIZE )
				break;
		};

		fclose(file);

		pTempStation=NULL;

		list<string *>::iterator itLines;
		for(itLines = listLines.begin();itLines != listLines.end();)
		{
			string *s = (*itLines);
			const char *ptr = s->c_str();
			while( *ptr && *ptr==' ' )
				ptr++;

			// We're only concerned with lines that start with <channel or <programme
			if( strncmp(ptr,"<channel",8)==0 )
				ProcessStation(s,listLines,itLines);
			else if( strncmp(ptr,"<programme",10)==0 )
				ProcessProgram(s,listLines,itLines);
			else if( strncmp(ptr,"</tv",3)==0 )
			{
				// We made it to the bottom
				m_bValid=true;
				break;
			}
			else
				++itLines;  // The above will increment the iterator
		}
	}
	else
		return;

	XMLTVStation *pLastStation = m_pFirstStation;
	while( pLastStation && pLastStation->m_pNextStation )
		pLastStation = pLastStation->m_pNextStation;

	// Add custom channels that maybe aren't being grabbed right now
	string query = string("SELECT XMLID,Channel FROM EPG_ChannelOverride WHERE FK_Provider=") + StringUtils::itos(m_iPK_Provider);
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;

// The following block causes mysql to fail to function anymore
// Don't use the normal thread safe functions--this is only run at startup
//	if( mysql_query(&m_pCore->m_MySQL,query.c_str())!=0 && (result_set.r=mysql_store_result(&m_pCore->m_MySQL)) )
	if( (result_set.r=m_pCore->mysql_query_result(query)) )
	{
		while( (row=mysql_fetch_row(result_set.r)) )
		{
			if( !row[0] || !row[1] || !atoi(row[1]) )  // Without a channel number
				continue;

			string ID = row[0];
			if( mapXMLTVStationsByID.find(ID)!=mapXMLTVStationsByID.end() )
				continue; // We did it already

			XMLTVStation *pStation = new XMLTVStation(this);
			pStation->m_sDescription = ID;
			pStation->m_sID = ID;
			pStation->m_Channel = atoi(row[1]);

			if( !m_pFirstStation )
				m_pFirstStation = pStation;

			if( pLastStation )
			{
				pLastStation->m_pNextStation = pStation;
			}
			pLastStation = pStation;

			mapXMLTVStationsByID[pStation->m_sID] = pStation;
		}
	}

//	return;

	// Be sure there is a schedule at least until this date, create an unknown program for any missing gaps
	time_t tScheduleUntil = time(NULL) + (86400 * 7); // 7 days in the future
	XMLTVProgram *pProgram = new XMLTVProgram();
	pProgram->m_sTitle = "Unknown";
	mapXMLTVPrograms["UNKNOWN"] = pProgram;

	XMLTVStation *pStation = m_pFirstStation;
	while( pStation )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Processing station: %s %s %d",pStation->m_sDescription.c_str(),pStation->m_sID.c_str(),pStation->m_Channel);
		XMLTVSchedule *pSchedule = pStation->m_pFirstSchedule;
		XMLTVSchedule *pLastSchedule = NULL;
		while( pSchedule )
		{
			if( pSchedule->tStartDate==0 )
			{
				if( pLastSchedule )
					pSchedule->tStartDate = pLastSchedule->tEndDate;
//				else
//					pSchedule->tStartDate = DataGridDevice::GetEPGStartingTime();
			}
			if( pSchedule->m_pNextSchedule && pSchedule->tEndDate==0 )
				pSchedule->tEndDate = pSchedule->m_pNextSchedule->tStartDate;
			else if( pSchedule->m_pNextSchedule && pSchedule->tEndDate!=pSchedule->m_pNextSchedule->tStartDate )
			{
				g_pPlutoLogger->Write(LV_WARNING,"For show %s channel %s start date: %d stop date: %d",
					pSchedule->m_pProgram->m_sTitle.c_str(),pSchedule->m_pStation->m_sID.c_str(),(int) pSchedule->tStartDate,(int) pSchedule->tEndDate);


				pSchedule->tEndDate = pSchedule->m_pNextSchedule->tStartDate;
			}
			pLastSchedule = pSchedule;
			pSchedule = pSchedule->m_pNextSchedule;
		}

		// If the last schedule had no endtime, arbitrarily set it to the start time + 30 mins
		if( pLastSchedule && !pLastSchedule->tEndDate )
			pLastSchedule->tEndDate = pLastSchedule->tStartDate + 1800;

		if( !pLastSchedule || pLastSchedule->tEndDate < tScheduleUntil )
		{
			time_t tStartDate;// = pLastSchedule ? pLastSchedule->tEndDate : DataGridDevice::GetEPGStartingTime();

			struct tm *lt = localtime(&tStartDate);
			if( lt->tm_min>=30 )
			{
				++lt->tm_hour;
				lt->tm_min=0;
			}
			else
				lt->tm_min=30;

			lt->tm_sec=0;
			time_t tEndDate = mktime(lt);

			while( tStartDate<tScheduleUntil )
			{
				XMLTVSchedule *pSchedule = new XMLTVSchedule();
				pSchedule->m_pProgram = pProgram;
				pSchedule->m_pStation = pStation;
				pSchedule->tStartDate = tStartDate;
				pSchedule->tEndDate = tEndDate;
				pSchedule->m_sDescription = "Unknown";

				// Move the block forward 30 minutes
				tStartDate += 1800;
				tEndDate += 1800;

				if( !pLastSchedule )
					pStation->m_pFirstSchedule = pSchedule;
				else
					pLastSchedule->m_pNextSchedule = pSchedule;

				pLastSchedule = pSchedule;
			}
		}
		pStation = pStation->m_pNextStation;
	};
}
