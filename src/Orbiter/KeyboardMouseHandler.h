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
		char m_cDirection;
		int m_dwPK_Direction_ScrollGrid;
		DesignObj_DataGrid *m_pObj_ScrollingGrid;

	public:
		KeyboardMouseHandler(DesignObj_Orbiter *pObj,MouseBehavior *pMouseBehavior) : MouseHandler(pObj,pMouseBehavior) { m_bLockAxis=true; m_dwPK_Direction_ScrollGrid=0; m_pObj_ScrollingGrid=NULL; }

		void Start();
		void Stop();

		bool ButtonDown(int PK_Button);
		bool ButtonUp(int PK_Button);
		void Move(int X,int Y);
		void MoveExternalApp(int X,int Y);

		void DoIteration();
		void IterateObject();
		void IterateExternalApp();

		void SelectFirstObject();
		int GetDirectionAwayFromHighlight(int X,int Y);
		void PositionMouseAtObjectEdge(int PK_Direction);
		PlutoRectangle GetHighlighedObjectCoordinates();
		bool MovedPastTopBottomOfDataGrid(DesignObj_DataGrid *pObj,int PK_Direction,int Y);
		void ScrollGrid(int dwPK_Direction,int X,int Y);
	};
}
//-----------------------------------------------------------------------------------------------------
#endif
