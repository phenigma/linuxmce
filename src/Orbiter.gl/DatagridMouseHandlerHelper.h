#ifndef __DatagridMouseHandlerHelper_H__
#define __DatagridMouseHandlerHelper_H__
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
	class DatagridMouseHandlerHelper
	{
		friend class KeyboardMouseHandler;

		class MouseHandler *m_pMouseHandler;
		class MouseBehavior *m_pMouseBehavior;
		int m_dwPK_Direction_ScrollGrid;
		DesignObj_DataGrid *m_pObj_ScrollingGrid;
		DesignObj_Orbiter *m_pObj_MediaBrowser_Alpha;
		DesignObj_Orbiter *m_pObj_MediaBrowser_Down,*m_pObj_MediaBrowser_Up;

	public:
		DatagridMouseHandlerHelper(MouseHandler *pMouseHandler);
		void Start(DesignObj_DataGrid *pObj_ScrollingGrid);
		bool StayInGrid(int PK_Direction,int X,int Y);

		bool MovedPastTopBottomOfDataGrid(DesignObj_DataGrid *pObj,int PK_Direction,int Y);
		void ScrollGrid(int dwPK_Direction,int X,int Y);
	};
}
//-----------------------------------------------------------------------------------------------------
#endif
