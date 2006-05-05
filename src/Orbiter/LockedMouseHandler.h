#ifndef __LockedMouseHandler_H__
#define __LockedMouseHandler_H__
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
	class LockedMouseHandler : public MouseHandler
	{
		DesignObj_Orbiter *m_pObj_Highlighted;
		bool m_bFirstTime; // Will be true the first time movement is locked onto this handler, false for subsequent times
		int m_PK_Direction_Last;

	public:
		bool m_bActivatedObject; // Set to true whenever an object was activated

		LockedMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior);
		virtual EMouseHandler TypeOfMouseHandler() { return mh_Locked; }

		void Start();
		void Stop();

		bool ButtonDown(int PK_Button);
		bool ButtonUp(int PK_Button);
		void Move(int X,int Y,int PK_Direction);
		void Notch(int PK_Direction,int iRepeat); // The user moved a 'notch' in the given direction
		bool SlowDrift(int &X,int &Y); // We're about to call a move after the user has been slowly drifting.  The handler can alter the position, and/or return true to ignore the move
		bool MovedOutside(int PK_Direction); // Override this and return true

		void ActivatedMainMenuPad();
		void ActivatedSubMenu();
	};

}
//-----------------------------------------------------------------------------------------------------
#endif
