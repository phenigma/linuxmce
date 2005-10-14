#include "XProgressWnd.h"
#include "DCE/Logger.h"

#include <X11/Xutil.h>

using namespace DCE;

XProgressWnd::XProgressWnd()
 : m_pButton(NULL)
 , m_bCanceled(false)
 , m_bDone(false)
 , m_bDestroy(false)
{
    m_nTextX = 0;
    m_nTextY = 0;
    m_nBarX = 0;
    m_nBarY = 0;
    m_nBarWidth = 0;
    m_nBarHeight = 0;

    m_sText = "";
    m_nProgress = 0;

	m_pDisplay = NULL;
}


XProgressWnd::~XProgressWnd()
{
    if (m_pButton) delete m_pButton;
    if (m_pDisplay) XCloseDisplay(m_pDisplay);
}


bool XProgressWnd::ShowWindow(bool bShow/* = true*/)
{
    if (!m_wndThis) return false;
    
    if (m_pButton)
        m_pButton->ShowWindow(bShow);
        
    X3DWindow::ShowWindow(bShow);
    return true;
}

bool XProgressWnd::DrawWindow()
{
    //XLockDisplay(m_pDisplay);

    bool bRet = X3DWindow::DrawWindow();
    
    if (bRet && m_pButton) {
        bRet = m_pButton->DrawWindow();
    }
    
    if (bRet) {
        unsigned long mask = GCForeground | GCBackground | GCLineWidth;
        XGCValues values;
        values.line_width = 1;
        values.foreground = 0x000000;
        values.background = 0x000000;
        GC gcBackground = XCreateGC(
                    m_pDisplay, 
                    m_wndThis,
                    mask,
                    &values);
        XFillRectangle(m_pDisplay, m_wndThis, gcBackground, 2, 2, m_nWidth-4, m_nHeight-4);
        
        values.foreground = 0xFFFFFF;
        values.background = 0xFFFFFF;
        
        GC gcBorder = XCreateGC(
                    m_pDisplay, 
                    m_wndThis,
                    mask,
                    &values);
                    
        values.foreground = 0x0000FF;
        values.background = 0x0000FF;
        GC gcBar = XCreateGC(
                    m_pDisplay, 
                    m_wndThis,
                    mask,
                    &values);

        Font font = XLoadFont(m_pDisplay, "-*-*-*-R-Normal--*-140-75-75-*-*");
        mask = GCForeground | GCBackground | GCLineWidth | GCFont;
        values.foreground = 0xFFFFFF;
        values.background = 0xFFFFFF;
        values.font = font;
        GC gcText = XCreateGC(
                    m_pDisplay, 
                    m_wndThis,
                    mask,
                    &values);
                    
        int nBarWidth = (m_nBarWidth-3) * m_nProgress / 100;
        
        XDrawString(m_pDisplay, m_wndThis, gcText, m_nTextX, m_nTextY, m_sText.c_str(), m_sText.length());
        XDrawRectangle(m_pDisplay, m_wndThis, gcBorder, m_nBarX, m_nBarY, m_nBarWidth, m_nBarHeight);
        XFillRectangle(m_pDisplay, m_wndThis, gcBar, m_nBarX+2, m_nBarY+2, nBarWidth, m_nBarHeight-2);
        
        XUnloadFont(m_pDisplay, font);
        XFreeGC(m_pDisplay, gcText);
        XFreeGC(m_pDisplay, gcBar);
        XFreeGC(m_pDisplay, gcBorder);  
        XFreeGC(m_pDisplay, gcBackground);
    }
    
    //XUnlockDisplay(m_pDisplay);
    return bRet;
}

bool XProgressWnd::EventLoop()
{
    XEvent event;
    while (not m_bDone) {
        if (XCheckIfEvent(m_pDisplay, &event, &CheckIfEvent, (char*)this)) {
        	//process event ...
		if (g_pPlutoLogger) g_pPlutoLogger->Write(LV_STATUS, "Event received");

        	switch (event.type) {
        	case Expose:
		    if (g_pPlutoLogger) g_pPlutoLogger->Write(LV_STATUS, "Expose event received");
        	    DrawWindow();
        	    break;
        	case ButtonRelease:
		    if (g_pPlutoLogger) g_pPlutoLogger->Write(LV_STATUS, "ButtonRelease event received");
        	    XButtonEvent *pButtonEvent = (XButtonEvent *)&event;
        	    if (m_pButton && m_pButton->HitTest(pButtonEvent->x, pButtonEvent->y)) {
                	m_bDone = true;
			m_bCanceled = true;
        	    }
        	    break;
        	}
	} else {
		usleep(250000);
	}
    }
    
    return true;
}

