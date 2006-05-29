//
// Author : ...
//
// Changed by : Remus C.
//

// Notes : use a single instance
//         now: all member functions can be made static

#ifndef __WINDOW_MANAGER_H__
#define __WINDOW_MANAGER_H__

#include <X11/Xlib.h>

//#include "../../../../PlutoUtils/MultiThreadIncludes.h"
//#include "../../../../PlutoUtils/singleton.h"
//using namespace cpp;
//using namespace cpp::Threading;

#include "wmctrl.h"
#include "WindowLayer.h"

#include <map>
#include <list>
#include <string>
using namespace std;

//-------------------------------------------------------------------------------------------------------------
class WMControllerImpl
{
private:
	static bool m_bVerboseEnabled;

    /*static*/ void SetLayerInternal(Display *pDisplay, const string& sWindowName, const string& sCommand);

public:
    WMControllerImpl();
    ~WMControllerImpl();

	/*static*/ bool SetVisible(const string& sWindowName, bool bVisible);
	/*static*/ bool SetLayer(const string& sWindowName, WindowLayer aLayer);
	/*static*/ bool SetPosition(const string& sWindowName, int x, int y, int w, int h);
	/*static*/ bool SetFullScreen(const string& sWindowName, bool bFullScreen);
	/*static*/ bool SetMaximized(const string& sWindowName, bool bMaximized);
	/*static*/ bool ActivateWindow(const string& sWindowName);

	/*static*/ bool ListWindows(list<WinInfo> &listWinInfo);
	/*static*/ bool GetWindowParams(const string& sWindowName, string& sWindowParams);
	/*static*/ bool GetDesktopSize(int& x, int& y, int& w, int& h);

	static bool VerboseEnabled();
	/*static*/ void SetVerbose(bool Value);
};
//-------------------------------------------------------------------------------------------------------------
//typedef Singleton<WMControllerImpl, CreateUsingNew<WMControllerImpl>, DefaultLifetime, MultiThreaded> WMController;
//-------------------------------------------------------------------------------------------------------------

#endif
