#ifndef __MouseBehavior_Linux_H__
#define __MouseBehavior_Linux_H__
//-----------------------------------------------------------------------------------------------------
#include <list>
#include <string>
#include <map>
using namespace std;

#include "../MouseBehavior.h"
using namespace DCE;
#include "OrbiterLinux.h"

namespace DCE
{
    //-----------------------------------------------------------------------------------------------------
	/**
	* @brief Handles special mouse behavior in a Linux environment
	*/
	//-----------------------------------------------------------------------------------------------------
	class MouseBehavior_Linux : public MouseBehavior
	{
	public:
		MouseBehavior_Linux(Orbiter *pOrbiter);
		~MouseBehavior_Linux();
		virtual void SetMousePosition(int X,int Y);
		virtual void ShowMouse(bool bShow);
        virtual bool ConstrainMouse(const PlutoRectangle &rect);
		virtual void SetMouseCursorStyle(MouseCursorStyle mouseCursorStyle);
    protected:
        OrbiterLinux * ptrOrbiterLinux();

        // activate   : both width and height must be > 0
        // deactivate : both width and height == 0
        bool X11_Mouse_Constrain(Display *pDisplay, Window parent_window, int nPosX, int nPosY, unsigned int nWidth, unsigned int nHeight, std::string *pStringError=NULL);

        bool X11_Window_SetCursor_Font(Display *pDisplay, Window window, int nShape, std::string *pStringError=NULL);

        bool X11_Window_SetCursor_Image(Display *pDisplay, Window window, const std::string &sPath, const std::string &sPathMask, std::string *pStringError=NULL);

        bool m_bIsActive_Mouse_Constrain;
        Window m_window_Mouse_Constrain;
	};
}

std::string X11_ErrorText(Display * pDisplay, int error_code);

// XFreePixmap should be called
bool X11_Pixmap_ReadFile(Display *pDisplay, Window window, const std::string &sPath, Pixmap &pixmap_return, unsigned int &width_return, unsigned int &height_return, int &x_hot_return, int &y_hot_return, std::string *pStringError=NULL);

//-----------------------------------------------------------------------------------------------------

#endif
// __MouseBehavior_Linux_H__
