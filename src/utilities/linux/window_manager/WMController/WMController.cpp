//
// Author : ...
//
// Changed by : Remus C.
//

#include "WMController.h"

#include "../../wrapper/wrapper_x11.h"
#include "../../../defines/define_logger.h"

bool WMControllerImpl::m_bVerboseEnabled = false;

//-------------------------------------------------------------------------------------------------------------
WMControllerImpl::WMControllerImpl()
{
	init_charset(true); //use UTF8
}
//-------------------------------------------------------------------------------------------------------------
WMControllerImpl::~WMControllerImpl()
{
}

#define NEW_DISPLAY_OR_RETURN(pDisplay, ...) \
    X11_Locker_NewDisplay x11_locker_newdisplay; \
    Display *pDisplay = x11_locker_newdisplay.GetDisplay(); \
	if (NULL == pDisplay) \
	{ \
        _LOG_ERR("Cannot open display"); \
		return __VA_ARGS__; \
    } \
    do {} while (0)

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetVisible(const string& sWindowName, bool bVisible)
{
	fprintf(stderr, "WMControllerImpl::SetVisible() : window name: %s, visible: %d\n", sWindowName.c_str(), bVisible);

    NEW_DISPLAY_OR_RETURN(pDisplay, false);

    string sParam = bVisible ? "0" : "1";

	action_window_str(
		pDisplay,
		't', //mode
		const_cast<char *>(sWindowName.c_str()),
		const_cast<char *>(sParam.c_str()),
		true,  //use class name
		false //full window title match
        );

	return true;
}

//-------------------------------------------------------------------------------------------------------------
void WMControllerImpl::SetLayerInternal(Display *pDisplay, const string& sWindowName, const string& sCommand)
{
	action_window_str(
		pDisplay,
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

    NEW_DISPLAY_OR_RETURN(pDisplay, false);

	string sParam;
	switch(aLayer)
	{
		case LayerNormal:
            SetLayerInternal(pDisplay, sWindowName, "remove,above");
            SetLayerInternal(pDisplay, sWindowName, "remove,below");
			break;

		case LayerAbove:
            SetLayerInternal(pDisplay, sWindowName, "remove,below");
            SetLayerInternal(pDisplay, sWindowName, "add,above");
			break;

		case LayerBelow:
            SetLayerInternal(pDisplay, sWindowName, "remove,above");
            SetLayerInternal(pDisplay, sWindowName, "add,below");
			break;

		default:
			fprintf(stderr, "WMControllerImpl::SetLayer() : Unknown layer!");
			return false;
	}

	return true;
}
//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetPosition(const string& sWindowName, int x, int y, int w, int h)
{
	fprintf(stderr, "WMControllerImpl::SetPosition() : window name: %s, position: %d %d %d %d\n", sWindowName.c_str(), x, y, w, h);

    NEW_DISPLAY_OR_RETURN(pDisplay, false);

	char pParam[128];
	sprintf(pParam, "0,%d,%d,%d,%d", x, y, w, h);
	string sParam(pParam);

	action_window_str(
		pDisplay,
		'e', //mode
		const_cast<char *>(sWindowName.c_str()),
		const_cast<char *>(sParam.c_str()),
		true,  //use class name
		false //full window title match
        );

	return true;
}
//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetFullScreen(const string& sWindowName, bool bFullScreen)
{
	fprintf(stderr, "WMControllerImpl::SetFullScreen() : window name: %s, fullscreen: %d\n", sWindowName.c_str(), bFullScreen);

    NEW_DISPLAY_OR_RETURN(pDisplay, false);

	string sParam = bFullScreen ? "add,fullscreen" : "remove,fullscreen";

	action_window_str(
		pDisplay,
		'b', //mode
		const_cast<char *>(sWindowName.c_str()),
		const_cast<char *>(sParam.c_str()),
		true,  //use class name
		false //full window title match
        );

	return true;
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetMaximized(const string& sWindowName, bool bMaximized)
{
	fprintf(stderr, "WMControllerImpl::SetMaximized() : window name: %s, maximized: %d\n", sWindowName.c_str(), bMaximized);

    NEW_DISPLAY_OR_RETURN(pDisplay, false);

	string sParam = bMaximized ? "add,maximized_horz,maximized_vert" : "remove,maximized_horz,maximized_vert";

	action_window_str(
		pDisplay,
		'b', //mode
		const_cast<char *>(sWindowName.c_str()),
		const_cast<char *>(sParam.c_str()),
		true,  //use class name
		false //full window title match
        );

	return true;
}

//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::ActivateWindow(const string& sWindowName)
{
	fprintf(stderr, "WMControllerImpl::ActivateWindow() : window name: %s\n", sWindowName.c_str());

    NEW_DISPLAY_OR_RETURN(pDisplay, false);

	string sParam;

	action_window_str(
		pDisplay,
		'a', //mode
		const_cast<char *>(sWindowName.c_str()),
		const_cast<char *>(sParam.c_str()),
		true,  //use class name
		false //full window title match
        );

	return true;
}
//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::ListWindows(list<WinInfo> &listWinInfo)
{
	fprintf(stderr, "WMControllerImpl::ListWindows()\n");

    NEW_DISPLAY_OR_RETURN(pDisplay, false);

	list_windows(pDisplay, true, true, true, listWinInfo);

	return true;
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
