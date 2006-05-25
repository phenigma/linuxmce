#ifndef __ORBITERRENDERERSDLLINUX_H__
#define __ORBITERRENDERERSDLLINUX_H__

#include "../SDL/StartOrbiterSDL.h"
#include "../SDL/OrbiterSDL.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include "XRecordExtensionHandler.h"

#include "win_list_manager.h"

// should be deleted
#include "XProgressWnd.h"
#include "XPromptUser.h"

class wxDialog_WaitGrid;
class wxDialog_WaitList;
class wxDialog_WaitUser;

#include <string>
using namespace std;

class OrbiterRendererSDLLinux : public OrbiterSDL/*, public RatpoisonHandler<OrbiterLinux>*/
{
	friend class OrbiterLinux;
private:
	XRecordExtensionHandler *m_pRecordHandler;

    string m_strWindowName;
    string m_strDisplayName;

    Display *XServerDisplay;

    bool findWindowByName();

    Window searchChildsWindowByName(Window current);
    string getWindowName(Window name);

    bool openDisplay();
    bool closeDisplay();

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

    WinListManager m_WinListManager;

    bool m_bOrbiterReady; // ready to process events
    bool m_bIsExclusiveMode; // it's alone on the desktop

	keyboardState m_keyboardState;
public:
	OrbiterRendererSDLLinux();

    virtual ~OrbiterRendererSDLLinux();

	/**
	 * @brief Sets up the class right after the constructor is called
	 */
	//virtual bool GetConfig();

	void HideOtherWindows();
		
	void setWindowName(string strDesktopWindowName);
    void setDisplayName(string strDisplayName);

    // overriden to handle the desktop window hiding
    virtual void RenderScreen( bool bRenderGraphicsOnly );
    virtual void reinitGraphics();
    virtual void Initialize(GraphicType Type, int iPK_Room=0, int iPK_EntertainArea=0);

	virtual bool PreprocessEvent( Orbiter::Event &event );
    virtual Display *getDisplay();
	virtual Window getWindow();

    /*virtual */bool DisplayProgress(string sMessage, int nProgress);
    /*virtual */bool DisplayProgress(string sMessage, const map<string, bool> &mapChildDevices, int nProgress);
	/*virtual */int PromptUser(string sPrompt,int iTimeoutSeconds=10,map<int,string> *p_mapPrompts=NULL);

    void GrabPointer(bool bEnable);
    void GrabKeyboard(bool bEnable);

    virtual void X_LockDisplay();
    virtual void X_UnlockDisplay();

	XRecordExtensionHandler * GetXRecordExtensionHandler()
        {
            return m_pRecordHandler;
        }


};

#endif
