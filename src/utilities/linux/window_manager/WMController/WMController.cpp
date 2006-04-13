#include "WMController.h"
#include "wmctrl.h"

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
	m_pDisplay = XOpenDisplay(NULL);
	
	if (NULL == m_pDisplay) 
	{
		printf("Cannot open display! (did you forget to use a 'export DISPLAY' ?) \n");
		return false;
    }
	
	init_charset(true); //use UTF8 
	
	return true;
}
//-------------------------------------------------------------------------------------------------------------	
bool WMControllerImpl::Fini()
{
	if(NULL != m_pDisplay)
	{
		XCloseDisplay(m_pDisplay);
		return true;
	}
	
	printf("Cannot close NULL display!\n");
	return false;
}
//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetVisible(const string& sWindowName, bool bVisible)
{
	printf("SetVisible, window name: %s, visible: %d\n", sWindowName.c_str(), bVisible);

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
bool WMControllerImpl::SetLayer(const string& sWindowName, WindowLayer aLayer)
{
	printf("SetLayer, window name: %s, aLayer: %d\n", sWindowName.c_str(), aLayer);

	if(!Init())
		return false;
	
	string sParam;
	switch(aLayer)
	{
		case wlNormal:
			sParam = "remove,above,below";
			break;
			
		case wlAbove:
			sParam = "add,above";
			break;
			
		case wlBelow:
			sParam = "add,below";
			break;
		
		default:
			printf("Unknown layer!");
			return false;
	}
		
	action_window_str(
		m_pDisplay, 
		'b',  //mode
		const_cast<char *>(sWindowName.c_str()), 
		const_cast<char *>(sParam.c_str()),
		true,  //use class name
		false //full window title match
	);
		
	return Fini();
}
//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::SetPosition(const string& sWindowName, int x, int y, int w, int h)
{
	printf("SetPosition, window name: %s, position: %d %d %d %d\n", sWindowName.c_str(), x, y, w, h);

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
	printf("SetFullScreen, window name: %s, fullscreen: %d\n", sWindowName.c_str(), bFullScreen);

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
bool WMControllerImpl::ActivateWindow(const string& sWindowName)
{
	printf("ActivateWindow, window name: %s\n", sWindowName.c_str());

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
bool WMControllerImpl::ListWindows()
{
	printf("ListWindows\n");

	if(!Init())
		return false;

	list_windows(m_pDisplay, true, true, true);
		
	return Fini();
}
//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::GetWindowParams(const string& sWindowName, string& sWindowParams)
{
	printf("GetWindowParams, window name: %s, \n", sWindowName.c_str());
	printf("Not implemented!");
	sWindowParams = "Not implemented!";
	
	return true;
}
//-------------------------------------------------------------------------------------------------------------
bool WMControllerImpl::GetDesktopSize(int& x, int& y, int& w, int& h)
{
	printf("Not implemented!");
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