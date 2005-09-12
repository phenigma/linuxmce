#include "X3DButton.h"

X3DButton::X3DButton()
{
    m_sText = "";
}


X3DButton::~X3DButton()
{
}


bool X3DButton::DrawWindow()
{
    bool bRet = X3DWindow::DrawWindow();
    if (bRet) {
        Font font = XLoadFont(m_pDisplay, "-*-*-*-R-Normal--*-140-100-100-*-*");
        
        unsigned long mask = GCForeground | GCBackground | GCLineWidth | GCFont;
        XGCValues values;
        values.foreground = 0x000000;
        values.background = 0x000000;
        values.line_width = 1;
        values.font = font;
        
        GC gcText = XCreateGC(m_pDisplay, m_wndThis, mask, &values);
        
        int dir, ascent, descent;
        XCharStruct charStr;
        XQueryTextExtents (m_pDisplay, font, m_sText.c_str(), m_sText.length(), 
                        &dir, &ascent, &descent, &charStr);
                        
        int xPos = (m_nWidth - charStr.width) / 2;
        int yPos = (m_nHeight + ascent - descent) / 2;
        
        XDrawString(m_pDisplay, m_wndThis, gcText, xPos, yPos, m_sText.c_str(), m_sText.length());
        
        XUnloadFont(m_pDisplay, font);
        XFreeGC(m_pDisplay, gcText);
    }
    
    return bRet;
}

int X3DButton::CreateWindow(Display *pDisplay, int screen, Window wndParent, int x, int y, int cx, int cy, std::string sText)
{
    X3DWindow::CreateWindow(pDisplay, screen, wndParent, x, y, cx, cy);
    
    m_sText = sText;
    
    return 0;
}
