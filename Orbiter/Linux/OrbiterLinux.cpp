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
    openDisplay();

    //reinitGraphics();
}

OrbiterLinux::~OrbiterLinux()
{
    closeDisplay();
}

void OrbiterLinux::reinitGraphics()
{
    if ( ! XServerDisplay && ! openDisplay() )
        return;

    commandRatPoison(":desktop off");
    SDL_WM_SetCaption(m_strWindowName.c_str(), "");
    commandRatPoison(string(":select ") + m_strWindowName);
    commandRatPoison(":desktop on");
    commandRatPoison(":keystodesktop on");
    commandRatPoison(":keybindings off");
    setDesktopVisible(false);

    OrbiterCallBack callback = (OrbiterCallBack)&OrbiterLinux::setInputFocusToMe;
    CallMaintenanceInTicks( 3000, callback, NULL, true );
}

void OrbiterLinux::setInputFocusToMe(void *)
{
    if ( ! m_bYieldInput )
        commandRatPoison(":keystodesktop on");

    CallMaintenanceInTicks( 7000, (OrbiterCallBack)&OrbiterLinux::setInputFocusToMe, NULL, true ); // do this every 7 seconds
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

    currentScreen = XDefaultScreen(XServerDisplay);
    m_nDesktopWidth = DisplayWidth(XServerDisplay, currentScreen);
    m_nDesktopHeight = DisplayHeight(XServerDisplay, currentScreen);

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
        commandRatPoison(":keystodesktop on");
    else
    {
        commandRatPoison(":keystodesktop off");
        // patch the rectangle to match the actual resolution
        width = m_nDesktopWidth;
        height = m_nDesktopHeight;
    }

    g_pPlutoLogger->Write(LV_STATUS, "Resizing desktop to (%d, %d) and dimensions [%dx%d]", x, y, width, height);

    stringstream commandLine;

    commandLine << ":set padding " << x << " " << y << " "
                << m_nDesktopWidth - x - width << " " << m_nDesktopHeight - y - height;

    commandRatPoison(commandLine.str());

/*
    commandLine.str("");
    commandLine << ":redisplay";
    commandRatPoison(commandLine.str());
*/
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

void OrbiterLinux::Initialize(GraphicType Type)
{
    OrbiterSDL::Initialize(Type);
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
// void OrbiterLinux::Initialize(GraphicType Type)
// {
//     OrbiterSDL::Initialize(Type);
// }



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

