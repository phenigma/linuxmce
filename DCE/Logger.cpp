/* 
	Logger
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

/**
 *
 * @file Logger.cpp
 * @brief source file for the Logger, NullLogger and FileLogger classes
 * @author Aaron
 * @todo notcommented
 *
 */



#include <time.h>
#ifndef WIN32
#include <sys/time.h>
#endif

#include "Logger.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/StringUtils.h"
#include "Message.h"

#if (defined WIN32) && !defined(UNDER_CE) && !defined(UNICODE)
#include "PlutoUtils/ConsoleColor.h"
#endif

// some environment specific stuff
#ifndef WIN32
	char *LoggerFileName="/usr/local/pluto/var/log/PlutoServer.Logger.txt";
#else
	#ifdef UNDER_CE
		char *LoggerFileName="\\Storage Card\\PlutoServer.Logger.txt";
	#else
		char *LoggerFileName="C:\\ha\\level1\\PlutoServer\\PlutoServer.Logger.txt";
	#endif
#endif

using namespace ::std;
using namespace DCE;

#ifdef WINCE
	#include "wince.h"
#endif

Logger::Logger( const char* pcName ) : m_Lock( "logger" )
{
	if( pcName ) m_Name = pcName;

//	m_dwPK_Installation = atoi( StringUtils::get_pluto_parameter( "INSTALLATION" ).c_str() );

	pthread_mutexattr_init( &m_MutexAttr );
	pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
	m_Lock.Init( &m_MutexAttr );
}

Logger::~Logger()
{
	pthread_mutex_destroy(&m_Lock.mutex);
	pthread_mutexattr_destroy(&m_MutexAttr);
}

void Logger::SetInstallation(int iInstallation)
{
	m_dwPK_Installation = iInstallation;
}

void Logger::SetName( const char* pcName )
{
	m_Name = pcName;
}

void Logger::Write( int iLevel, const char *pcFormat, ... )
{
	timeval tv;
#ifdef WIN32
	SYSTEMTIME lt;
	::GetLocalTime( &lt );
	
	/** @todo Need to fill tv */
	tv.tv_sec = (long)time( NULL );
	tv.tv_usec = lt.wMilliseconds * 1000;
#else
	gettimeofday( &tv, NULL );
#endif
	
 	PLUTO_SAFETY_LOCK_LOGGER( sSM, m_Lock );  // Don't log anything but failures
	
#ifdef UNDER_CE
	/** @todo check comment */
	//LACA_B4_3("logger::before va_list entry clock: %d thread: %p format: %s",clock(),pthread_self(),Format);
#endif
	
	static char s[1024];
	memset(s, 0, sizeof(s));
	va_list argList;
	va_start( argList, pcFormat );
#ifdef WIN32
	_vsnprintf( s, sizeof(s), pcFormat, argList );
#else
	vsnprintf( s, sizeof(s), pcFormat, argList );
#endif
	va_end( argList );

	if( iLevel == LV_CRITICAL )
	{
#ifdef ERRORS2PLUTOHOME
#ifndef WIN32
		FILE* f = popen(( string("wget -q -O - \"http://www.plutohome.com/utils/message.php?FK_Installation=")
		 	+ StringUtils::itos( m_dwPK_Installation ) + "&Severity=3&Message=" 
			+ StringUtils::URLEncode(s) + "\"").c_str(), "r");
			
		if( f != NULL ) fclose(f);
		/** @todo -- need to somehow do this in a separate thread and not inline 
		the whole system dies while this is happening */
#endif
#endif
	}

	string sData;
	if( iLevel == LV_CRITICAL )
		sData = string( "\x1b[31;1m" ) + string(s) + string( "\x1b[0m" ); // setting the text color to red and back to black
	else if( iLevel == LV_WARNING )
		sData = string( "\x1b[33;1m" ) + string(s) + string( "\x1b[0m" ); // setting the text color to yellow and back to black
	else
		sData = s;

	Entry e( iLevel, tv, m_Name, sData, 0 ); // creating the entry

	WriteEntry( e ); // writting it
}

FileLogger::FileLogger( FILE* log )
{
	m_LogFile = log;
	m_bNeedToClose = false;
}

FileLogger::FileLogger( const char* file )
{
	m_LogFile = fopen(file, "a");
	m_bNeedToClose = true;
	if( m_LogFile == NULL )
	{
		m_bNeedToClose = false;
		m_LogFile = stderr;
	}
}

FileLogger::~FileLogger()
{
	if( m_bNeedToClose ) fclose( m_LogFile );
}

void FileLogger::SetColor( int iColor )
{
	// This takes an ANSI color value (the number before the m)
#if (defined WIN32) && !defined(UNDER_CE) && !defined(UNICODE)
	static int iBold = 0, iFgColor=7, iBgColor=0;
	int aiColortbl[] = { 0, 4, 2, 6, 1, 5, 3, 7 };

	if ( iColor==1 )
	{
		iBold = 1;
	}
	if ( iColor==0 )
	{
		iBold = 0;
		iFgColor = 7;
		iBgColor = 0;
	}
	if ( iColor >= 30 && iColor <= 37 )
	{
		iFgColor = aiColortbl[iColor-30];
	}
	if ( iColor >= 40 && iColor <= 47 )
	{
		iBgColor = aiColortbl[iColor-40];
	}
	setcolor( iFgColor + (iBold ? 8 : 0) + (iBgColor << 4) );
#endif
}

