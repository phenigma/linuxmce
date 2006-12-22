// Window List Manager

#ifndef __WM_LIST_MANAGER__
#define __WM_LIST_MANAGER__

#include "SerializeClass/ShapesColors.h"
#include "utilities/linux/window_manager/WMController/WMController.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include <string>
#include <list>
using namespace std;
//-----------------------------------------------------------------------------------------------------
class WindowContext;
//-----------------------------------------------------------------------------------------------------
typedef map<string, WindowContext> WindowsContext;
//-----------------------------------------------------------------------------------------------------
class WinListManager
{
public:
    WinListManager(const string &sSdlWindowName);
    ~WinListManager();

    void ActivateWindow(const string& sWindowsName);
    void ActivateSdlWindow();

    void ShowSdlWindow(bool bExclusive, bool bYieldInput);

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
	void SetSdlWindowVisibility(bool bValue);
	void ResetOrbiterWindow();

	void SetLayer(const string &sClassName, WindowLayer layer);

	void ApplyContext();
	
private:

	WindowsContext m_CurrentContext;
	WindowsContext m_PendingContext;

	WindowContext& PendingContext(string sWindowName);

    pthread_mutexattr_t m_WindowsMutexAttr;
	pluto_pthread_mutex_t m_WindowsMutex;

	string m_sExternApplicationName;
    PlutoRectangle m_coordExternalApplication;
    unsigned long m_pidExternalApplication;

    
	bool m_bHideSdlWindow;

	//internal helpers methods
	void Internal_ActivateWindow(const string& sWindowsName);
	void Internal_ActivateSdlWindow();
	void Internal_ShowSdlWindow(bool bExclusive, bool bYieldInput);
	void Internal_ShowWindow(const string &sWindowName);
	void Internal_MaximizeWindow(const string &sWindowName);
	void Internal_PositionWindow(const string &sWindowName, int x, int y, int w, int h);
	bool Internal_HideWindow(const string &sClassName);
	void Internal_SetSdlWindowVisibility(bool bValue);
	void Internal_ResetOrbiterWindow();

private:
	
	string m_sSdlWindowName;
	WMControllerImpl *m_pWMController;

};

#endif
// __WM_LIST_MANAGER__