Bool XProgressWnd::CheckIfEvent(Display *pDisplay, XEvent *pEvent, XPointer arg)
{
    XProgressWnd *pThis = (XProgressWnd *)arg;
    XAnyEvent *pAnyEvent = (XAnyEvent *)pEvent;
    
    bool bFound = false;
    if (pAnyEvent->window == pThis->m_wndThis) bFound = true;
    std::list<Window>::iterator it = pThis->m_childs.begin();
    while (!bFound && it != pThis->m_childs.end()) {
        if (*it == pAnyEvent->window) {
            bFound = true;
            break;
        }
        it++;
    }
    
    return true;
}

bool XProgressWnd::UpdateProgress(std::string sText, int nProgress)
{
   if (nProgress == -1) {
      m_bDone = true;
   }
   
    m_sText = sText;
    m_nProgress = nProgress;
   
    return true;
}

static void *MyThreadFunc(void *pWindow)
{
    if (g_pPlutoLogger) g_pPlutoLogger->Write(LV_STATUS, "Thread func start ...");
    XProgressWnd *pWnd = (XProgressWnd *)pWindow;
    
    pWnd->EventLoop();
    
    if (g_pPlutoLogger) g_pPlutoLogger->Write(LV_STATUS, "Thread func ending ...");
    pWnd->DestroyWindow();

	Display * pDisplay = pWnd->GetDisplay();
	if (pDisplay)
	    XSync(pDisplay, false);
    if (g_pPlutoLogger) g_pPlutoLogger->Write(LV_STATUS, "Thread func ended.");
    
    if (pWnd->Destroy())
    {
    	delete pWnd;
    }
    
    return NULL;
}

pthread_t XProgressWnd::Run()
{
    pthread_t threadID;
    if (g_pPlutoLogger) g_pPlutoLogger->Write(LV_STATUS, "Starting the thread ...");
    m_bCanceled = false;
    m_bDone = false;

	// don't put this in the thread (again) as that creates a race condition; leave it here :)
    Display *pDisplay = XOpenDisplay(NULL); // needs XCloseDisplay?
    int nScreenNo = DefaultScreen(pDisplay);
    int nDesktopX, nDesktopY;
    nDesktopX = DisplayWidth(pDisplay, nScreenNo);
    nDesktopY = DisplayHeight(pDisplay, nScreenNo);
    int nWidth = 250, nHeight = 140;
    int xPos = (nDesktopX - nWidth) / 2;
    int yPos = (nDesktopY - nHeight) / 2;
    CreateWindow(pDisplay, nScreenNo, DefaultRootWindow(pDisplay), 0, 0, nDesktopX, nDesktopY);
    ShowWindow();
    DrawWindow();
        
    int iResult = pthread_create( &threadID, NULL, MyThreadFunc, (void *)this );
    if ( iResult != 0 )
    {
	if (g_pPlutoLogger) g_pPlutoLogger->Write(LV_CRITICAL, "Fatal error: Cannot start thread!");
    }
    else {
	if (g_pPlutoLogger) g_pPlutoLogger->Write(LV_STATUS, "Thread started ...");
	pthread_detach(threadID);
    }
    
    m_thisThread = threadID;
    return threadID;
}

void XProgressWnd::Terminate()
{
	m_bDestroy = true;
	m_bDone = true;
}

int XProgressWnd::CreateWindow(Display *pDisplay, int screen, Window wndParent, int x, int y, int cx, int cy)
{
    X3DWindow::CreateWindow(pDisplay, screen, wndParent, x, y, cx, cy);
    
    m_nTextX = 20;
    m_nTextY = 40;
    m_nBarX = 20;
    m_nBarY = 50;
    m_nBarWidth = m_nWidth - 40;
    m_nBarHeight = 24;
    
    if (g_pPlutoLogger) g_pPlutoLogger->Write(LV_STATUS, "Constructing ProgressWindow");
    
    m_wndName = "Progress";
    XClassHint *pClassHint = XAllocClassHint();
    pClassHint->res_name = (char *)m_wndName.c_str();
    pClassHint->res_class = (char *)m_wndName.c_str();
    XSetClassHint(pDisplay, m_wndThis, pClassHint);
    XFree(pClassHint);
    return 0;
}

Display * XProgressWnd::GetDisplay()
{
	return m_pDisplay;
}
