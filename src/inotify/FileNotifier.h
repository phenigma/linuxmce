#ifndef __FILE_NOTIFIER_H__
#define __FILE_NOTIFIER_H__
//-----------------------------------------------------------------------------------------------------
#include <pthread.h>
#include <memory>
#include <list>
#include <map>
using namespace std;
//-----------------------------------------------------------------------------------------------------
#include "inotify/inotify_class.h"
#include "PlutoUtils/MultiThreadIncludes.h"
//-----------------------------------------------------------------------------------------------------
typedef void (*FileNotifierCallback)(list<string> &listFiles);
//-----------------------------------------------------------------------------------------------------
//   Class FileNotifier (wrapper for inotifier):
//     - cross-platform
//     - recursive
//     - event driven (using callbacks registered by the user class)
//-----------------------------------------------------------------------------------------------------
class FileNotifier 
{
private:
    FileNotifierCallback m_pfOnCreate;
    FileNotifierCallback m_pfOnDelete;

    bool m_bCallbacksRegistered;
    pthread_t m_WorkerThreadID;
    map<int, string> m_mapWatchedFiles;

public:
    FileNotifier(void); 
    ~FileNotifier(void);

    void RegisterCallbacks(FileNotifierCallback pfOnCreate, FileNotifierCallback pfOnDelete);
    void Watch(string sDirectory);
	void Run();

    bool m_bCancelThread;
    inotify m_inotify;
	
	int m_wdRootFolder;
	string m_sRootFolder;
	void ResetWatches();

    pluto_pthread_mutex_t m_WatchedFilesMutex;
    string m_mapWatchedFiles_Find(int wd)	
    { 
        map<int, string>::iterator it = m_mapWatchedFiles.find(wd); 
        return it == m_mapWatchedFiles.end() ? NULL : (*it).second; 
    }

	void FireOnCreate(list<string> &listFiles);
	void FireOnDelete(list<string> &listFiles);
};
//-----------------------------------------------------------------------------------------------------
#endif //__FILE_NOTIFIER_H__
