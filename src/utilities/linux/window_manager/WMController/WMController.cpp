//
// Author : ...
//
// Changed by : Remus C.
//

#include "WMController.h"

#include "../../wrapper/wrapper_x11.h"
#include "../../../defines/define_logger.h"
#include "DCE/Logger.h"
using namespace DCE;

//-------------------------------------------------------------------------------------------------------------
WMControllerImpl::WMControllerImpl()
{
    const char *pPrimaryDesktopName = getenv("ORBITER_PRIMARY_DESKTOP");
    if(NULL != pPrimaryDesktopName)
        m_sPrimaryDestop = pPrimaryDesktopName;
    else
 		m_sPrimaryDestop = "0";

    const char *pSecondaryDesktopName = getenv("ORBITER_SECONDARY_DESKTOP");
    if(NULL != pSecondaryDesktopName)
        m_sSecondaryDestop = pSecondaryDesktopName;
    else
 		m_sSecondaryDestop = "1";

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl: primary desktop %s", m_sPrimaryDestop.c_str());
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl: secondary desktop %s", m_sSecondaryDestop.c_str());

	//Switch to primary desktop
    wmctrl.ActionCommand('s', NULL, m_sPrimaryDestop.c_str());
}

//-------------------------------------------------------------------------------------------------------------
WMControllerImpl::~WMControllerImpl()
{
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetVisible(const string& sWindowName, bool bVisible)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl::SetVisible() : window name: %s, visible: %d\n", sWindowName.c_str(), bVisible);
#endif
    return wmctrl.ActionCommand('t', sWindowName.c_str(), bVisible ? m_sPrimaryDestop.c_str() : m_sSecondaryDestop.c_str());
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetLayer(const string& sWindowName, WindowLayer aLayer)
{
    // TODO: activate the lines with ActionCommand and 'O', after the demo!
    // these should remove the need for the bogus_xterm
    // ActionCommand 'O' :  clear/set recursive the override_redirect property for a window
    // SetLayer() was broken by the alpha-patch, when xcompmgr is used
    // not activated by default, to not change anything before the demo

#ifdef DEBUG
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl::SetLayer() : window name: %s, aLayer: %d\n", sWindowName.c_str(), aLayer);
#endif

    bool bResult = true;
	switch(aLayer)
	{
		case LayerNormal:
            //bResult = bResult && wmctrl.ActionCommand('O', sWindowName.c_str(), "0");
            bResult = bResult && wmctrl.ActionCommand('b', sWindowName.c_str(), "remove,above");
            bResult = bResult && wmctrl.ActionCommand('b', sWindowName.c_str(), "remove,below");
			break;
		case LayerAbove:
            //bResult = bResult && wmctrl.ActionCommand('O', sWindowName.c_str(), "0");
            bResult = bResult && wmctrl.ActionCommand('b', sWindowName.c_str(), "remove,below");
            bResult = bResult && wmctrl.ActionCommand('b', sWindowName.c_str(), "add,above");
			break;
		case LayerBelow:
            //bResult = bResult && wmctrl.ActionCommand('O', sWindowName.c_str(), "0");
            bResult = bResult && wmctrl.ActionCommand('b', sWindowName.c_str(), "remove,above");
            bResult = bResult && wmctrl.ActionCommand('b', sWindowName.c_str(), "add,below");
			break;
		default:
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl::SetLayer() : Unknown layer!");
			return false;
	}
	return bResult;
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetPosition(const string& sWindowName, int x, int y, int w, int h)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl::SetPosition() : window name: %s, position: %d %d %d %d\n", sWindowName.c_str(), x, y, w, h);
#endif

	char sParam[128];
	sprintf(sParam, "0,%d,%d,%d,%d", x, y, w, h);
    return wmctrl.ActionCommand('e', sWindowName.c_str(), sParam);
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetFullScreen(const string& sWindowName, bool bFullScreen)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl::SetFullScreen() : window name: %s, fullscreen: %d\n", sWindowName.c_str(), bFullScreen);
#endif

    return wmctrl.ActionCommand('b', sWindowName.c_str(), bFullScreen ? "add,fullscreen" : "remove,fullscreen");
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetMaximized(const string& sWindowName, bool bMaximized)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl::SetMaximized() : window name: %s, maximized: %d\n", sWindowName.c_str(), bMaximized);
#endif

    return wmctrl.ActionCommand('b', sWindowName.c_str(), bMaximized ? "add,maximized_horz,maximized_vert" : "remove,maximized_horz,maximized_vert");
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::ActivateWindow(const string& sWindowName)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl::ActivateWindow() : window name: %s\n", sWindowName.c_str());
#endif

    if (sWindowName == "")
        return false;
    return wmctrl.ActionCommand('R', sWindowName.c_str());
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::ListWindows(list<WinInfo> &listWinInfo)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "WMControllerImpl::ListWindows()\n");
#endif

    return wmctrl.ActionCommand('l', NULL, NULL, &listWinInfo);
}

//-------------------------------------------------------------------------------------------------------------
