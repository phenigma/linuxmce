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
		bool m_bCapturingOffscreenMovement;
		int m_saveX, m_saveY;
		PlutoPoint m_Rel;

		DesignObj_DataGrid *m_pObj_ListGrid,*m_pObj_PicGrid;
		DesignObj_Orbiter *m_pObj_CoverArtPopup;

	public:
		MediaBrowserMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior);
		~MediaBrowserMouseHandler();
		virtual EMouseHandler TypeOfMouseHandler() { return mh_MediaBrowser; }

		void Start();
		void Stop();

		bool ButtonDown(int PK_Button);
		bool ButtonUp(int PK_Button);
		void RelativeMove(int DeltaX, int DeltaY);
		void ReleaseRelative();
		void Move(int X,int Y,int PK_Direction);
		void ShowCoverArtPopup();

        /// relative mouse pointer
        ///   hide when relative motion is wanted
        ///   show the standard pointer whdn relative motion is not wanted
    public:
        // speed == 0 : normal pointer
        // speed < 0 : up
        // speed > 0 : down
        bool RelativePointer_SetStatus(int nSpeedShape);
    protected:
        // holds both speed and shape
        int m_nRelativePointer_SpeedShape;
        // path to the pointer image
        // which will be displayed instead of the real pointer
		class PlutoGraphic * m_pRelativePointer_Image;

    };

}
//-----------------------------------------------------------------------------------------------------
#endif
