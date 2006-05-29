// Window List Manager

#include "win_list_manager.h"
#include "DCE/Logger.h"
using namespace DCE;

WinListManager::WinListManager(WMControllerImpl *pWMController, const string &sSdlWindowName)
        : m_WindowsMutex("Windows List Mutex")
        , m_pWMController(pWMController)
        , m_sSdlWindowName(sSdlWindowName)
{
    pthread_mutexattr_init( &m_WindowsMutexAttr );
    pthread_mutexattr_settype( &m_WindowsMutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
    m_WindowsMutex.Init(&m_WindowsMutexAttr);
}

WinListManager::~WinListManager()
{
    pthread_mutex_destroy(&m_WindowsMutex.mutex);
    pthread_mutexattr_destroy(&m_WindowsMutexAttr);
}

void WinListManager::ActivateSdlWindow()
{
    PLUTO_SAFETY_LOCK(cm, m_WindowsMutex);
    g_pPlutoLogger->Write(LV_WARNING, "WinListManager::ActivateSdlWindow()");
    m_pWMController->ActivateWindow(m_sSdlWindowName);
}

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
    m_pWMController->SetVisible(m_sSdlWindowName, true);
    m_pWMController->SetMaximized(m_sSdlWindowName, true);
    m_pWMController->SetLayer(m_sSdlWindowName, bExclusive ? LayerAbove : LayerBelow);
    if (bExclusive)
        ActivateSdlWindow();
    m_pWMController->SetFullScreen(m_sSdlWindowName, bExclusive);
    return;
}

void WinListManager::ShowWindow(const string &sWindowName)
{
    PLUTO_SAFETY_LOCK(cm, m_WindowsMutex);
    g_pPlutoLogger->Write(LV_WARNING, "WinListManager::ShowWindow(%s)", sWindowName.c_str());
    m_pWMController->SetVisible(sWindowName, true);
    m_listVisibleWindows.push_back(sWindowName);
}

void WinListManager::MaximizeWindow(const string &sWindowName)
{
    PLUTO_SAFETY_LOCK(cm, m_WindowsMutex);
    g_pPlutoLogger->Write(LV_WARNING, "WinListManager::MaximizeWindow(%s)", sWindowName.c_str());
	//m_pWMController->SetVisible(sWindowName, false);
	m_pWMController->SetFullScreen(sWindowName, true);
    ShowWindow(sWindowName);
}

void WinListManager::PositionWindow(const string &sWindowName, int x, int y, int w, int h)
{
    PLUTO_SAFETY_LOCK(cm, m_WindowsMutex);
    g_pPlutoLogger->Write(LV_WARNING, "WinListManager::PositionWindow(%s)", sWindowName.c_str());
    //m_pWMController->SetVisible(sWindowName, false);
    m_pWMController->SetFullScreen(sWindowName, false);
	m_pWMController->SetMaximized(sWindowName, false);
    m_pWMController->SetPosition(sWindowName, x, y, w, h);
    ShowWindow(sWindowName);
    g_pPlutoLogger->Write(LV_WARNING, "WinListManager::PositionWindow(%s) : done", sWindowName.c_str());
}

void WinListManager::HideAllWindows()
{
    PLUTO_SAFETY_LOCK(cm, m_WindowsMutex);
    g_pPlutoLogger->Write(LV_WARNING, "WinListManager::HideAllWindows()");
    for (list<string>::iterator it = m_listVisibleWindows.begin(); it != m_listVisibleWindows.end(); ++it)
    {
        m_pWMController->SetVisible(*it, false);
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
    if (! m_pWMController->ListWindows(listWinInfo))
        return false;
    for (list<WinInfo>::iterator it = listWinInfo.begin(); it !=  listWinInfo.end(); ++it)
    {
		g_pPlutoLogger->Write(LV_STATUS, "Comparing '%s' with '%s'...", it->sClassName.c_str(), sClassName.c_str());
        if (string::npos != it->sClassName.find(sClassName))
            return true;
    }
    return false;
}

bool WinListManager::HideWindow(const string &sClassName)
{
	PLUTO_SAFETY_LOCK(cm, m_WindowsMutex);
	m_pWMController->SetVisible(sClassName, false);
}

void WinListManager::GetWindows(list<WinInfo>& listWinInfo)
{
	PLUTO_SAFETY_LOCK(cm, m_WindowsMutex);
	m_pWMController->ListWindows(listWinInfo);
}
