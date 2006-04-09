#ifndef __MouseGovernor_H__
#define __MouseGovernor_H__
//-----------------------------------------------------------------------------------------------------

#include "PlutoUtils/MultiThreadIncludes.h"

namespace DCE
{
	//-----------------------------------------------------------------------------------------------------
	/**
	* @brief Limits mouse movements to only 1 move per x ms to prevent extreme numbers of messages burdening the system
	*/
	//-----------------------------------------------------------------------------------------------------
	class MouseGovernor
	{
	private:
		class MouseBehavior *m_pMouseBehavior;
		bool m_bQuit,m_bThreadRunning;
		pluto_pthread_mutex_t m_GovernorMutex;
		pthread_cond_t m_GovernorCond;

		// Pointers to keep track of states
		int m_dwBufferMs; // Number of milliseconds to wait between messages
		unsigned long m_dwTime_Last_Reported_Mouse;
		int m_X,m_Y;

	public:
		MouseGovernor(MouseBehavior *pMouseBehavior);
		virtual ~MouseGovernor();

		void Run(); // Runs in a separate thread
		bool Move(int X, int Y);  // Returns false if it should not be processed in MouseBehavior just yet
		void SetBuffer(int MS);
	};
}
//-----------------------------------------------------------------------------------------------------
#endif
