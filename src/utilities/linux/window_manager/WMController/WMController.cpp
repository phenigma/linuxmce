//
// Author : ...
//
// Changed by : Remus C.
//

#include "WMController.h"

#include "../../wrapper/wrapper_x11.h"
#include "../../../defines/define_logger.h"

//-------------------------------------------------------------------------------------------------------------
WMControllerImpl::WMControllerImpl()
{
}

//-------------------------------------------------------------------------------------------------------------
WMControllerImpl::~WMControllerImpl()
{
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetVisible(const string& sWindowName, bool bVisible)
{
	fprintf(stderr, "WMControllerImpl::SetVisible() : window name: %s, visible: %d\n", sWindowName.c_str(), bVisible);
    return wmctrl.ActionCommand('t', sWindowName.c_str(), bVisible ? "0" : "1");
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetLayer(const string& sWindowName, WindowLayer aLayer)
{
	fprintf(stderr, "WMControllerImpl::SetLayer() : window name: %s, aLayer: %d\n", sWindowName.c_str(), aLayer);
    bool bResult = true;
	switch(aLayer)
	{
		case LayerNormal:
            bResult = bResult && wmctrl.ActionCommand('b', sWindowName.c_str(), "remove,above");
            bResult = bResult && wmctrl.ActionCommand('b', sWindowName.c_str(), "remove,below");
			break;
		case LayerAbove:
            bResult = bResult && wmctrl.ActionCommand('b', sWindowName.c_str(), "remove,below");
            bResult = bResult && wmctrl.ActionCommand('b', sWindowName.c_str(), "add,above");
			break;
		case LayerBelow:
            bResult = bResult && wmctrl.ActionCommand('b', sWindowName.c_str(), "remove,above");
            bResult = bResult && wmctrl.ActionCommand('b', sWindowName.c_str(), "add,below");
			break;
		default:
			fprintf(stderr, "WMControllerImpl::SetLayer() : Unknown layer!");
			return false;
	}
	return bResult;
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetPosition(const string& sWindowName, int x, int y, int w, int h)
{
	fprintf(stderr, "WMControllerImpl::SetPosition() : window name: %s, position: %d %d %d %d\n", sWindowName.c_str(), x, y, w, h);
	char sParam[128];
	sprintf(sParam, "0,%d,%d,%d,%d", x, y, w, h);
    return wmctrl.ActionCommand('e', sWindowName.c_str(), sParam);
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetFullScreen(const string& sWindowName, bool bFullScreen)
{
	fprintf(stderr, "WMControllerImpl::SetFullScreen() : window name: %s, fullscreen: %d\n", sWindowName.c_str(), bFullScreen);
    return wmctrl.ActionCommand('b', sWindowName.c_str(), bFullScreen ? "add,fullscreen" : "remove,fullscreen");
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetMaximized(const string& sWindowName, bool bMaximized)
{
	fprintf(stderr, "WMControllerImpl::SetMaximized() : window name: %s, maximized: %d\n", sWindowName.c_str(), bMaximized);
    return wmctrl.ActionCommand('b', sWindowName.c_str(), bMaximized ? "add,maximized_horz,maximized_vert" : "remove,maximized_horz,maximized_vert");
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::ActivateWindow(const string& sWindowName)
{
	fprintf(stderr, "WMControllerImpl::ActivateWindow() : window name: %s\n", sWindowName.c_str());
    if (sWindowName == "")
        return false;
    return wmctrl.ActionCommand('R', sWindowName.c_str());
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::ListWindows(list<WinInfo> &listWinInfo)
{
	fprintf(stderr, "WMControllerImpl::ListWindows()\n");
    return wmctrl.ActionCommand('l', NULL, NULL, &listWinInfo);
}

//-------------------------------------------------------------------------------------------------------------
