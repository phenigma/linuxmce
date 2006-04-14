// Window List Manager

#ifndef __WM_LIST_MANAGER__
#define __WM_LIST_MANAGER__

#include "utilities/linux/window_manager/WMController/WMController.h"
#include <string>
#include <list>
using namespace std;

class WinListManager
{
public:
    WinListManager(const string &sSdlWindowName);
    ~WinListManager();

    void ShowSdlWindow(bool bExclusive);

    void ShowWindow(const string &sWindowName);
    void MaximizeWindow(const string &sWindowName);
    void PositionWindow(const string &sWindowName, int x, int y, int w, int h);
    void HideAllWindows();

    string GetExternApplicationName();
    void SetExternApplicationName(const string &sName);

    bool IsEmpty();

protected:
    pthread_mutexattr_t m_WindowsMutexAttr;
	pluto_pthread_mutex_t m_WindowsMutex;

    list<string> m_listVisibleWindows;
    string m_sExternApplicationName;
    string m_sSdlWindowName;
};

#endif
// __WM_LIST_MANAGER__
