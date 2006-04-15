#ifndef __SpeedMouseHandler_H__
#define __SpeedMouseHandler_H__
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
	class SpeedMouseHandler : public MouseHandler
	{
		bool m_bHasTimeline;
		const static int m_iSpeeds[MAX_SPEEDS];
		int m_CurrentMedia_Start,m_CurrentMedia_Stop,m_CurrentMedia_Pos;  // The start and stop values in seconds of the media, and where we are now

	public:
		SpeedMouseHandler(DesignObj_Orbiter *pObj,MouseBehavior *pMouseBehavior);

		void Start();
		void Stop();

		bool ButtonDown(int PK_Button);
		bool ButtonUp(int PK_Button);
		void Move(int X,int Y);

		bool ParsePosition(string sMediaPosition);
	};

}
//-----------------------------------------------------------------------------------------------------
#endif
