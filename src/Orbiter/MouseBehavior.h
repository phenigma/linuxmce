#ifndef __MouseBehavior_H__
#define __MouseBehavior_H__
//-----------------------------------------------------------------------------------------------------
#include <list>
#include <string>
#include <map>
using namespace std;

#include "DesignObj_Orbiter.h"
#include "Orbiter.h"
#include "PlutoUtils/ProcessUtils.h"
using namespace DCE;

#define MAX_SPEEDS		17

namespace DCE
{
	class MouseSensitivity
	{
	public:
		const static int SampleInterval;  
		const static int DiscardSamplesOlderThanSec; 
		const static int MinMoveToStart;
		const static int MaxMoveToStart;
		const static int MinMovePerSampleToChangeDir;
		const static int MaxMovePerSampleToChangeDir;
		const static int MinMoveAllSamplesToChangeDir;
		const static int MaxMoveAllSamplesToChangeDir;
		const static int HoldTime;
		const static int IgnoreMouseAfterReposition;;
	};

	//-----------------------------------------------------------------------------------------------------
	/**
	* @brief Handles special mouse behavior, such as locking axis's, ramping and acceleration, etc.
	*/
	//-----------------------------------------------------------------------------------------------------
	class MouseBehavior
	{
		typedef enum EMouseBehaviorEvent { mb_SettingUp, mb_StartMove, mb_ChangeDirection, mb_Movement, mb_MouseUp, mb_MouseDown };
		typedef enum EMenuOnScreen { mb_None, mb_MainMenu, mb_MediaControl, mb_Ambiance };
		typedef bool ( MouseBehavior::*MouseBehaviorHandler ) ( EMouseBehaviorEvent eMouseBehaviorEvent,DesignObj_Orbiter *pObj, int Parm,int X, int Y );

	protected:
		Orbiter *m_pOrbiter;
		DesignObj_Orbiter *m_pObj_Locked_Horizontal,*m_pObj_Locked_Vertical;
		MouseBehaviorHandler m_pMouseBehaviorHandler_Horizontal,m_pMouseBehaviorHandler_Vertical;
	    unsigned long m_dwSamples[4];
		PlutoPoint m_pSamples[4];
		PlutoPoint m_pStartMovement;
		string m_sHorizontalOptions,m_sVerticalOptions;
		int m_iLockedPosition; // Either the X or Y
		char m_cLockedAxes,m_cLocked_Axis_Current;  // one of the #define AXIS_LOCK to indicate which axes are locked
		const static int m_iSpeeds[MAX_SPEEDS];
		int m_iLastSpeed,m_iLastNotch;
		int m_iPK_Button_Mouse_Last; // The last mouse button
		unsigned long m_iTime_Last_Mouse_Down,m_iTime_Last_Mouse_Up; // When it was pressed
		EMenuOnScreen m_EMenuOnScreen;
		bool m_bRepeatMenu;

		bool m_bHasTimeline;  // Will be set to true if we don't have total time and position within media
		bool m_bUseAbsoluteSeek; // If true, we are seeking to an absolute position
		int m_CurrentMedia_Start,m_CurrentMedia_Stop,m_CurrentMedia_Pos;  // The start and stop values in seconds of the media, and where we are now

	public:
		MouseBehavior(Orbiter *pOrbiter);
		virtual ~MouseBehavior() {} //no-op destructor

		void Set_Mouse_Behavior(string sOptions,bool bExclusive,string sDirection,string sDesignObj);
		bool ButtonDown(int PK_Button);     // Return true means don't process this anymore
		bool ButtonUp(int PK_Button);   // Return true means don't process this anymore
		void Move(int X,int Y);
		bool CheckForChangeInDirection(int X,int Y);
		DesignObj_Orbiter *FindChildObjectAtPosition(DesignObj_Orbiter *pObj_Parent,int X,int Y);

		void ResetSamples() { m_dwSamples[0]=m_dwSamples[1]=m_dwSamples[2]=m_dwSamples[3]=0; }
		void ShiftSamples() { m_dwSamples[2]=m_dwSamples[1]; m_pSamples[2]=m_pSamples[1]; m_dwSamples[1]=m_dwSamples[0]; m_pSamples[1]=m_pSamples[0]; }
		void Clear();

		// Override these for OS specific handling
		virtual void SetMousePosition(int X,int Y) { m_pSamples[0].X=X; m_pSamples[0].Y=Y; }
		virtual void GetMousePosition(PlutoPoint *p) { }

		// Handlers for special mouse functions
		bool LockedBar( EMouseBehaviorEvent eMouseBehaviorEvent ,DesignObj_Orbiter *pObj, int Parm,int X, int Y );
		bool SpeedControl( EMouseBehaviorEvent eMouseBehaviorEvent,DesignObj_Orbiter *pObj, int Parm,int X, int Y );
		bool LightControl( EMouseBehaviorEvent eMouseBehaviorEvent ,DesignObj_Orbiter *pObj, int Parm,int X, int Y );
		bool VolumeControl( EMouseBehaviorEvent eMouseBehaviorEvent,DesignObj_Orbiter *pObj, int Parm,int X, int Y );

		void LockedMove( int X, int Y ); // LockedBar and move event

		void SetMediaInfo(string sTime,string sTotal,string sSpeed,string sTitle,string sSection);
		bool ParsePosition(string sMediaPosition);
	};

}
//-----------------------------------------------------------------------------------------------------
#endif
