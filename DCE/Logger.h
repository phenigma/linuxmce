#ifndef Logger_H
#define Logger_H

#ifndef SYMBIAN
#include <stdio.h>
#endif

#include <pthread.h>
#ifndef WIN32
#include <features.h>
#endif
#include <string>
#include <set>
#include <queue>

#include "SerializeClass/SerializeClass.h"

#define LV_CRITICAL 		1
#define LV_WARNING 			5
#define LV_EVENTHANDLER 	6
#define LV_EVENT 			7
#define LV_ACTION 			8
#define LV_CRITERIA 		9
#define LV_STATUS 			10
#define LV_EPG				11
#define LV_ALARM		12
#define LV_SOCKET		13
#define LV_LIGHTING		14
#define LV_CONTROLLER		15
#define LV_APPDAEMON		16
#define LV_CAMERA		17	
#define LV_CORPCLIENT		18
#define LV_FILEREQUEST		19
#define LV_DATAGRID		20
#define LV_LOCKING		21
#define LV_REGISTRATION		22
#define LV_XINE			23
#define LV_BURNER		24
#define LV_1394			25
#define LV_SPHINX		26
#define LV_FESTIVAL		27
#define LV_IR			28
#define LV_START		29
#define LV_START_DEVICE	30
#define LV_STOP			31
#define LV_MOUSE_CLICKS	32
#define LV_EPG_UPDATE	33
#define LV_MEDIA		34

#include "PlutoLockLogger.h"

namespace DCE
{
	class Logger
	{
	public:

		class Entry;

		Logger(const char* name = NULL);
		virtual ~Logger();

		virtual void SetInstallation(int Installation);
		virtual void SetName(const char* name);
		virtual void Start() {}

		virtual void Flush() {}

		virtual void Write(int Level, const char *Format, ...);

		virtual void WriteEntry(class Logger::Entry& entry) = 0;

		class Entry : public SerializeClass
		{
		public:
			int m_iLevel;
			long m_dwTime,m_dwMicroseconds;
			::std::string m_sName;
			::std::string m_sData;
			int m_iPK_Device;

			Entry() {}
			Entry(int Level,int Time,int Microseconds,string sName,string sData,int iPK_Device)
			{
				m_iLevel = Level; m_dwTime=Time; m_dwMicroseconds=Microseconds; m_sName=sName; m_sData=sData, m_iPK_Device=iPK_Device;
			}
			void SetupSerialization()
			{
				StartSerializeList() +	m_iLevel + m_dwTime + m_dwMicroseconds + m_sName + m_sData + m_iPK_Device;
			}
			virtual string SerializeClassClassName() { return "Logger::Entry"; }
		};

	protected:
		::std::string m_Name;
		int m_iPK_Installation;
		pluto_pthread_mutex_t m_Lock;
		pthread_mutexattr_t m_MutexAttr;
	};

	typedef map<string, Logger*> NameLoggerMap;
	typedef map<string, string> StringStringMap;

	class NullLogger : public Logger
	{
	public:
		NullLogger() : Logger("")
		{}

		virtual ~NullLogger()
		{}

		virtual void WriteEntry(class Logger::Entry& entry)
		{
			if (entry.m_iLevel == 0)
			{
			}
		}
	};


	class FileLogger : public Logger
	{
	public:

		FileLogger(FILE* log = stderr);
		FileLogger(const char* file);
		virtual ~FileLogger();

		virtual void SetColor(int Color);
		virtual void ClearConsole();
		virtual void WriteEntry(class Logger::Entry& entry);

	protected:
		FILE* m_LogFile;
		bool m_bNeedToClose;

	};

	// While global variables aren't recommended, every object within the same memory space
	// will be using the same logger anyway.  This is cleaner than passing a pointer to a
	// the logger around.  Each program that uses this will have to create the actual
	// instance of g_pPlutoLogger, using whatever derived class is appropriate
	extern class DCE::Logger *g_pPlutoLogger;
}

#endif
