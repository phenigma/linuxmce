#ifndef __HorizMenuMouseHandler_H__
#define __HorizMenuMouseHandler_H__
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

#define MAX_SPEEDS		17

namespace DCE
{
	//-----------------------------------------------------------------------------------------------------
	/**
	* @brief Handles special mouse behavior for lighting control
	*/
	//-----------------------------------------------------------------------------------------------------
	class HorizMenuMouseHandler : public MouseHandler
	{
		DesignObj_Orbiter *m_pObj_ActiveMenuPad,*m_pObj_ActiveSubMenu;

	public:
		HorizMenuMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior);
		virtual EMouseHandler TypeOfMouseHandler() { return mh_HorizMenu; }

		void Start();
		void Stop();

		bool ButtonDown(int PK_Button);
		bool ButtonUp(int PK_Button);
		void Move(int X,int Y,int PK_Direction);

		void ShowMainMenuPopup(DesignObj_Orbiter *pObj_MenuPad);
	};

}
//-----------------------------------------------------------------------------------------------------
#endif
