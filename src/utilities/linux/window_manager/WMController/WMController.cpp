#include "WMController.h"

//-------------------------------------------------------------------------------------------------------------
WMControllerImpl::WMControllerImpl(): m_pDisplay(NULL), m_bVerboseEnabled(false)
{
}
//-------------------------------------------------------------------------------------------------------------
WMControllerImpl::~WMControllerImpl()
{
}
//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::Init()
{
    //printf("WMControllerImpl::Init()\n");
	m_pDisplay = XOpenDisplay(NULL);
	
	if (NULL == m_pDisplay)
	{
		printf("WMControllerImpl::Init() : Cannot open display! (did you forget to use a 'export DISPLAY' ?) \n");
		return false;
    }

    XLockDisplay(m_pDisplay);	
	init_charset(true); //use UTF8

    //printf("Done WMControllerImpl::Init()\n");
	return true;
}
//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::Fini()
{
    //fprintf(stderr, "WMControllerImpl::Fini()\n");
	if(NULL != m_pDisplay)
	{
		XUnlockDisplay(m_pDisplay);	
		XCloseDisplay(m_pDisplay);
        //fprintf(stderr, "Done WMControllerImpl::Fini()\n");
		return true;
	}

	//fprintf(stderr, "WMControllerImpl::Fini() : Cannot close NULL display!\n");
	return false;
}
//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetVisible(const string& sWindowName, bool bVisible)
{
	fprintf(stderr, "WMControllerImpl::SetVisible() : window name: %s, visible: %d\n", sWindowName.c_str(), bVisible);

	if(!Init())
		return false;

	string sParam = bVisible ? "0" : "1";

	action_window_str(
		m_pDisplay,
		't', //mode
		const_cast<char *>(sWindowName.c_str()),
		const_cast<char *>(sParam.c_str()),
		true,  //use class name
		false //full window title match
	);

	return Fini();
}

//-------------------------------------------------------------------------------------------------------------
void WMControllerImpl::SetLayerInternal(const string& sWindowName, const string& sCommand)
{
	action_window_str(
		m_pDisplay,
		'b',  //mode
		const_cast<char *>(sWindowName.c_str()),
		const_cast<char *>(sCommand.c_str()),
		true,  //use class name
		false //full window title match
	);
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetLayer(const string& sWindowName, WindowLayer aLayer)
{
	fprintf(stderr, "WMControllerImpl::SetLayer() : window name: %s, aLayer: %d\n", sWindowName.c_str(), aLayer);

	if(!Init())
		return false;

	string sParam;
	switch(aLayer)
	{
		case LayerNormal:
            SetLayerInternal(sWindowName, "remove,above");
            SetLayerInternal(sWindowName, "remove,below");
			break;

		case LayerAbove:
            SetLayerInternal(sWindowName, "remove,below");
            SetLayerInternal(sWindowName, "add,above");
			break;

		case LayerBelow:
            SetLayerInternal(sWindowName, "remove,above");
            SetLayerInternal(sWindowName, "add,below");
			break;

		default:
			fprintf(stderr, "WMControllerImpl::SetLayer() : Unknown layer!");
			return false;
	}

    return Fini();
}
//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetPosition(const string& sWindowName, int x, int y, int w, int h)
{
	fprintf(stderr, "WMControllerImpl::SetPosition() : window name: %s, position: %d %d %d %d\n", sWindowName.c_str(), x, y, w, h);

	if(!Init())
		return false;

	char pParam[128];
	sprintf(pParam, "0,%d,%d,%d,%d", x, y, w, h);
	string sParam(pParam);

	action_window_str(
		m_pDisplay,
		'e', //mode
		const_cast<char *>(sWindowName.c_str()),
		const_cast<char *>(sParam.c_str()),
		true,  //use class name
		false //full window title match
	);

	return Fini();
}
//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetFullScreen(const string& sWindowName, bool bFullScreen)
{
	fprintf(stderr, "WMControllerImpl::SetFullScreen() : window name: %s, fullscreen: %d\n", sWindowName.c_str(), bFullScreen);

	if(!Init())
		return false;

	string sParam = bFullScreen ? "add,fullscreen" : "remove,fullscreen";

	action_window_str(
		m_pDisplay,
		'b', //mode
		const_cast<char *>(sWindowName.c_str()),
		const_cast<char *>(sParam.c_str()),
		true,  //use class name
		false //full window title match
	);

	return Fini();
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetMaximized(const string& sWindowName, bool bMaximized)
{
	fprintf(stderr, "WMControllerImpl::SetMaximized() : window name: %s, maximized: %d\n", sWindowName.c_str(), bMaximized);

	if(!Init())
		return false;

	string sParam = bMaximized ? "add,maximized_horz,maximized_vert" : "remove,maximized_horz,maximized_vert";

	action_window_str(
		m_pDisplay,
		'b', //mode
		const_cast<char *>(sWindowName.c_str()),
		const_cast<char *>(sParam.c_str()),
		true,  //use class name
		false //full window title match
	);

	return Fini();
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::ActivateWindow(const string& sWindowName)
{
	fprintf(stderr, "WMControllerImpl::ActivateWindow() : window name: %s\n", sWindowName.c_str());

	if(!Init())
		return false;

	string sParam;

	action_window_str(
		m_pDisplay,
		'a', //mode
		const_cast<char *>(sWindowName.c_str()),
		const_cast<char *>(sParam.c_str()),
		true,  //use class name
		false //full window title match
	);

	return Fini();
}
//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::ListWindows(list<WinInfo> &listWinInfo)
{
	fprintf(stderr, "WMControllerImpl::ListWindows()\n");

	if(!Init())
		return false;

	list_windows(m_pDisplay, true, true, true, listWinInfo);

	return Fini();
}
//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::GetWindowParams(const string& sWindowName, string& sWindowParams)
{
	fprintf(stderr, "WMControllerImpl::GetWindowParams() : window name: %s, \n", sWindowName.c_str());
	fprintf(stderr, "WMControllerImpl::GetWindowParams() : Not implemented!");
	sWindowParams = "Not implemented!";

	return true;
}
//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::GetDesktopSize(int& x, int& y, int& w, int& h)
{
	fprintf(stderr, "WMControllerImpl::GetDesktopSize() : Not implemented!");
	x = 0;
	y = 0;
	w = 0;
	h = 0;

	return true;
}
//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::VerboseEnabled()
{
	return m_bVerboseEnabled;
}
//-------------------------------------------------------------------------------------------------------------
void WMControllerImpl::SetVerbose(bool Value)
{
	m_bVerboseEnabled = Value;
}
//-------------------------------------------------------------------------------------------------------------
