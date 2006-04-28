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

	public:
		int m_iLastNotch,m_iCancelLevel;  // Notch = last position on a locked vert/horiz bar.  CancelLevel = if doing discrete movement, cancel returns to this value
		string m_sCancelLevel; // Same as m_i for cases where a string is better
		bool m_bTapAndRelease; // The user is adjusting to a discrete/absolute level
		bool m_bLockAxis; // True, the default, means the framework will report Move's with the locked axis fixed over the locked object
		DesignObj_Orbiter *m_pObj;
		MouseBehavior *m_pMouseBehavior;
		bool m_bIsActive; // Means this handler is currently being used
		string m_sOptions;

		MouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior) { m_pObj=pObj; m_sOptions=sOptions; m_pMouseBehavior=pMouseBehavior; m_bLockAxis=true; m_bIsActive=false; }
		virtual ~MouseHandler() {}


		virtual void Start() {}  // This is now active (ie the user started moving in the direction this is locked onto)
		virtual void Stop() {}  // The user is now moving in another direction using a different mouse handler

		virtual bool ButtonDown(int PK_Button) { return false; }     // Return true means don't process this anymore
		virtual bool ButtonUp(int PK_Button) { return false; }   // Return true means don't process this anymore
		virtual void Move(int X,int Y,int PK_Direction) {}
		virtual void Notch(int PK_Direction,int iRepeat) {} // The user moved a 'notch' in the given direction
		virtual bool SlowDrift(int &X,int &Y) { return false; } // We're about to call a move after the user has been slowly drifting.  The handler can alter the position, and/or return true to ignore the move
		typedef enum EMouseHandler { mh_Locked, mh_Speed, mh_Light, mh_Volume, mh_Media, mh_Keyboard };
		virtual EMouseHandler TypeOfMouseHandler()=0;
	};

	#define	NUM_SAMPLES		30
	class MouseSensitivity
	{
	public:
		const static int SampleInterval;  
		const static int DiscardSamplesOlderThanSec; 
		// Threshold 1 represents the fastest, most clear movement, requiring a high ratio, for users that are very precise
		const static int Threshhold_1_Minimum;  // Minimum number of pixels to move
		const static int Threshhold_1_Ratio; // Minimum ratio : 100=baseline, 200=twice as much on the directional axis as the non-directional, 500=5x as much
		const static int Threshhold_2_Minimum;
		const static int Threshhold_2_Ratio;
		const static int Threshhold_2_Samples; // How many samples must be taken before the ratio/minimum's are lowered to threshhold 2
		const static int Threshhold_3_Minimum;
		const static int Threshhold_3_Ratio;
		const static int Threshhold_3_Samples;
		const static int HoldTime;
		const static int IgnoreMouseAfterReposition;

		const static int NumberOfSamplesForNotch;
		const static int DistanceForNotch;
		const static int UseAccelerationIfMovementsWithin;

		const static int IgnoreMovesLessThanThisPerSample;

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
	    unsigned long m_dwSamples[NUM_SAMPLES];
		unsigned long m_dwLastSampleShift;
		PlutoPoint m_pSamples[NUM_SAMPLES];
		PlutoPoint m_pLastPosition;
		PlutoPoint m_pStartMovement;
		string m_sHorizontalOptions,m_sVerticalOptions;
		int m_iLockedPosition; // Either the X or Y
		char m_cLockedAxes,m_cLocked_Axis_Current;  // one of the #define AXIS_LOCK to indicate which axes are locked
		int m_iPK_Button_Mouse_Last; // The last mouse button
		unsigned long m_iTime_Last_Mouse_Down,m_iTime_Last_Mouse_Up; // When it was pressed
		unsigned long m_dwTime_Last_Notch;
		EMenuOnScreen m_EMenuOnScreen;
		bool m_bMouseHandler_Horizontal_Exclusive,m_bMouseHandler_Vertical_Exclusive;

	public:
		MouseBehavior(Orbiter *pOrbiter);
		virtual ~MouseBehavior();

		void Set_Mouse_Behavior(string sOptions,bool bExclusive,string sDirection,string sDesignObj);
		bool ButtonDown(int PK_Button);     // Return true means don't process this anymore
		bool ButtonUp(int PK_Button);   // Return true means don't process this anymore
		void Move(int X,int Y);
		bool CheckForChangeInDirection(int &PK_Direction);
		DesignObj_Orbiter *FindChildObjectAtPosition(DesignObj_Orbiter *pObj_Parent,int X,int Y);

		void ResetSamples() { for(int i=0;i<NUM_SAMPLES;++i) m_dwSamples[i]=0; }
		void ShiftSamples() { for(int i=NUM_SAMPLES-2;i>=0;--i) { m_dwSamples[i+1]=m_dwSamples[i]; m_pSamples[i+1]=m_pSamples[i]; } m_pSamples[0].X=m_pSamples[0].Y=0; m_dwSamples[0]=0; }
		int GetDirection(PlutoPoint &pt,int *CumulativeThisDirection=NULL,int *CumulativeOtherDirection=NULL);
		void Clear(bool bGotoMainMenu=false);
		void HighlightObject(DesignObj_Orbiter *pObj);

		// Override these for OS specific handling
		virtual void SetMousePosition(int X,int Y) { m_pLastPosition.X=X; m_pLastPosition.Y=Y; /*g_pPlutoLogger->Write(LV_FESTIVAL,"SetMousePosition %d,%d",X,Y);*/ }
		virtual void GetMousePosition(PlutoPoint *p) { }
		virtual void ShowMouse(bool bShow) { }

		void SetMediaInfo(string sTime,string sTotal,string sSpeed,string sTitle,string sSection);
	};

}
//-----------------------------------------------------------------------------------------------------
#endif
