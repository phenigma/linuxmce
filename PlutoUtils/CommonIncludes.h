#ifndef COMMONINCLUDES_H
#define COMMONINCLUDES_H

#include <string>
#include <map>
#include <list>
#include <vector>
#include <fstream>

#include <stdarg.h>
#include "DCE/DCEConfig.h"

using namespace std;

#ifndef WIN32 
	// Linux stuff ------------------------
	#include <stdio.h>
	#include <unistd.h>
	#include <stdlib.h>
	#include <errno.h>
	#include <sys/stat.h>
	#include <sys/signal.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netinet/tcp.h>

	#include <netdb.h>
	#include <string.h>

	#define Sleep(x)   usleep(x*1000)
	//#define Sleep(x) { struct timeval t; t.tv_sec = (x / 1000) ; t.tv_usec = (x % 1000) * 1000000; select(0, NULL, NULL, NULL, &t); }

	#define stricmp(x, y) strcasecmp(x, y)

	#define min(x,y) ((x<y)?x:y)
	#define max(x,y) ((x>y)?x:y)

	typedef unsigned int UINT;
	typedef unsigned int LPARAM;
	typedef unsigned int WPARAM;
	#define LPVOID void *
	#define SOCKET int
	#define SOCKFAIL(x) (x < 0)
	#define SOCKOPTTYPE void *
	#define INVALID_SOCKET -1
	#define closesocket close
#else
// Windows Stuff ----------------------
	#define trunc(s) s 

	typedef __int8  int8_t;
	typedef __int16 int16_t;
	typedef __int32 int32_t;
	typedef __int64 int64_t;
	typedef unsigned __int8  uint8_t;
	typedef unsigned __int16 uint16_t;
	typedef unsigned __int32 uint32_t;
	typedef unsigned __int64 u_int64_t;
	typedef int socklen_t;
	#define u_int64_t __int64

	#define snprintf _snprintf // @#$#!%!!

	#include <windows.h>
	#include <objbase.h>
	#ifndef UNDER_CE
		#include <io.h>
	#else
		struct tm * __stdcall wce_localtime(const time_t *);
		time_t		__stdcall wce_time(time_t *);
	#endif 

	#define SOCKOPTTYPE char *
	#include <time.h>
	#include <winsock.h>
	#include <TCHAR.H>

	#define vsnprintf _vsnprintf
	#define SOCKFAIL(x) (x == SOCKET_ERROR)
#endif

#ifdef PROFILING
	#define PROFILE_START(x) clock_t x = clock();
	#define PROFILE_STOP(x, Desc)	g_pPlutoLogger->Write(LV_WARNING, "%s took %d ms", Desc, clock()-x);
#else
	#define PROFILE_START(x) 
	#define PROFILE_STOP(x, Desc)
#endif 

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember)) 

class PlutoFile
{
public:
	int *m_FD;
	PlutoFile(int *FD)
	{
		m_FD = FD;
	}
	~PlutoFile()
	{
		if (*m_FD)
		{
			close(*m_FD);
			*m_FD = 0;
		}
	}
};


#ifdef CENET
	extern "C" // Use extern "c" for C++ file only
	{ 
		#include "PKFUNCS.H"
	}; 
#endif

bool AskYNQuestion(string Question,bool bDefault);
char AskMCQuestion(string Question,string Prompts);

#endif
