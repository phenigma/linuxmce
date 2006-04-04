#ifndef __MouseBehavior_H__
#define __MouseBehavior_H__
//-----------------------------------------------------------------------------------------------------
#include <list>
#include <string>
#include <map>
using namespace std;

#include "DesignObj_Orbiter.h"
#include "Orbiter.h"
using namespace DCE;

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
	};

	//-----------------------------------------------------------------------------------------------------
	/**
	* @brief Handles special mouse behavior, such as locking axis's, ramping and acceleration, etc.
	*/
	//-----------------------------------------------------------------------------------------------------
	class MouseBehavior
	{
	protected:
		Orbiter *m_pOrbiter;
		DesignObj_Orbiter *m_pObj_Locked_Horizontal,*m_pObj_Locked_Vertical;
	    unsigned long m_dwSamples[4];
		PlutoPoint m_pSamples[4];
		PlutoPoint m_pStartMovement;
		string m_sHorizontalOptions,m_sVerticalOptions;
		int m_iLockedPosition; // Either the X or Y
		char m_cLockedAxes,m_cLocked_Axis_Current;  // one of the #define AXIS_LOCK to indicate which axes are locked

	public:
		MouseBehavior(Orbiter *pOrbiter);

		void Set_Mouse_Behavior(string sOptions,bool bExclusive,string sDirection,string sDesignObj);
		void Down(int X,int Y);
		void Up(int X,int Y);
		void Move(int X,int Y);
		bool CheckForChangeInDirection(int X,int Y);
		DesignObj_Orbiter *FindChildObjectAtPosition(DesignObj_Orbiter *pObj_Parent,int X,int Y);

		void ResetSamples() { m_dwSamples[0]=m_dwSamples[1]=m_dwSamples[2]=m_dwSamples[3]=0; }
		void ShiftSamples() { m_dwSamples[2]=m_dwSamples[1]; m_pSamples[2]=m_pSamples[1]; m_dwSamples[1]=m_dwSamples[0]; m_pSamples[1]=m_pSamples[0]; }

		// Override these for OS specific handling
		virtual void SetMousePosition(int X,int Y) { m_pSamples[0].X=X; m_pSamples[0].Y=Y; }
		virtual void GetMousePosition(PlutoPoint *p) { }
	};
}
//-----------------------------------------------------------------------------------------------------
#endif
