/*
 OrbiterLinux

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "DCE/Logger.h"
#include "OrbiterLinux.h"

#include <iomanip>
#include <sstream>
#include <sys/time.h>

#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/keysym.h>

#include "pluto_main/Define_Button.h"

using namespace std;

OrbiterLinux::OrbiterLinux(int DeviceID,
               string ServerAddress, string sLocalDirectory,
               bool bLocalMode,
               int nImageWidth, int nImageHeight)
: OrbiterSDL(DeviceID, ServerAddress, sLocalDirectory, bLocalMode, nImageWidth, nImageHeight),
    // defaults
    m_strWindowName("pluto-linux-orbiter"),
    m_strDisplayName(getenv("DISPLAY")),

    // initializations
    desktopInScreen(0),
    XServerDisplay(NULL)
{
	XInitThreads();
	openDisplay();

	SDL_WM_SetCaption(m_strWindowName.c_str(), "");

	m_pRecordHandler = new XRecordExtensionHandler(m_strDisplayName);
}

OrbiterLinux::~OrbiterLinux()
{
	delete m_pRecordHandler;
	closeDisplay();
}

void OrbiterLinux::reinitGraphics()
{
    if ( ! XServerDisplay && ! openDisplay() )
        return;

	commandRatPoison(":set winname class");
    commandRatPoison(":desktop off");

	SDL_WM_SetCaption(m_strWindowName.c_str(), "");
    commandRatPoison(string(":select ") + m_strWindowName);
    commandRatPoison(":desktop on");
    commandRatPoison(":keystodesktop on");
    commandRatPoison(":keybindings off");
    setDesktopVisible(false);

    OrbiterCallBack callback = (OrbiterCallBack)&OrbiterLinux::setInputFocusToMe;
    CallMaintenanceInMiliseconds( 3000, callback, NULL, true );
}

void OrbiterLinux::setInputFocusToMe(void *)
{
    if ( ! m_bYieldInput )
        commandRatPoison(":keystodesktop on");

    CallMaintenanceInMiliseconds( 7000, (OrbiterCallBack)&OrbiterLinux::setInputFocusToMe, NULL, true ); // do this every 7 seconds
}

void OrbiterLinux::setWindowName(string strWindowName)
{
    m_strWindowName = strWindowName;
}

void OrbiterLinux::setDisplayName(string strDisplayName)
{
    m_strDisplayName = strDisplayName;
}

bool OrbiterLinux::openDisplay()
{
	XServerDisplay = XOpenDisplay(m_strDisplayName.c_str());

    int currentScreen;

    if ( XServerDisplay == NULL )
        return false;

	XLockDisplay(XServerDisplay);
    currentScreen = XDefaultScreen(XServerDisplay);
    m_nDesktopWidth = DisplayWidth(XServerDisplay, currentScreen);
    m_nDesktopHeight = DisplayHeight(XServerDisplay, currentScreen);
	XUnlockDisplay(XServerDisplay);
    return true;
}

bool OrbiterLinux::closeDisplay()
{
    if ( XServerDisplay )
        XCloseDisplay(XServerDisplay);

    XServerDisplay = NULL;

    return true;
}


Display *OrbiterLinux::getDisplay()
{
    if ( ! XServerDisplay )
        openDisplay();

    return XServerDisplay;
}

Window OrbiterLinux::getWindow()
{
	return 0;
}

bool OrbiterLinux::RenderDesktop(DesignObj_Orbiter *pObj, PlutoRectangle rectTotal)
{
    vector<int> vectButtonMaps;
    GetButtonsInObject(pObj,vectButtonMaps);

    resizeMoveDesktop(rectTotal.Left(), rectTotal.Top(),
            rectTotal.Right() - rectTotal.Left(),
            rectTotal.Bottom() - rectTotal.Top());

    desktopInScreen = true;
    return true;
}

// public interface implementations below
bool OrbiterLinux::resizeMoveDesktop(int x, int y, int width, int height)
{
    if ( ! m_bYieldInput )
	{
		m_pRecordHandler->enableRecording(this, false);
		commandRatPoison(":keystodesktop on");
	}
    else
    {
        commandRatPoison(":keystodesktop off");

		// patch the rectangle to match the actual resolution
        width = m_nDesktopWidth;
        height = m_nDesktopHeight;
		m_pRecordHandler->enableRecording(this);
    }

//    g_pPlutoLogger->Write(LV_STATUS, "Resizing desktop to (%d, %d) and dimensions [%dx%d]", x, y, width, height);

    stringstream commandLine;

    commandLine << ":set padding " << x << " " << y << " "
                << m_nDesktopWidth - x - width << " " << m_nDesktopHeight - y - height;

    commandRatPoison(commandLine.str());
    commandRatPoison(":redisplay");

    return true;
}

bool OrbiterLinux::setDesktopVisible(bool visible)
{
    if ( ! visible )
        return resizeMoveDesktop(m_nDesktopWidth, m_nDesktopHeight, 10, 10);
    else
        return resizeMoveDesktop(0, 0, m_nDesktopWidth - m_nImageWidth, m_nDesktopHeight - m_nImageHeight);
}

// void OrbiterLinux::bootNestedXServer()
// {
//     fixNestedDisplayName();
//
//     g_pPlutoLogger->Write(LV_STATUS,
//                 "Launching Nested XServer for display %s running on the display %s",
//                 m_strDesktopDisplayName.c_str(),
//                 m_strDisplayName.c_str());
//
//     stringstream strGeometry;
//
//     strGeometry << m_nImageWidth << "x" << m_nImageHeight;

/*    char *const xnest[] =
    {
        "/usr/bin/X11/Xnest",
        (char *)m_strDesktopDisplayName.c_str(),
        "-display", (char *)m_strDisplayName.c_str(),
        "-name", (char *)m_strDesktopWindowName.c_str(),
        "-geometry", (char *)strGeometry.str().c_str(),
        "+kb",
        "-bw", "0",
        "-ac",
        0
    };

    char *const ratpoisonArgs[] =
    {
        "/usr/bin/ratpoison",
        "-d",
        (char*)m_strDesktopDisplayName.c_str(),*/
