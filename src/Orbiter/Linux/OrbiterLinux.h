#ifndef __CONTROLLERLINUXDESKTOP_H__
#define __CONTROLLERLINUXDESKTOP_H__

#include "../SDL/OrbiterSDL.h"
#include "utilities/linux/RatpoisonHandler.h"

#include <string>

// X11 related includes (used by the code to find the window on the server)
#include <X11/Xlib.h>
#include <X11/Xatom.h>

using namespace std;

class OrbiterLinux : public OrbiterSDL, public RatpoisonHandler<OrbiterLinux>
{
private:
    string m_strWindowName;
    string m_strDisplayName;

    int desktopInScreen;

    Display *XServerDisplay;

//     Atom rp_command, rp_command_request, rp_command_result;

    bool findWindowByName();

    Window searchChildsWindowByName(Window current);
    string getWindowName(Window name);

    bool openDisplay();
    bool closeDisplay();

//     void sendCommandToRatpoison(string command);
//     void readCommandResult(Display* display, Window commandWindow);

    int m_nDesktopWidth;
    int m_nDesktopHeight;

protected:
    // virtual bool forkAndWait(char * const args[], int millis);
    virtual bool RenderDesktop(class DesignObj_Orbiter *pObj, PlutoRectangle rectTotal);

    virtual bool resizeMoveDesktop(int x, int y, int width, int height);
    virtual bool setDesktopVisible(bool visible = true);
    virtual void setInputFocusToMe(void *);

public:
    OrbiterLinux(int DeviceID,
                 string ServerAddress, string sLocalDirectory,
                 bool bLocalMode, int nImageWidth, int nImageHeight);

    virtual ~OrbiterLinux();

    void setWindowName(string strDesktopWindowName);
    void setDisplayName(string strDisplayName);

    // overriden to handle the desktop window hiding
    virtual void RenderScreen();
    virtual void reinitGraphics();
    virtual void Initialize(GraphicType Type);
    virtual Display *getDisplay();

//     virtual void Initialize(GraphicType Type);
};

#endif // __CONTROLLERLINUXDESKTOP_H__
