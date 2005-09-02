#ifndef __CONTROLLERLINUXDESKTOP_H__
#define __CONTROLLERLINUXDESKTOP_H__


#include "../SDL/OrbiterSDL.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "utilities/linux/RatpoisonHandler.h"

#include "XRecordExtensionHandler.h"
#include <string>



using namespace std;

class OrbiterLinux : public OrbiterSDL, public RatpoisonHandler<OrbiterLinux>
{
private:
	XRecordExtensionHandler *m_pRecordHandler;

    string m_strWindowName;
    string m_strDisplayName;

    int desktopInScreen;

    Display *XServerDisplay;

    bool findWindowByName();

    Window searchChildsWindowByName(Window current);
    string getWindowName(Window name);

    bool openDisplay();
    bool closeDisplay();

    int m_nDesktopWidth;
    int m_nDesktopHeight;

protected:
    // virtual bool forkAndWait(char * const args[], int millis);
	virtual bool RenderDesktop( class DesignObj_Orbiter *pObj, PlutoRectangle rectTotal, PlutoPoint point = PlutoPoint(0, 0) );

    virtual bool resizeMoveDesktop(int x, int y, int width, int height);
    virtual bool setDesktopVisible(bool visible = true);
    virtual void setInputFocusToMe(void *);

public:
    OrbiterLinux(int DeviceID,int PK_DeviceTemplate,
                 string ServerAddress, string sLocalDirectory,
                 bool bLocalMode, int nImageWidth, int nImageHeight);

    virtual ~OrbiterLinux();

    void setWindowName(string strDesktopWindowName);
    void setDisplayName(string strDisplayName);

    // overriden to handle the desktop window hiding
    virtual void RenderScreen();
    virtual void reinitGraphics();
    virtual void Initialize(GraphicType Type, int iPK_Room=0, int iPK_EntertainArea=0);

	virtual bool PreprocessEvent( Orbiter::Event &event );
    virtual Display *getDisplay();
	virtual Window getWindow();

	// overridden to handle turning on and off the mouse pointer
	virtual void CMD_Show_Mouse_Pointer(string sOnOff,string &sCMD_Result,Message *pMessage);
	virtual void CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage);
	virtual void CMD_Activate_Window(string sName,string &sCMD_Result,Message *pMessage);
	virtual void CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,Message *pMessage);
	virtual void CMD_Set_Mouse_Position_Relative(int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);
	virtual void CMD_Simulate_Mouse_Click_At_Present_Pos(string sType,string &sCMD_Result,Message *pMessage);
};

#endif // __CONTROLLERLINUXDESKTOP_H__
