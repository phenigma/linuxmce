#ifndef __MouseBehavior_Win32_H__
#define __MouseBehavior_Win32_H__
//-----------------------------------------------------------------------------------------------------
#include <list>
#include <string>
#include <map>
using namespace std;

#include "MouseBehavior.h"
using namespace DCE;

namespace DCE
{
	//-----------------------------------------------------------------------------------------------------
	/**
	* @brief Handles special mouse behavior in a Win32 environment
	*/
	//-----------------------------------------------------------------------------------------------------
	class MouseBehavior_Win32 : public MouseBehavior
	{
	public:
		MouseBehavior_Win32(Orbiter *pOrbiter);
		virtual void SetMousePosition(int X,int Y);
		virtual void GetMousePosition(PlutoPoint *p) { POINT pt; GetCursorPos(&pt); p->X=pt.x; p->Y=pt.y; }
		virtual void ShowMouse(bool bShow);
		bool ConstrainMouse(const PlutoRectangle &rect);
		void SetMouseCursorStyle(MouseCursorStyle mouseCursorStyle);
	};
}
//-----------------------------------------------------------------------------------------------------
#endif
