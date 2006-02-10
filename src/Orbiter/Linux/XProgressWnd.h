#ifndef XProgressWnd_H
#define XProgressWnd_H

#include "X3DWindow.h"
#include "X3DButton.h"

#include "PlutoUtils/MultiThreadIncludes.h"

#include <string>
#include <pthread.h>

namespace DCE {
/**
@author Ioan Marius Curelariu
*/
class XProgressWnd : public X3DWindow
{
protected:
    X3DButton *m_pButton;
    int m_nBarX, m_nBarY, m_nTextX, m_nTextY;
    int m_nBarWidth, m_nBarHeight;
    
    std::string m_sText;
    int m_nProgress;
    
    bool m_bCanceled;
    bool m_bDone;
    bool m_bDestroy;
		
    
public:
    pthread_t m_thisThread;
    
    std::string m_wndName;
	
public:
    XProgressWnd();
    ~XProgressWnd();

    /* virtual */bool DrawWindow();
    /* virtual */bool ShowWindow(bool bShow = true);
    /* virtual */int CreateWindow(Display *pDisplay, int screen, Window wndParent, int x, int y, int cx, int cy);
    bool UpdateProgress(std::string sText, int nProgress);
    bool IsCancelled(){ return m_bCanceled; };
    bool Destroy(){ return m_bDestroy; }
    
    virtual bool EventLoop();
    virtual pthread_t Run();
    virtual void Terminate();
    
    static Bool CheckIfEvent(Display *pDisplay, XEvent *pEvent, XPointer arg);

	Display * GetDisplay();
};

}	// namespace DCE

#endif
