/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef __ORBITERLINUX_H__
#define __ORBITERLINUX_H__

#include "../SDL/StartOrbiterSDL.h"
#include "../Orbiter.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>
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
	class OrbiterRenderer_Linux;
};

class OrbiterLinux : public Orbiter
{
private:
	friend class DCE::OrbiterRenderer_Linux;

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
	
	bool m_bMaskApplied;

	virtual bool RenderDesktop( class DesignObj_Orbiter *pObj, PlutoRectangle rectTotal, PlutoPoint point = PlutoPoint(0, 0) );
    virtual void ActivateExternalWindowAsync(void *);

    bool m_bOrbiterReady; // ready to process events
    bool m_bIsExclusiveMode; // it's alone on the desktop
	
	bool MaskApplied();
	void ResetAppliedMask();
	void ApplyMask(PlutoRectangle rectTotal, PlutoPoint point);

	void ConfirmPcDesktop();

    // grabbed once for info only
    // do not use this
    Display *m_pDisplay_SDL;

public:
	OrbiterLinux(int DeviceID,int PK_DeviceTemplate,
                 string ServerAddress, string sLocalDirectory,
                 bool bLocalMode, int nImageWidth, int nImageHeight,
                 bool bFullScreen);

    virtual ~OrbiterLinux();

    WinListManager *m_pWinListManager;
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

	virtual Display * GetDisplay(); // use this
    virtual Window GetMainWindow(); // use this
    virtual Display * GetDisplay_MainWindow(); // do not use

	virtual void CMD_Activate_PC_Desktop(bool bTrueFalse,string &sCMD_Result,Message *pMessage);

	virtual void CMD_Activate_Window(string sWindowName,string &sCMD_Result,Message *pMessage);
	virtual void CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,Message *pMessage);
	virtual void CMD_Set_Mouse_Position_Relative(int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);
	virtual void CMD_Simulate_Mouse_Click_At_Present_Pos(string sType,string &sCMD_Result,Message *pMessage);

    virtual void CMD_Surrender_to_OS(string sOnOff,bool bFully_release_keyboard,bool bAlways,string &sCMD_Result,Message *pMessage);

	virtual class ScreenHandler *CreateScreenHandler();

    void GrabPointer(bool bEnable);
    void GrabKeyboard(bool bEnable);

	virtual bool PreprocessEvent(Orbiter::Event &event);
	int TranslateXKeyCodeToPlutoButton(int Keycode,int Type);

	virtual void X_LockDisplay();
    virtual void X_UnlockDisplay();

	XRecordExtensionHandler * GetXRecordExtensionHandler()
    {
        return m_pRecordHandler;
    }

	void StopActivateExternalWindowTask();
};

#endif // __ORBITERLINUX_H__
