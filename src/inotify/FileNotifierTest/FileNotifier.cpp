#include "FileNotifier.h"
#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"
using namespace DCE;
//-----------------------------------------------------------------------------------------------------
#ifdef WIN32
enum DummyEvents { IN_ALL_EVENTS, CREATE, MOVE_TO, IN_ISDIR }; 
#endif
//-----------------------------------------------------------------------------------------------------
void *WorkerThread(void *);
//-----------------------------------------------------------------------------------------------------
FileNotifier::FileNotifier() : m_WatchedFilesMutex("watched files")
{
    m_bCallbacksRegistered = false;
    m_bCancelThread = false;

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
    FileUtils::FindFiles(listFilesOnDisk,sDirectory,"",false,false,0,"");   

    PLUTO_SAFETY_LOCK(wfm, m_WatchedFilesMutex);
    for(list<string>::iterator it = listFilesOnDisk.begin(); it != listFilesOnDisk.end(); it++)
    {
        string sItem = *it;
        int wd = m_inotify.watch(*it, IN_ALL_EVENTS);

        g_pPlutoLogger->Write(LV_STATUS, "Adding file to watch map: filename %s", sItem.c_str());
        m_mapWatchedFiles[wd] = *it; 
    }
}
//-----------------------------------------------------------------------------------------------------
void *WorkerThread(void *p)
{
    FileNotifier *pFileNotifier = (FileNotifier *)p;

    while(!pFileNotifier->m_bCancelThread)
    {
        cpp_inotify_event event = pFileNotifier->m_inotify.get_event();

        if(event.mask & CREATE || event.mask & MOVE_TO)
        {
            PLUTO_SAFETY_LOCK(wfm, pFileNotifier->m_WatchedFilesMutex);
            string sFilename = pFileNotifier->m_mapWatchedFiles_Find(event.wd);
            wfm.Release();

            bool bIsDir = (event.mask & IN_ISDIR) != 0;

            if(bIsDir)
            {

            }
            else
            {
                
            }

            g_pPlutoLogger->Write(LV_STATUS, "About to fire OnCreate event: filename %s, is dir %d",
                sFilename.c_str(), bIsDir);
        }
    }
        
    return NULL;
}
//-----------------------------------------------------------------------------------------------------
void FileNotifier::RegisterCallbacks(FileNotifierCallback *pfOnCreate, FileNotifierCallback *pfOnDelete)
{
    if(!m_bCallbacksRegistered)
    {
        m_pfOnCreate = pfOnCreate;
        m_pfOnDelete = pfOnDelete;

        m_bCancelThread = false;
        pthread_create(&m_WorkerThreadID, NULL, WorkerThread, (void *)this);
    }
}
//-----------------------------------------------------------------------------------------------------