void FileLogger::ClearConsole()
{
#if (defined WIN32) && !defined(UNDER_CE) && !defined(UNICODE)
	clrscr();
#endif
}


void FileLogger::WriteEntry( Entry& Entry )
{
	PLUTO_SAFETY_LOCK_LOGGER( sSM, m_Lock );  // Don't log anything but failures
	
	struct tm *t = localtime((time_t *)&Entry.m_TimeStamp.tv_sec);
	char acBuff[50];
	double dwSec = (double)(Entry.m_TimeStamp.tv_usec/1E6) + t->tm_sec;
	snprintf( acBuff, sizeof(acBuff), "%02d/%02d/%02d %d:%02d:%06.3f", (int)t->tm_mon + 1, (int)t->tm_mday, (int)t->tm_year - 100, (int)t->tm_hour, (int)t->tm_min, dwSec );

#ifdef LOG_MOUSE_CLICKS
	static time_t tLastTouch=0;
	if( Entry.m_iLevel == LV_MOUSE_CLICKS ) // print the interval passed since the last writting of the mouse clicks entry
	{
		// I use these to automate sending messages to a remote system
		// and don't want the time and date stuff
		if( tLastTouch == 0 )
		{
			tLastTouch = time( NULL );
			fprintf( m_LogFile, "%s", Entry.m_sName.c_str() );
		}
		else
		{
			fprintf( m_LogFile, "sleep %d\n%s", time(NULL) - tLastTouch, Entry.m_sName.c_str() );
			tLastTouch = time( NULL );
		}
	}
	else
#endif
		fprintf( m_LogFile, "%02d\t%s\t%s\t", Entry.m_iLevel, acBuff, Entry.m_sName.c_str() );
	
	char *pcLp = (char*)Entry.m_sData.c_str();
#ifdef WIN32
	if ( m_LogFile == stdout )
	{
		char *pc, *pcMp;
		while( pcLp && *pcLp && (pc = strstr( pcLp, "\x1b[" )) )
		{
			fwrite( pcLp, pc - pcLp, 1, m_LogFile );
			pc += 2;
			pcMp = strchr( pc, 'm' );
			if ( pcMp )
			{
				do
				{
					SetColor( atoi(pc) );
					pc = strchr( pc, ';' );
				} while( pc++ && *pc && pc < pcMp );
				pcLp = pcMp + 1;
			}
		}
	}
#endif
	fwrite( pcLp, strlen( pcLp ), 1, m_LogFile );
#ifdef DEBUG

#ifdef WIN32
	fprintf( m_LogFile, " <%p>\n", pthread_self() );
#else
	fprintf( m_LogFile, " <%d>\n", (int)pthread_self() );
#endif

#else
	fwrite( "\n", 1, 1, m_LogFile );
#endif
	fflush( m_LogFile );
}

#ifdef WIN32
void WinOrbiterLogger::WriteEntry( Entry& entry )
{
	PLUTO_SAFETY_LOCK_LOGGER( sSM, m_Lock );  // Don't log anything but failures

/*	struct tm *t = localtime((time_t *)&Entry.m_dwTime);
	char acBuff[50];
	double dwSec = (double)(Entry.m_dwMicroseconds/1E6) + t->tm_sec;
	snprintf( acBuff, sizeof(acBuff), "%02d/%02d/%02d %d:%02d:%06.3f", (int)t->tm_mon + 1, (int)t->tm_mday, (int)t->tm_year - 100, (int)t->tm_hour, (int)t->tm_min, dwSec );
*/

	string str = entry.m_sData;
	StringUtils::Replace(str, "\x1b[31;1m", "");
	StringUtils::Replace(str, "\x1b[0m", "");
	StringUtils::Replace(str, "\x1b[33;1m", "");

	//TODO : use entry.m_iLevel to set a color for the output string
	//TODO: add time stamp 

#ifdef WINCE
	const MAX_STRING_LEN = 4096;
	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, str.c_str(), MAX_STRING_LEN);
	#define MESSAGE wTextBuffer
#else
	#define MESSAGE str.c_str()
#endif

	string s = str + "\n";
	FILE* f = fopen("orbiter_logger.out", "a+");
	fseek(f, 0, SEEK_END);
	fwrite(s.c_str(), s.length(), 1, f);
	fclose(f);

	const cTimeOutInterval = 50; //miliseconds
	DWORD Count;
	if(0 == ::SendMessageTimeout(m_hWndList, LB_ADDSTRING, 0L, (LPARAM)MESSAGE, SMTO_NORMAL, cTimeOutInterval, &Count))
		return; //send message timed out

	DWORD Result;
	::SendMessageTimeout(m_hWndList, LB_SETTOPINDEX, Count - 1, 0L, SMTO_NORMAL, cTimeOutInterval, &Result);
}
#endif