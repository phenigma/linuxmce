// Window List Manager

#ifndef __WM_LIST_MANAGER__
#define __WM_LIST_MANAGER__

#include "SerializeClass/ShapesColors.h"
#include "utilities/linux/window_manager/WMController/WMController.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include <string>
#include <list>
using namespace std;

class WinListManager
{
public:
    WinListManager(WMControllerImpl *pWMController, const string &sSdlWindowName);
    ~WinListManager();

    void ActivateSdlWindow();

    void ShowSdlWindow(bool bExclusive);

    void ShowWindow(const string &sWindowName);
    void MaximizeWindow(const string &sWindowName);
    void PositionWindow(const string &sWindowName, int x, int y, int w, int h);
    void HideAllWindows();

    string GetExternApplicationName();
    void SetExternApplicationName(const string &sName);

    void GetExternApplicationPosition(PlutoRectangle &coord);
    void SetExternApplicationPosition(const PlutoRectangle &coord);

    bool IsEmpty();

    bool IsWindowAvailable(const string &sClassName);
	bool HideWindow(const string &sClassName);

	void GetWindows(list<WinInfo>& listWinInfo);

protected:
    pthread_mutexattr_t m_WindowsMutexAttr;
	pluto_pthread_mutex_t m_WindowsMutex;

    list<string> m_listVisibleWindows;

    string m_sExternApplicationName;
    PlutoRectangle m_coordExternalApplication;
    unsigned long m_pidExternalApplication;

    WMControllerImpl *m_pWMController;
    string m_sSdlWindowName;
};

#endif
// __WM_LIST_MANAGER__
