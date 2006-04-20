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

#define AXIS_LOCK_NONE	0
#define AXIS_LOCK_X		1
#define AXIS_LOCK_Y		2
#define AXIS_LOCK_BOTH	3

namespace DCE
{
	// A pure virtual base class for the call-backs
	class MouseHandler
	{
		friend class MouseBehavior;

	protected:
		int m_iLastNotch,m_iCancelLevel;  // Notch = last position on a locked vert/horiz bar.  CancelLevel = if doing discrete movement, cancel returns to this value
		string m_sCancelLevel; // Same as m_i for cases where a string is better
		bool m_bTapAndRelease; // The user is adjusting to a discrete/absolute level
		bool m_bLockAxis; // True, the default, means the framework will report Move's with the locked axis fixed over the locked object
		DesignObj_Orbiter *m_pObj;
		MouseBehavior *m_pMouseBehavior;

		MouseHandler(DesignObj_Orbiter *pObj,MouseBehavior *pMouseBehavior) { m_pObj=pObj; m_pMouseBehavior=pMouseBehavior; m_bLockAxis=true; }
		virtual ~MouseHandler() {}


		virtual void Start() {}  // This is now active (ie the user started moving in the direction this is locked onto)
		virtual void Stop() {}  // The user is now moving in another direction using a different mouse handler

		virtual bool ButtonDown(int PK_Button) { return false; }     // Return true means don't process this anymore
		virtual bool ButtonUp(int PK_Button) { return false; }   // Return true means don't process this anymore
		virtual void Move(int X,int Y) {}

	public:
		typedef enum EMouseHandler { mh_Locked, mh_Speed, mh_Light, mh_Volume, mh_Media, mh_Keyboard };
		virtual EMouseHandler TypeOfMouseHandler()=0;
	};

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
		friend class Orbiter;
		friend class MouseIterator;
		friend class MouseGovernor;
		friend class LightMouseHandler;
		friend class SpeedMouseHandler;
		friend class VolumeMouseHandler;
		friend class MediaMouseHandler;
		friend class LockedMouseHandler;
		friend class KeyboardMouseHandler;

		typedef enum EMenuOnScreen { mb_None, mb_MainMenu, mb_MediaControl, mb_Ambiance };

	protected:
		Orbiter *m_pOrbiter;
		class MouseGovernor *m_pMouseGovernor;
		class MouseIterator *m_pMouseIterator;
		DesignObj_Orbiter *m_pObj_Locked_Horizontal,*m_pObj_Locked_Vertical;
		MouseHandler *m_pMouseHandler_Horizontal,*m_pMouseHandler_Vertical;
	    unsigned long m_dwSamples[4];
		PlutoPoint m_pSamples[4];
		PlutoPoint m_pStartMovement;
		string m_sHorizontalOptions,m_sVerticalOptions;
		int m_iLockedPosition; // Either the X or Y
		char m_cLockedAxes,m_cLocked_Axis_Current;  // one of the #define AXIS_LOCK to indicate which axes are locked
		int m_iPK_Button_Mouse_Last; // The last mouse button
		unsigned long m_iTime_Last_Mouse_Down,m_iTime_Last_Mouse_Up; // When it was pressed
		EMenuOnScreen m_EMenuOnScreen;

	public:
		MouseBehavior(Orbiter *pOrbiter);
		virtual ~MouseBehavior();

		void Set_Mouse_Behavior(string sOptions,bool bExclusive,string sDirection,string sDesignObj);
		bool ButtonDown(int PK_Button);     // Return true means don't process this anymore
		bool ButtonUp(int PK_Button);   // Return true means don't process this anymore
		void Move(int X,int Y);
		bool CheckForChangeInDirection(int X,int Y);
		DesignObj_Orbiter *FindChildObjectAtPosition(DesignObj_Orbiter *pObj_Parent,int X,int Y);

		void ResetSamples() { m_dwSamples[0]=m_dwSamples[1]=m_dwSamples[2]=m_dwSamples[3]=0; }
		void ShiftSamples() { m_dwSamples[2]=m_dwSamples[1]; m_pSamples[2]=m_pSamples[1]; m_dwSamples[1]=m_dwSamples[0]; m_pSamples[1]=m_pSamples[0]; }
		void Clear();
		void HighlightObject(DesignObj_Orbiter *pObj);

		// Override these for OS specific handling
		virtual void SetMousePosition(int X,int Y) { m_pSamples[0].X=X; m_pSamples[0].Y=Y; g_pPlutoLogger->Write(LV_FESTIVAL,"SetMousePosition %d,%d",X,Y); }
		virtual void GetMousePosition(PlutoPoint *p) { }

		void SetMediaInfo(string sTime,string sTotal,string sSpeed,string sTitle,string sSection);
	};

}
//-----------------------------------------------------------------------------------------------------
#endif
