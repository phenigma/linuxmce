#ifndef X3DWINDOW_H
#define X3DWINDOW_H

#include <X11/X.h>
#include <X11/Xlib.h>

#include <list>

/**
@author Ioan Marius Curelariu
*/
class X3DWindow{
protected:
    Display *m_pDisplay;
    int m_nScreen;
    int m_nPosX, m_nPosY, m_nWidth, m_nHeight;
    int m_nBorderWidth;
    Window m_wndParent, m_wndThis;
    bool m_bVisible;
    
    std::list<Window> m_childs;
    
public:
    X3DWindow();
    virtual ~X3DWindow();
    
    virtual bool ShowWindow(bool bShow = true);
    virtual bool ChangeAttributes(unsigned long mask, XSetWindowAttributes attrs);
    virtual bool DrawWindow();
    
    Window GetWindow() { return m_wndThis; }
    bool HitTest(int x, int y);
    
public:
    virtual int CreateWindow(Display *pDisplay, int screen, Window wndParent, int x, int y, int cx, int cy);
    virtual int DestroyWindow();
};

#endif
