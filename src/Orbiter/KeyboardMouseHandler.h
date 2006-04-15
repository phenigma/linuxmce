#ifndef __KeyboardMouseHandler_H__
#define __KeyboardMouseHandler_H__
//-----------------------------------------------------------------------------------------------------
#include <list>
#include <string>
#include <map>
using namespace std;

#include "DesignObj_Orbiter.h"
#include "Orbiter.h"
#include "PlutoUtils/ProcessUtils.h"
#include "MouseBehavior.h"
using namespace DCE;

namespace DCE
{
	//-----------------------------------------------------------------------------------------------------
	/**
	* @brief Handles special mouse behavior for lighting control
	*/
	//-----------------------------------------------------------------------------------------------------
	class KeyboardMouseHandler : public MouseHandler
	{
		bool m_bHorizontal;
	public:
		KeyboardMouseHandler(DesignObj_Orbiter *pObj,MouseBehavior *pMouseBehavior) : MouseHandler(pObj,pMouseBehavior) {}

		void Start();
		void Stop();

		bool ButtonDown(int PK_Button);
		bool ButtonUp(int PK_Button);
		void Move(int X,int Y);
	};

}
//-----------------------------------------------------------------------------------------------------
#endif
