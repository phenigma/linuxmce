#ifndef __WINDOW_MANAGER_H__
#define __WINDOW_MANAGER_H__

#include <X11/Xlib.h>

#include "../../../../../PlutoUtils/MultiThreadIncludes.h"
#include "../../../../../PlutoUtils/singleton.h"
using namespace cpp;
using namespace cpp::Threading;

#include <map>
#include <string>
using namespace std;
//-------------------------------------------------------------------------------------------------------------
enum WindowLayer
{
	wlNormal,
	wlBelow,
	wlAbove,
	wlUnknown
};
//-------------------------------------------------------------------------------------------------------------
class WMControllerImpl
{
private:
	
	bool m_bVerboseEnabled;
	Display *m_pDisplay;

	//helper functions
	bool Init();
	bool Fini();

public:
	WMControllerImpl();
	~WMControllerImpl();

	bool SetVisible(const string& sWindowName, bool bVisible);
	bool SetLayer(const string& sWindowName, WindowLayer aLayer);
	bool SetPosition(const string& sWindowName, int x, int y, int w, int h);
	bool SetFullScreen(const string& sWindowName, bool bFullScreen);
	bool ActivateWindow(const string& sWindowName);

	bool ListWindows();
	bool GetWindowParams(const string& sWindowName, string& sWindowParams);
	bool GetDesktopSize(int& x, int& y, int& w, int& h);
	
	bool VerboseEnabled();
	void SetVerbose(bool Value);
};
//-------------------------------------------------------------------------------------------------------------
typedef Singleton<WMControllerImpl, CreateUsingNew<WMControllerImpl>, DefaultLifetime, MultiThreaded> WMController;
//-------------------------------------------------------------------------------------------------------------
#endif

