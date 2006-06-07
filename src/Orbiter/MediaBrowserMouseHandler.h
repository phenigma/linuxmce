#ifndef __MediaBrowserMouseHandler_H__
#define __MediaBrowserMouseHandler_H__
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
	class MediaBrowserMouseHandler : public MouseHandler
	{
		friend class ScreenHandler;
		int m_RelativeVirtualY;

		DesignObj_DataGrid *m_pObj_ListGrid,*m_pObj_PicGrid;
		DesignObj_Orbiter *m_pObj_CoverArtPopup;

	public:
		MediaBrowserMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior);
		virtual EMouseHandler TypeOfMouseHandler() { return mh_MediaBrowser; }

		void Start();
		void Stop();

		bool ButtonDown(int PK_Button);
		bool ButtonUp(int PK_Button);
		void RelativeMove(int DeltaX, int DeltaY);
		void Move(int X,int Y,int PK_Direction);
		void ShowCoverArtPopup();
	};

}
//-----------------------------------------------------------------------------------------------------
#endif
