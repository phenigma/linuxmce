#ifndef __LightMouseHandler_H__
#define __LightMouseHandler_H__
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
	class LightMouseHandler : public MouseHandler
	{
	public:
		LightMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior) : MouseHandler(pObj,sOptions,pMouseBehavior) {}
		virtual EMouseHandler TypeOfMouseHandler() { return mh_Light; }

		void Start();
		void Stop();

		bool ButtonDown(int PK_Button);
		bool ButtonUp(int PK_Button);
		void Move(int X,int Y);

		void DrawSquare(int Notch,PlutoColor plutoColor);
	};

}
//-----------------------------------------------------------------------------------------------------
#endif