//         "-display",
//         (char*)m_strDesktopDisplayName.c_str(),
//         0
//
//     };

//     forkAndWait( xnest, 4 );
//     forkAndWait( ratpoisonArgs, 2 );
// }

void OrbiterLinux::Initialize(GraphicType Type, int iPK_Room, int iPK_EntertainArea)
{
    OrbiterSDL::Initialize(Type,iPK_Room,iPK_EntertainArea);
    reinitGraphics();
}

void OrbiterLinux::RenderScreen()
{
    if ( XServerDisplay == NULL && ! openDisplay() )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Couldn't open the display: \"%s\"", m_strDisplayName.c_str());

        return;
    }

    desktopInScreen = false;


    OrbiterSDL::RenderScreen();

    if ( false == desktopInScreen )
        setDesktopVisible(false);

    XFlush(XServerDisplay);
}

/**
void OrbiterLinux::RenderObject(
        DesignObj_Controller *pObj, DesignObj_Controller *pObjAttr,
        int XOffset, int YOffset)
{
    if ( pObj->m_ObjectType == C_DESIGNOBJTYPE_APP_DESKTOP_CONST )
    {
        desktopInScreen = true;

        resizeMoveDesktop(
            pObj->m_rPosition.Left(),
            pObj->m_rPosition.Top(),
            pObj->m_rPosition.Width,
            pObj->m_rPosition.Height);

        return;
    }

    ControllerImageSDL::RenderObject(pObj, pObjAttr, XOffset, YOffset);
}
*/


/*!
    \fn OrbiterLinux::fixNestedDisplayName()
 */
