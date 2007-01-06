#ifndef __EPGMouseHandler_H__
#define __EPGMouseHandler_H__
//-----------------------------------------------------------------------------------------------------
#include <list>
#include <string>
#include <map>
using namespace std;

#include "DesignObj_Orbiter.h"
#include "Orbiter.h"
#include "PlutoUtils/ProcessUtils.h"
#include "MouseBehavior.h"
#include "DatagridMouseHandlerHelper.h"

using namespace DCE;

#define MAX_SPEEDS		17

namespace DCE
{
	//-----------------------------------------------------------------------------------------------------
	/**
	* @brief Handles special mouse behavior for lighting control
	*/
	//-----------------------------------------------------------------------------------------------------
	class EPGMouseHandler : public MouseHandler
	{
		DatagridMouseHandlerHelper *m_pDatagridMouseHandlerHelper;
		DesignObj_DataGrid *m_pDesignObj_DataGrid_Active;
		DesignObj_DataGrid *m_pDesignObj_UpcomingChannels;
		DesignObjText *m_pDesignObjText_Synopsis;
		int m_iRow_Last;

	public:
		EPGMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior);
		virtual EMouseHandler TypeOfMouseHandler() { return mh_Media; }
		virtual ~EPGMouseHandler();

		void Start();
		void Stop();

		bool ButtonDown(int PK_Button);
		bool ButtonUp(int PK_Button);
		void Move(int X,int Y,int PK_Direction);
	};

}
//-----------------------------------------------------------------------------------------------------
#endif
