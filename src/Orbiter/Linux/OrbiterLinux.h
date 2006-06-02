#ifndef __ORBITERLINUX_H__
#define __ORBITERLINUX_H__

#include "../SDL/StartOrbiterSDL.h"
#include "../Orbiter.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>
//#include "utilities/linux/RatpoisonHandler.h"
#include "XRecordExtensionHandler.h"

#include "win_list_manager.h"
#include "utilities/linux/wrapper/wrapper_x11.h"

// should be deleted
#include "XProgressWnd.h"
#include "XPromptUser.h"

class wxDialog_WaitGrid;
class wxDialog_WaitList;
class wxDialog_WaitUser;

#include <string>
using namespace std;

namespace DCE
{
	class OrbiterRenderer_SDL_Linux;
};

class OrbiterLinux : public Orbiter
{
private:
	friend class OrbiterRenderer_SDL_Linux;

    string m_strWindowName;
    string m_strDisplayName;

    bool findWindowByName();

    Window searchChildsWindowByName(Window current);
    string getWindowName(Window name);

    bool X11_Init();
    bool X11_Exit();

    XRecordExtensionHandler *m_pRecordHandler;

    int m_nDesktopWidth;
    int m_nDesktopHeight;

    int m_nProgressWidth;
    int m_nProgressHeight;

    XProgressWnd *m_pProgressWnd;

    wxDialog_WaitGrid *m_pWaitGrid;
    bool m_bButtonPressed_WaitGrid;
    wxDialog_WaitList *m_pWaitList;
    bool m_bButtonPressed_WaitList;
    wxDialog_WaitUser *m_pWaitUser;

protected:
    // virtual bool forkAndWait(char * const args[], int millis);
	virtual bool RenderDesktop( class DesignObj_Orbiter *pObj, PlutoRectangle rectTotal, PlutoPoint point = PlutoPoint(0, 0) );
    virtual void ActivateExternalWindowAsync(void *);

    virtual void setInputFocusToMe(void *);

    bool m_bOrbiterReady; // ready to process events
    bool m_bIsExclusiveMode; // it's alone on the desktop

	//keyboardState m_keyboardState;

    // grabbed once for info only
    // do not use this
    Display *m_pDisplay_SDL;

public:
	OrbiterLinux(int DeviceID,int PK_DeviceTemplate,
                 string ServerAddress, string sLocalDirectory,
                 bool bLocalMode, int nImageWidth, int nImageHeight,
                 bool bFullScreen,
                 bool bUseOpenGL);

    virtual ~OrbiterLinux();

    WinListManager *m_pWinListManager;
    WMControllerImpl *m_pWMController;
    X11wrapper *m_pX11;

	/**
	 * @brief Sets up the class right after the constructor is called
	 */
	//virtual bool GetConfig();

	void HideOtherWindows();

    void setWindowName(string strDesktopWindowName);
    void setDisplayName(string strDisplayName);

    virtual void reinitGraphics();
    virtual void Initialize(GraphicType Type, int iPK_Room=0, int iPK_EntertainArea=0);

    // called by the destructor
    // can be called from outside
    virtual void Destroy();

    virtual bool PreprocessEvent( Orbiter::Event &event );

    virtual Display * GetDisplay(); // use this
    virtual Window GetMainWindow(); // use this
    virtual Display * GetDisplay_MainWindow(); // do not use

	// overridden to handle turning on and off the mouse pointer
	virtual void CMD_Show_Mouse_Pointer(string sOnOff,string &sCMD_Result,Message *pMessage);
	virtual void CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage);
	virtual void CMD_Activate_Window(string sWindowName,string &sCMD_Result,Message *pMessage);
	virtual void CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,Message *pMessage);
	virtual void CMD_Set_Mouse_Position_Relative(int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);
	virtual void CMD_Simulate_Mouse_Click_At_Present_Pos(string sType,string &sCMD_Result,Message *pMessage);

    virtual void CMD_Surrender_to_OS(string sOnOff, bool bFully_release_keyboard, string &sCMD_Result, Message *pMessage);

	virtual class ScreenHandler *CreateScreenHandler();

    void GrabPointer(bool bEnable);
    void GrabKeyboard(bool bEnable);

    virtual void X_LockDisplay();
    virtual void X_UnlockDisplay();

	XRecordExtensionHandler * GetXRecordExtensionHandler()
    {
        return m_pRecordHandler;
    }
};

#endif // __ORBITERLINUX_H__