/*
void OrbiterLinux::fixNestedDisplayName()
{
    if ( m_strDesktopDisplayName != "" )
        return;

    string strHost;
    string strDisplay;
    string strScreen;
    string strTemp;

    int temp;
    g_pPlutoLogger->Write(LV_STATUS, "Fixing nested display name location!");

    if ( XServerDisplay == NULL && openDisplay() == false )
        return;

    strTemp = XDisplayString(XServerDisplay);

    temp =  strTemp.find(':');
    if ( temp != -1 )
        strHost = strTemp.substr(0, temp);

    strTemp = strTemp.substr(temp + 1);

    temp = strTemp.find('.');

    if ( temp != -1 )
    {
        strScreen = strTemp.substr(temp);
        strTemp = strTemp.substr(0, temp);
    }

    strDisplay = strTemp;

    temp = atoi(strDisplay.c_str());

    stringstream strStream;

    strStream << strHost <<  ":" << temp + 1 << strScreen;

    m_strDesktopDisplayName = strStream.str();

    g_pPlutoLogger->Write(LV_STATUS, "Using %s as the DISPLAY var for the nested server!", m_strDesktopDisplayName.c_str());
*/


bool OrbiterLinux::PreprocessEvent(Orbiter::Event &event)
{
	if ( event.type != Orbiter::Event::BUTTON_DOWN && event.type != Orbiter::Event::BUTTON_UP )
		return false;

	XKeyEvent  kevent;
	KeySym   keysym;
	char   buf[1];

	kevent.type = KeyPress;
	kevent.display = getDisplay();
	kevent.state = 0;
	kevent.keycode = event.data.button.m_iPK_Button;;
	XLookupString(&kevent, buf, sizeof(buf), &keysym, 0);

	switch ( keysym )
	{
		case XK_F1:		event.data.button.m_iPK_Button = BUTTON_F1_CONST; break;
		case XK_F2:		event.data.button.m_iPK_Button = BUTTON_F2_CONST; break;
		case XK_F3:		event.data.button.m_iPK_Button = BUTTON_F3_CONST; break;
		case XK_F4:		event.data.button.m_iPK_Button = BUTTON_F4_CONST; break;
		case XK_F5:		event.data.button.m_iPK_Button = BUTTON_F5_CONST; break;

		case XK_0: case XK_KP_0: 	event.data.button.m_iPK_Button = BUTTON_0_CONST; break;
		case XK_1: case XK_KP_1: 	event.data.button.m_iPK_Button = BUTTON_1_CONST; break;
		case XK_2: case XK_KP_2: 	event.data.button.m_iPK_Button = BUTTON_2_CONST; break;
		case XK_3: case XK_KP_3: 	event.data.button.m_iPK_Button = BUTTON_3_CONST; break;
		case XK_4: case XK_KP_4: 	event.data.button.m_iPK_Button = BUTTON_4_CONST; break;
		case XK_5: case XK_KP_5: 	event.data.button.m_iPK_Button = BUTTON_5_CONST; break;
		case XK_6: case XK_KP_6: 	event.data.button.m_iPK_Button = BUTTON_6_CONST; break;
		case XK_7: case XK_KP_7: 	event.data.button.m_iPK_Button = BUTTON_7_CONST; break;
		case XK_8: case XK_KP_8: 	event.data.button.m_iPK_Button = BUTTON_8_CONST; break;
		case XK_9: case XK_KP_9: 	event.data.button.m_iPK_Button = BUTTON_9_CONST; break;

		case XK_Up:			event.data.button.m_iPK_Button = BUTTON_Up_Arrow_CONST; break;
		case XK_Down:		event.data.button.m_iPK_Button = BUTTON_Down_Arrow_CONST; break;
		case XK_Left:		event.data.button.m_iPK_Button = BUTTON_Left_Arrow_CONST; break;
		case XK_Right:		event.data.button.m_iPK_Button = BUTTON_Right_Arrow_CONST; break;

		default:
			if ( XK_a <= keysym && keysym <= XK_z )
				event.data.button.m_iPK_Button = BUTTON_a_CONST + keysym - XK_a;
			else if ( XK_A <= keysym && keysym <= XK_Z )
				event.data.button.m_iPK_Button = BUTTON_A_CONST + keysym - XK_A;
			else
				event.type = Orbiter::Event::NOT_PROCESSED;
	}

	g_pPlutoLogger->Write(LV_STATUS, "The keysym was %d, the final event type %d", keysym, event.type);
}
