#include <sys/ioctl.h>
#include <errno.h>

#include "inotify/inotify.h"
#include "inotify/inotify-syscalls.h"

#include "FileNotifier.h"
#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"
using namespace DCE;
//-----------------------------------------------------------------------------------------------------
#ifdef WIN32
enum DummyEvents { IN_ALL_EVENTS, IN_CREATE, IN_MOVE_TO, IN_ISDIR }; 
#endif
//-----------------------------------------------------------------------------------------------------
void *WorkerThread(void *);
//-----------------------------------------------------------------------------------------------------
FileNotifier::FileNotifier() : m_WatchedFilesMutex("watched files")
{
    m_bCallbacksRegistered = false;
    m_bCancelThread = false;

    m_pfOnCreate = NULL;
	m_pfOnDelete = NULL;
					

    m_WatchedFilesMutex.Init(NULL);
}
//-----------------------------------------------------------------------------------------------------
FileNotifier::~FileNotifier(void)
{
    m_bCancelThread = true;
}
//-----------------------------------------------------------------------------------------------------
void FileNotifier::Watch(string sDirectory)
{
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

			//g_pPlutoLogger->Write(LV_STATUS, "Adding file to watch map: filename %s", sItem.c_str());
			m_mapWatchedFiles[wd] = *it; 
		}
		catch(...)
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Failed to add notifier for file %s", sItem.c_str());
		}
    }
}
//-----------------------------------------------------------------------------------------------------
void *WorkerThread(void *p)
{
    FileNotifier *pFileNotifier = (FileNotifier *)p;

    while(!pFileNotifier->m_bCancelThread)
    {
		while(!pFileNotifier->m_inotify.pending_events()) 
			sleep(1); 
		
        cpp_inotify_event event = pFileNotifier->m_inotify.get_event();

		//g_pPlutoLogger->Write(LV_STATUS, "We got something: mask %d, name %s, wd %d, tostring %s",
		//	event.mask, event.name.c_str(), event.wd, event.tostring().c_str());

		bool bCreateEvent = event.mask & IN_CREATE || event.mask & IN_MOVED_TO;
		bool bDeleteEvent = event.mask & IN_DELETE || event.mask & IN_MOVED_FROM;
		
		if(bCreateEvent || bDeleteEvent)
        {
            PLUTO_SAFETY_LOCK(wfm, pFileNotifier->m_WatchedFilesMutex);
            string sFilename = pFileNotifier->m_mapWatchedFiles_Find(event.wd) + "/" + event.name;
            wfm.Release();

            bool bIsDir = (event.mask & IN_ISDIR) != 0;
			list<string> listFiles;

            if(bIsDir)
			{
				if(bCreateEvent)
					pFileNotifier->Watch(sFilename);

			    //FileUtils::FindDirectories(listFiles, sFilename,true,false,0,sFilename + "/");
				//FileUtils::FindFiles(listFiles, sFilename,"",true,false,0,sFilename + "/");
			}
			
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
	    pthread_create(&m_WorkerThreadID, NULL, WorkerThread, (void *)this);
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
	//let the thread to its work
	pthread_join(m_WorkerThreadID, NULL);
}
//-----------------------------------------------------------------------------------------------------
