/*
 * $Id: Logger.cpp,v 1.11 2004/10/28 05:11:28 mihaid Exp $
 */

#include <time.h>
#ifndef WIN32
#include <sys/time.h>
#endif

#include "PlutoUtils/CommonIncludes.h"	
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "Message.h"

#if (defined WIN32) && !defined(UNDER_CE) && !defined(UNICODE)
#include "PlutoUtils/ConsoleColor.h"
#endif

#ifndef WIN32
	char *LoggerFileName="/usr/local/pluto/var/log/PlutoServer.Logger.txt";
#else
	#ifdef UNDER_CE
		char *LoggerFileName="\\Storage Card\\PlutoServer.Logger.txt";
	#else
		char *LoggerFileName="C:\\ha\\level1\\PlutoServer\\PlutoServer.Logger.txt";
	#endif
#endif

using namespace std;
using namespace DCE;

Logger::Logger( const char* name) : m_Lock("logger")
{
	if(name)
		m_Name = name;

	pthread_mutexattr_init(&m_MutexAttr);
	pthread_mutexattr_settype(&m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
	m_Lock.Init(&m_MutexAttr);
}

Logger::~Logger()
{
	pthread_mutex_destroy(&m_Lock.mutex);
	pthread_mutexattr_destroy(&m_MutexAttr);
}

void Logger::SetInstallation(int Installation)
{
	m_iPK_Installation=Installation;
}

void Logger::SetName(const char* name)
{
	m_Name = name;
}

void Logger::Write(int Level, const char *Format, ...)
{
	timeval tv;
#ifdef WIN32
	SYSTEMTIME lt;
	::GetLocalTime(&lt);
	//TODO Need to fill tv
	tv.tv_sec = (long) time(NULL);
	tv.tv_usec = lt.wMilliseconds * 1000;
#else
	gettimeofday(&tv, NULL);
#endif
	
 	PLUTO_SAFETY_LOCK_LOGGER(sSM,m_Lock);  // Don't log anything but failures
	
#ifdef UNDER_CE
//	LACA_B4_3("logger::before va_list entry clock: %d thread: %p format: %s",
//		clock(),pthread_self(),Format);
#endif
	static char s[1024];
	memset(s, 0, sizeof(s));
	va_list argList;
	va_start(argList, Format);
#ifdef WIN32
	_vsnprintf(s, sizeof(s), Format, argList);
#else
	vsnprintf(s,sizeof(s), Format, argList);
#endif
	va_end(argList);

	if( Level==LV_CRITICAL )
	{
#ifdef ERRORS2PLUTOHOME
#ifdef WIN32
		int k=2;  // So I can set a breakpoint
#else
		FILE* f = popen(( string("wget -q -O - \"http://www.plutohome.com/utils/message.php?FK_Installation=") +
			StringUtils::itos(m_iPK_Installation) + "&Severity=3&Message=" + StringUtils::URLEncode(s) + "\"").c_str(), "r");
		if(f != NULL)
			fclose(f);
// TODO -- need to somehow do this in a separate thread and not inline -- the whole system dies while this is happening
#endif
#endif
	}

	string Data;
	if(Level == LV_CRITICAL)
		Data = string("\x1b[31;1m") + string(s) + string("\x1b[0m");
	else if(Level == LV_WARNING)
		Data = string("\x1b[33;1m") + string(s) + string("\x1b[0m");
	else
		Data = s;

	Entry e(Level,tv.tv_sec,tv.tv_sec,m_Name,Data,0);

	WriteEntry(e);
}

//////////////////////////////////////////////////////////////////////
FileLogger::FileLogger(FILE* log)
{
	m_LogFile = log;
	m_bNeedToClose = false;
}

FileLogger::FileLogger(const char* file)
{
	m_LogFile = fopen(file, "a");
	m_bNeedToClose = true;
	if(m_LogFile == NULL)
	{
		m_bNeedToClose = false;
		m_LogFile = stderr;
	}
}

FileLogger::~FileLogger()
{
	if(m_bNeedToClose)
		fclose(m_LogFile);
}

void FileLogger::SetColor(int Color)
{
	// This takes an ANSI color value (the number before the m)
#if (defined WIN32) && !defined(UNDER_CE) && !defined(UNICODE)
	static int bold = 0, fgcolor=7, bgcolor=0;
	int colortbl[] = { 0, 4, 2, 6, 1, 5, 3, 7 };

	if (Color==1)
	{
		bold = 1;
	}
	if (Color==0)
	{
		bold = 0;
		fgcolor = 7;
		bgcolor = 0;
	}
	if (Color >= 30 && Color <=37)
	{
		fgcolor = colortbl[Color-30];
	}
	if (Color >= 40 && Color<=47)
	{
		bgcolor = colortbl[Color-40];
	}
	setcolor(fgcolor + (bold ? 8 : 0) + (bgcolor << 4));
#endif
}

void FileLogger::ClearConsole()
{
#if (defined WIN32) && !defined(UNDER_CE) && !defined(UNICODE)
	clrscr();
#endif
}


void FileLogger::WriteEntry(Entry& e)
{
	PLUTO_SAFETY_LOCK_LOGGER(sSM,m_Lock);  // Don't log anything but failures
	
	struct tm *t = localtime((time_t *)&e.m_dwTime);
	char c[50];
	double sec = (double)(e.m_dwMicroseconds/1E6) + t->tm_sec;
	snprintf(c,sizeof(c),"%02d/%02d/%02d %d:%02d:%06.3f",(int) t->tm_mon+1,(int) t->tm_mday,(int) t->tm_year-100,(int) t->tm_hour,(int) t->tm_min, sec);
	
#ifdef LOG_MOUSE_CLICKS
	static time_t tLastTouch=0;
	if( e.m_iLevel == LV_MOUSE_CLICKS )
	{
		// I use these to automate sending messages to a remote system
		// and don't want the time and date stuff
		if( tLastTouch==0 )
		{
			tLastTouch=time(NULL);
			fprintf(m_LogFile, "%s", e.m_sName.c_str());
		}
		else
		{
			fprintf(m_LogFile, "sleep %d\n%s", time(NULL)-tLastTouch, e.m_sName.c_str());
			tLastTouch=time(NULL);
		}
	}
	else
#endif
		fprintf(m_LogFile, "%02d\t%s\t%s\t", e.m_iLevel,c,e.m_sName.c_str());
	char *lp = (char*)e.m_sData.c_str();
#ifdef WIN32
	if (m_LogFile==stdout)
	{
		char *p, *mp;
		while(lp && *lp && (p = strstr(lp, "\x1b[")))
		{
			fwrite(lp, p-lp, 1, m_LogFile);
			p+=2;
			mp = strchr(p, 'm');
			if (mp)
			{
				do
				{
					SetColor(atoi(p));
					p = strchr(p, ';');
				} while(p++ && *p && p < mp);
				lp=mp+1;
			}
		}
	}
#endif
	fwrite(lp, strlen(lp), 1, m_LogFile);
#ifdef DEBUG

#ifdef WIN32
	fprintf(m_LogFile, " <%p>\n",pthread_self());
#else
	fprintf(m_LogFile, " <%d>\n",(int) pthread_self());
#endif

#else
	fwrite("\n", 1, 1, m_LogFile);
#endif
	fflush(m_LogFile);
}
