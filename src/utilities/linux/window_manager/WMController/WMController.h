//
// Author : ...
//
// Changed by : Remus C.
//

// Notes : use a single instance

#ifndef __WINDOW_MANAGER_H__
#define __WINDOW_MANAGER_H__

#include <X11/Xlib.h>

#include "../wmctrl/wmctrl.h"
#include "WindowLayer.h"

#include <list>
#include <string>
using namespace std;

//-------------------------------------------------------------------------------------------------------------
class WMControllerImpl
{
public:
    WMControllerImpl();
    ~WMControllerImpl();

	bool SetVisible(const string& sWindowName, bool bVisible);
    bool SetLayer(const string& sWindowName, WindowLayer aLayer);
    bool SetPosition(const string& sWindowName, int x, int y, int w, int h);
    bool SetFullScreen(const string& sWindowName, bool bFullScreen);
    bool SetMaximized(const string& sWindowName, bool bMaximized);
    bool ActivateWindow(const string& sWindowName);

    bool ListWindows(list<WinInfo> &listWinInfo);

protected:
    WmCtrl wmctrl;

	string m_sPrimaryDestop;
	string m_sSecondaryDestop;
};

//-------------------------------------------------------------------------------------------------------------
#endif
