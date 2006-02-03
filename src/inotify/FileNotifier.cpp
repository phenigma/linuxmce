#ifndef WIN32
	#include <sys/ioctl.h>
	#include <errno.h>
	#include "inotify/inotify.h"
	#include "inotify/inotify-syscalls.h"
#else
	#include <sys/types.h> 
	#include <sys/stat.h>
#endif

#include "FileNotifier.h"
#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"
#include "PlutoUtils/Other.h"
#include "pluto_media/Database_pluto_media.h"
#include "pluto_media/Table_File.h"
using namespace DCE;
//-----------------------------------------------------------------------------------------------------
#ifdef WIN32
	#define IN_ALL_EVENTS 0xFFFFFFFF
	#define IN_CREATE		0x00000100
	#define IN_MOVED_TO		0x00000080
	#define IN_MOVED_FROM	0x00000040
	#define IN_DELETE		0x00000200
	#define IN_DELETE_SELF	0x00000400
	#define IN_ISDIR		0x40000000
#endif
//-----------------------------------------------------------------------------------------------------
void *INotifyWorkerThread(void *);
void *BackgroundWorkerThread(void *);
//-----------------------------------------------------------------------------------------------------
bool operator > (string s1, string s2)
{
	return s1.compare(0, s1.size(),  s2) > 0;
}
//-----------------------------------------------------------------------------------------------------
FileNotifier::FileNotifier(class Database_pluto_media *pDatabase_pluto_media) : 
	m_WatchedFilesMutex("watched files")
{
    m_bCallbacksRegistered = false;
    m_bCancelThread = false;

    m_pfOnCreate = NULL;
	m_pfOnDelete = NULL;

	m_wdRootFolder = 0;
    m_WatchedFilesMutex.Init(NULL);

	m_pDatabase_pluto_media = pDatabase_pluto_media;
}
//-----------------------------------------------------------------------------------------------------
FileNotifier::~FileNotifier(void)
{
    m_bCancelThread = true;
}
//-----------------------------------------------------------------------------------------------------
void FileNotifier::Watch(string sDirectory)
{
#ifdef WIN32	
	if(!m_wdRootFolder)
	{
		m_wdRootFolder = 1;
		m_sRootFolder = sDirectory;
	}
#endif

    list<string> listFilesOnDisk;
    FileUtils::FindDirectories(listFilesOnDisk,sDirectory,true,false,0,sDirectory + "/");  
	listFilesOnDisk.push_back(sDirectory);
		
	PLUTO_SAFETY_LOCK(wfm, m_WatchedFilesMutex);
    for(list<string>::iterator it = listFilesOnDisk.begin(); it != listFilesOnDisk.end(); it++)
	{
        string sItem = *it;
			
		try
		{
			int wd = m_inotify.watch(*it, IN_ALL_EVENTS);

			if(!m_wdRootFolder)
			{
				m_wdRootFolder = wd; //root folder
				m_sRootFolder = sDirectory;
			}

			g_pPlutoLogger->Write(LV_STATUS, "Adding file to watch map: filename %s", sItem.c_str());
			m_mapWatchedFiles[wd] = *it; 
		}
		catch(...)
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Failed to add notifier for file %s", sItem.c_str());
		}
    }
}
//-----------------------------------------------------------------------------------------------------
void FileNotifier::ResetWatches()
{
	m_sRootFolder = "";
	m_wdRootFolder = 0;
	m_mapWatchedFiles.clear();
}
//-----------------------------------------------------------------------------------------------------
void *BackgroundWorkerThread(void *p)
{
	FileNotifier *pFileNotifier = (FileNotifier *)p;

	while(!pFileNotifier->m_bCancelThread)
	{
		if(!pFileNotifier->m_wdRootFolder)
		{
			//we don't know yet what is the root folder
			Sleep(1000);
			continue;
		}

		g_pPlutoLogger->Write(LV_WARNING, "Background thread waking up...");

		string sRootFolder = pFileNotifier->m_sRootFolder;
		sRootFolder = StringUtils::Replace(&sRootFolder,"\\","/");  // Be sure no Windows \'s
		sRootFolder = FileUtils::ExcludeTrailingSlash(sRootFolder);
		
		list<string> listFilesOnDisk;
		FileUtils::FindDirectories(listFilesOnDisk,sRootFolder,true,false,0,sRootFolder + "/");  
		listFilesOnDisk.push_back(sRootFolder);

		for(list<string>::iterator it = listFilesOnDisk.begin(); it != listFilesOnDisk.end(); it++)
		{
			if(pFileNotifier->m_bCancelThread)
				return NULL;

			//timestamp on the disk
			string sItem = *it;
			sItem = StringUtils::Replace(&sItem,"\\","/");  // Be sure no Windows \'s
			sItem = FileUtils::ExcludeTrailingSlash(sItem);
			string sModifiedDate = pFileNotifier->GetLastModifiedDate(sItem);

			//timestamp on db
			string sDBModifiedDate;
			string sSQL;
			MYSQL_ROW row;

			sSQL = "SELECT psc_mod FROM File WHERE Path = '" + sItem + "' ORDER BY psc_mod DESC LIMIT 1";
			PlutoSqlResult result_set;
			if(
				NULL != (result_set.r = pFileNotifier->m_pDatabase_pluto_media->mysql_query_result(sSQL)) && 
				NULL != (row = mysql_fetch_row(result_set.r))
			)
				sDBModifiedDate = row[0];

			//compare timestamps
			if(sModifiedDate > sDBModifiedDate && sDBModifiedDate != "")
			{
				g_pPlutoLogger->Write(LV_WARNING, "Need to sync folder %s: on disk %s - on database %s" , sItem.c_str(), 
					sModifiedDate.c_str(), sDBModifiedDate.c_str());

				//touch records in db
				string sSQL = "UPDATE File SET psc_mod = '" + sModifiedDate + "' WHERE Path = '" + sItem + "'";
				pFileNotifier->m_pDatabase_pluto_media->threaded_mysql_query(sSQL);

				list<string> listFiles;
				listFiles.push_back(sItem);
				pFileNotifier->FireOnCreate(listFiles);
			}

			Sleep(100);
		}

		g_pPlutoLogger->Write(LV_WARNING, "Background thread going to sleep...");

		Sleep(300 * 1000); //every 5 minutes
	}

	return NULL;
}
//-----------------------------------------------------------------------------------------------------
void *INotifyWorkerThread(void *p)
{
    FileNotifier *pFileNotifier = (FileNotifier *)p;

    while(!pFileNotifier->m_bCancelThread)
    {
		while(!pFileNotifier->m_inotify.pending_events()) 
			Sleep(100); 
		
        cpp_inotify_event event = pFileNotifier->m_inotify.get_event();

		//g_pPlutoLogger->Write(LV_STATUS, "We got something: mask %d, name %s, wd %d, tostring %s",
		//	event.mask, event.name.c_str(), event.wd, event.tostring().c_str());

		bool bCreateEvent = event.mask & IN_CREATE || event.mask & IN_MOVED_TO;
		bool bDeleteEvent = event.mask & IN_DELETE || event.mask & IN_MOVED_FROM;

		if(event.mask & IN_DELETE_SELF)
		{
			if(pFileNotifier->m_wdRootFolder == event.wd)
			{
				string sRootFolder = pFileNotifier->m_sRootFolder;
				pFileNotifier->ResetWatches(); 
				g_pPlutoLogger->Write(LV_CRITICAL, "Voodoo with the root folder; someone reseted our watches. Reinitializing.");
				pFileNotifier->Watch(sRootFolder);
			}
		}
		
		if(bCreateEvent || bDeleteEvent)
        {
            PLUTO_SAFETY_LOCK(wfm, pFileNotifier->m_WatchedFilesMutex);
            string sFilename = pFileNotifier->m_mapWatchedFiles_Find(event.wd) + "/" + event.name;
            wfm.Release();

			g_pPlutoLogger->Write(LV_STATUS, "inotify: New event for %s", sFilename.c_str());

            bool bIsDir = (event.mask & IN_ISDIR) != 0;
			list<string> listFiles;

            if(bIsDir && bCreateEvent)
				pFileNotifier->Watch(sFilename);
			
			listFiles.push_back(sFilename);

			if(bCreateEvent)
				pFileNotifier->FireOnCreate(listFiles);
			else
				pFileNotifier->FireOnDelete(listFiles);
        }
    }
        
    return NULL;
}
//-----------------------------------------------------------------------------------------------------
void FileNotifier::RegisterCallbacks(FileNotifierCallback pfOnCreate, FileNotifierCallback pfOnDelete)
{
    if(!m_bCallbacksRegistered)
    {
        m_pfOnCreate = pfOnCreate;
        m_pfOnDelete = pfOnDelete;
		m_bCallbacksRegistered = true;

	    m_bCancelThread = false;

	    pthread_create(&m_INotifyWorkerThreadID, NULL, INotifyWorkerThread, (void *)this);

		if(m_pDatabase_pluto_media)
			pthread_create(&m_BackgroundWorkerThreadID, NULL, BackgroundWorkerThread, (void *)this);
    }
}
//-----------------------------------------------------------------------------------------------------
void FileNotifier::FireOnCreate(list<string> &listFiles)
{
	if(m_bCallbacksRegistered && m_pfOnCreate)
		m_pfOnCreate(listFiles);
}
//-----------------------------------------------------------------------------------------------------
void FileNotifier::FireOnDelete(list<string> &listFiles)
{
	if(m_bCallbacksRegistered && m_pfOnDelete)
		m_pfOnDelete(listFiles);
}
//-----------------------------------------------------------------------------------------------------
void FileNotifier::Run()
{
	//waiting for threads to finish
	pthread_join(m_INotifyWorkerThreadID, NULL);
	pthread_join(m_BackgroundWorkerThreadID, NULL);
}
//-----------------------------------------------------------------------------------------------------
string FileNotifier::GetLastModifiedDate(string sFolder)
{
	struct stat buf;
	memset(&buf, 0, sizeof(buf));

	string sItemWithoutTrailingSlash = FileUtils::ExcludeTrailingSlash(sFolder);
	int result = stat(sItemWithoutTrailingSlash.c_str(), &buf);
	if(!result && (buf.st_mode & S_IFDIR))
	{
		struct tm *tm;
		tm = localtime(&buf.st_mtime);   // Convert time to struct tm form 
		char acDateTime[100];
		sprintf( acDateTime, "%04d%02d%02d%02d%02d%02d",
			tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec );

		return acDateTime;
	}

	return "";
}
//-----------------------------------------------------------------------------------------------------

