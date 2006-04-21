// Window List Manager

#include "win_list_manager.h"
#include "DCE/Logger.h"
using namespace DCE;

WinListManager::WinListManager(const string &sSdlWindowName)
        : m_WindowsMutex("Windows List Mutex")
        , m_sSdlWindowName(sSdlWindowName)
{
    pthread_mutexattr_init( &m_WindowsMutexAttr );
    pthread_mutexattr_settype( &m_WindowsMutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
    m_WindowsMutex.Init(&m_WindowsMutexAttr);
};

WinListManager::~WinListManager()
{
    pthread_mutex_destroy(&m_WindowsMutex.mutex);
    pthread_mutexattr_destroy(&m_WindowsMutexAttr);
};

void WinListManager::ShowSdlWindow(bool bExclusive)
{
    PLUTO_SAFETY_LOCK(cm, m_WindowsMutex);
    g_pPlutoLogger->Write(LV_WARNING, "WinListManager::ShowSdlWindow(%s)", bExclusive ? "true" : "false");
    for (list<string>::iterator it = m_listVisibleWindows.begin(); it != m_listVisibleWindows.end(); ++it)
    {
        g_pPlutoLogger->Write(LV_WARNING, "List Windows : %s", (*it).c_str());
    }
    // when Orbiter is fullscreen no other dialog can be on top of
    // it, so it will be maximized instead
    WMController::Instance().SetVisible(m_sSdlWindowName, true);
    WMController::Instance().SetMaximized(m_sSdlWindowName, true);
    WMController::Instance().SetLayer(m_sSdlWindowName, bExclusive ? LayerAbove : LayerBelow);
    WMController::Instance().SetFullScreen(m_sSdlWindowName, bExclusive);
    return;
}

void WinListManager::ShowWindow(const string &sWindowName)
{
    PLUTO_SAFETY_LOCK(cm, m_WindowsMutex);
    g_pPlutoLogger->Write(LV_WARNING, "WinListManager::ShowWindow(%s)", sWindowName.c_str());
    WMController::Instance().SetVisible(sWindowName, true);
    m_listVisibleWindows.push_back(sWindowName);
}

void WinListManager::MaximizeWindow(const string &sWindowName)
{
    PLUTO_SAFETY_LOCK(cm, m_WindowsMutex);
    g_pPlutoLogger->Write(LV_WARNING, "WinListManager::MaximizeWindow(%s)", sWindowName.c_str());
	WMController::Instance().SetVisible(sWindowName, false);
	WMController::Instance().SetFullScreen(sWindowName, true);
    ShowWindow(sWindowName);
}

void WinListManager::PositionWindow(const string &sWindowName, int x, int y, int w, int h)
{
    PLUTO_SAFETY_LOCK(cm, m_WindowsMutex);
    g_pPlutoLogger->Write(LV_WARNING, "WinListManager::PositionWindow(%s)", sWindowName.c_str());
    WMController::Instance().SetVisible(sWindowName, false);
    WMController::Instance().SetFullScreen(sWindowName, false);
    WMController::Instance().SetPosition(sWindowName, x, y, w, h);
    ShowWindow(sWindowName);
    g_pPlutoLogger->Write(LV_WARNING, "Done WinListManager::PositionWindow(%s)", sWindowName.c_str());
}

void WinListManager::HideAllWindows()
{
    PLUTO_SAFETY_LOCK(cm, m_WindowsMutex);
    g_pPlutoLogger->Write(LV_WARNING, "WinListManager::HideAllWindows()");
    for (list<string>::iterator it = m_listVisibleWindows.begin(); it != m_listVisibleWindows.end(); ++it)
    {
        WMController::Instance().SetVisible(*it, false);
    }
    m_listVisibleWindows.clear();
}

string WinListManager::GetExternApplicationName()
{
    PLUTO_SAFETY_LOCK(cm, m_WindowsMutex);
    return m_sExternApplicationName;
}

void WinListManager::SetExternApplicationName(const string &sWindowName)
{
    g_pPlutoLogger->Write(LV_WARNING, "WinListManager::SetExternApplicationName(%s)", sWindowName.c_str());
    PLUTO_SAFETY_LOCK(cm, m_WindowsMutex);
    m_sExternApplicationName = sWindowName;
}

void WinListManager::GetExternApplicationPosition(PlutoRectangle &coord)
{
    PLUTO_SAFETY_LOCK(cm, m_WindowsMutex);
    coord = m_coordExternalApplication;
}

void WinListManager::SetExternApplicationPosition(const PlutoRectangle &coord)
{
    g_pPlutoLogger->Write(LV_WARNING, "WinListManager::SetExternApplicationPosition(), name=%s", m_sExternApplicationName.c_str());
    PLUTO_SAFETY_LOCK(cm, m_WindowsMutex);
    m_coordExternalApplication = coord;
}

bool WinListManager::IsEmpty()
{
    PLUTO_SAFETY_LOCK(cm, m_WindowsMutex);
    return (m_listVisibleWindows.size() == 0);
}

bool WinListManager::IsWindowAvailable(const string &sClassName)
{
    PLUTO_SAFETY_LOCK(cm, m_WindowsMutex);
    list<WinInfo> listWinInfo;
    if (! WMController::Instance().ListWindows(listWinInfo))
        return false;
    for (list<WinInfo>::iterator it = listWinInfo.begin(); it !=  listWinInfo.end(); ++it)
    {
        if (it->sClassName == sClassName)
            return true;
    }
    return false;
}
