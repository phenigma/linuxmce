#ifndef X3DBUTTON_H
#define X3DBUTTON_H

#include <string>

#include "X3DWindow.h"

namespace DCE {
/**
@author Ioan Marius Curelariu
*/
class X3DButton : public X3DWindow
{
protected:
    std::string m_sText;
    Font m_font;
    
public:
    X3DButton();
    ~X3DButton();
    
    void SetFont(Font font);

    /*virtual */bool DrawWindow();
    /*virtual */int CreateWindow(Display *pDisplay, int screen, Window wndParent, int x, int y, int cx, int cy, std::string sText);
};

}	// namespace DCE

#endif
