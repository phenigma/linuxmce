#include "FileStatusObserver.h"
#include "inotify/FileNotifier.h"
//------------------------------------------------------------------------------------------
#define CHECK_OBSERVED_FILES_STATUS 1
#define OBSERVER_SCANNING_INTERVAL  3 //seconds
//------------------------------------------------------------------------------------------
FileStatusObserver FileStatusObserver::m_Instance;
//------------------------------------------------------------------------------------------
FileStatusObserver::FileStatusObserver() :
	m_ObservedFilesMutex("observed files"), m_pAlarmManager(NULL), m_pFileNotifier(NULL)
{
    m_ObservedFilesMutex.Init(NULL);

	m_pAlarmManager = new AlarmManager();
    m_pAlarmManager->Start(1); 
	m_pAlarmManager->AddRelativeAlarm(3, this, CHECK_OBSERVED_FILES_STATUS, NULL);
}
//------------------------------------------------------------------------------------------
FileStatusObserver::~FileStatusObserver()
{
	delete m_pAlarmManager;
	m_pAlarmManager = NULL;
}
//------------------------------------------------------------------------------------------
FileStatusObserver &FileStatusObserver::Instance()
{
	return m_Instance;
}
//------------------------------------------------------------------------------------------
void FileStatusObserver::SetFileNotifier(FileNotifier *pFileNotifier)
{
	PLUTO_SAFETY_LOCK(ofm, m_ObservedFilesMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OBSERVER: using file notifier %p", pFileNotifier);
	m_pFileNotifier = pFileNotifier;
}
//------------------------------------------------------------------------------------------
void FileStatusObserver::UnsetFileNotifier()
{
	PLUTO_SAFETY_LOCK(ofm, m_ObservedFilesMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "OBSERVER: not using any file notifier");
	m_pFileNotifier = NULL;
}
//------------------------------------------------------------------------------------------
void FileStatusObserver::Observe(string sFilename)
{
	PLUTO_SAFETY_LOCK(ofm, m_ObservedFilesMutex);

	list<string>::iterator it = find(m_listFileToObserve.begin(), m_listFileToObserve.end(), sFilename);
	if(it == m_listFileToObserve.end())
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "OBSERVER: File added to list with files to be observed: %s", sFilename.c_str());
		m_listFileToObserve.push_back(sFilename);
	}
}
//------------------------------------------------------------------------------------------
void FileStatusObserver::AlarmCallback(int id, void* param)
{
	if(id == CHECK_OBSERVED_FILES_STATUS)
	{
		PLUTO_SAFETY_LOCK(ofm, m_ObservedFilesMutex);
		for(list<string>::iterator it = m_listFileToObserve.begin(); it != m_listFileToObserve.end(); ++it)
		{
			string sFilename = *it;

			if(!FileUtils::FileExists(sFilename))
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "OBSERVER: The file was delete: %s", sFilename.c_str());
				m_listFileToObserve.erase(it++);
			}
			else if(!IsFileOpen(sFilename) && NULL != m_pFileNotifier)
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "OBSERVER: File not opened anymore: %s", sFilename.c_str());

				list<string> listFiles;
				listFiles.push_back(sFilename);
				m_pFileNotifier->FireOnCreate(listFiles);

				m_listFileToObserve.erase(it++);
			}
			else
			{
				++it;
			}
		}

		m_pAlarmManager->AddRelativeAlarm(3, this, CHECK_OBSERVED_FILES_STATUS, NULL);
	}
}

bool FileStatusObserver::IsFileOpen(string sFilename)
{
#ifdef WIN32

	HANDLE hFile = CreateFile(
		sFilename.c_str(),     // file to open
        GENERIC_WRITE,         // open for writing
        0,					   // deny reading or writing
        NULL,                  // default security
        OPEN_EXISTING,         // existing file only
        FILE_ATTRIBUTE_NORMAL, // normal file
        NULL);   

	if(NULL != hFile && INVALID_HANDLE_VALUE != hFile)
	{
		CloseHandle(hFile);
		return false;
	}

	if(ERROR_SHARING_VIOLATION == GetLastError())
		return true;

	return false;

#else

/*
	//TODO

	FILE *f = NULL;
	if(NULL != (f = fopen(sFilename.c_str(), "rx+")))
	{
		fclose(f);
		return false;
	}
	return true;
*/
	return false;

#endif

}
//------------------------------------------------------------------------------------------
