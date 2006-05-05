#ifndef __VolumeMouseHandler_H__
#define __VolumeMouseHandler_H__
//-----------------------------------------------------------------------------------------------------
#include <list>
#include <string>
#include <map>
using namespace std;

#include "DesignObj_Orbiter.h"
#include "Orbiter.h"
#include "PlutoUtils/ProcessUtils.h"
#include "MouseBehavior.h"

//struct SDL_Surface;

using namespace DCE;

namespace DCE
{
	//-----------------------------------------------------------------------------------------------------
	/**
	* @brief Handles special mouse behavior for lighting control
	*/
	//-----------------------------------------------------------------------------------------------------
	class VolumeMouseHandler : public MouseHandler
	{
	public:
		int m_iLastGoodPosition;

		VolumeMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior);
        ~VolumeMouseHandler();

        virtual EMouseHandler TypeOfMouseHandler() { return mh_Volume; }

		void Start();
		void Stop();

		bool ButtonDown(int PK_Button);
		bool ButtonUp(int PK_Button);
		void Move(int X,int Y,int PK_Direction);
		bool SlowDrift(int &X,int &Y);  // We're about to call a move after the user has been slowly drifting.  The handler can alter the position, and/or return true to ignore the move

		void DrawSquare(int Notch, PlutoColor plutoColor);

    //protected:
    //    SDL_Surface *m_pPrevSurface;
	};

}
//-----------------------------------------------------------------------------------------------------
#endif
