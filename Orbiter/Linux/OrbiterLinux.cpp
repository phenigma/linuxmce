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
	//commandRatPoison(":keybindings off");
	commandRatPoison(":keystodesktop on");
    setDesktopVisible(false);
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

// int left, int top, int right, int bottom)
// void OrbiterLinux::sendCommandToRatpoison(string command)
// {
//     if ( ! XServerDisplay && ! openDisplay() )
//         return;
//
//     g_pPlutoLogger->Write(LV_STATUS, "Instructing ratpoison to do this: %s", command.c_str());
//
//     rp_command          = XInternAtom (XServerDisplay, "RP_COMMAND", True);
//     rp_command_request  = XInternAtom (XServerDisplay, "RP_COMMAND_REQUEST", True);
//     rp_command_result   = XInternAtom (XServerDisplay, "RP_COMMAND_RESULT", True);
//
//     if ( rp_command == None || rp_command_request == None || rp_command_result == None )
//     {
//         g_pPlutoLogger->Write(LV_WARNING, "Ratpoison window manager does not seem to be running on this server!");
//         return;
//     }
//
//     Window commandWindow = 0;
//
//     commandWindow = XCreateSimpleWindow (XServerDisplay, DefaultRootWindow (XServerDisplay), 0, 0, 1, 1, 0, 0, 0);
//
//     // wait for propertyChanges here.
//     XSelectInput (XServerDisplay, commandWindow, PropertyChangeMask);
//
//     XChangeProperty (XServerDisplay, commandWindow,
//                 rp_command, XA_STRING, 8, PropModeReplace,
//                 (unsigned char *)command.c_str(), command.size() + 2);
//
//     XChangeProperty (XServerDisplay, DefaultRootWindow (XServerDisplay),
//                 rp_command_request, XA_WINDOW, 8, PropModeAppend,
//                 (unsigned char *)&commandWindow, sizeof (Window));
//
//     bool done = false;
//
//     long timeSpent;
//     struct timeval startTime, currentTime;
//
//     gettimeofday(&startTime, NULL);
//     while ( ! done )
//     {
//         XEvent ev;
//
//         gettimeofday(&currentTime, NULL);
//
//         timeSpent = (currentTime.tv_sec - startTime.tv_sec) * 1000000 + (currentTime.tv_usec - startTime.tv_usec);
//         if ( XCheckMaskEvent (XServerDisplay, PropertyChangeMask, &ev) )
//         {
//             if (ev.xproperty.atom == rp_command_result && ev.xproperty.state == PropertyNewValue)
//             {
//                 g_pPlutoLogger->Write(LV_STATUS, "Reading command output");
//                 readCommandResult(XServerDisplay, ev.xproperty.window);
//                 done = 1;
//             }
//         }
//         else if ( timeSpent < 800000 ) // wait for at most 1 sec for the reply
//         {
//             usleep(25000); // read the result 20 pe second
//         }
//         else // timeout reading command response
//         {
//             g_pPlutoLogger->Write(LV_WARNING, "The ratpoison window manager didn't reply to the command. It is either crashed or not started at all");
//             done = 1;
//         }
//     }
//
//     // cleanups
//     XDestroyWindow (XServerDisplay, commandWindow);
// }

bool OrbiterLinux::RenderDesktop(DesignObj_Orbiter *pObj, PlutoRectangle rectTotal)
{
    g_pPlutoLogger->Write(LV_WARNING, "Need to resize in here [%d, %d, %dx%d]!",
            rectTotal.Top(), rectTotal.Left(),
            rectTotal.Bottom(), rectTotal.Right());

    resizeMoveDesktop(rectTotal.Left(), rectTotal.Top(),
            rectTotal.Right() - rectTotal.Left(),
            rectTotal.Bottom() - rectTotal.Top());

    desktopInScreen = true;
    return true;
}

// public interface implementations below
bool OrbiterLinux::resizeMoveDesktop(int x, int y, int width, int height)
{
    g_pPlutoLogger->Write(LV_STATUS, "Resizing desktop to (%d, %d) and dimensions [%dx%d]", x, y, width, height);

    stringstream commandLine;

    commandLine << ":set padding " << x << " " << y << " "
                << m_nDesktopWidth - x - width << " " << m_nDesktopHeight - y - height;

    commandRatPoison(commandLine.str());

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

    closeDisplay();
}*/

/*
bool OrbiterLinux::forkAndWait(char *const args[], int waitTime)
{
    switch ( fork() )
    {
        case -1:
            g_pPlutoLogger->Write(LV_WARNING, "There was an error trying to fork.");
            break;
        case 0:
        {
            g_pPlutoLogger->Write(LV_STATUS, "In Child process");
            stringstream commandLine;

            int arg = 0;
            while ( args[arg] != 0 )
                commandLine << " " << args[arg++] << "";

            g_pPlutoLogger->Write(LV_STATUS, "Spawning process: %s", commandLine.str().c_str());

            int result;
            if ( (result = execv(args[0], args)) == -1 )
            {

                g_pPlutoLogger->Write(LV_WARNING, "Exec failed %d", errno);
                perror("Execve error: ");
                execl("/bin/false", NULL); // Cause the image to owerwritten so that an exit will not close the XServerDisplay
            }
        }
//             break; // it doesn't reach here anyway.
        default:
            // i'm in the parent here
            g_pPlutoLogger->Write(LV_STATUS, "Fork was succesfull. Waiting %d", waitTime);
            usleep(waitTime * 1000 * 1000);
    }

    return true;
}
*/
