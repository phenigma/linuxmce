#ifndef __MouseIterator_H__
#define __MouseIterator_H__
//-----------------------------------------------------------------------------------------------------

#include "PlutoUtils/MultiThreadIncludes.h"

namespace DCE
{
	//-----------------------------------------------------------------------------------------------------
	/**
	* @brief Limits mouse movements to only 1 move per x ms to prevent extreme numbers of messages burdening the system
	*/
	//-----------------------------------------------------------------------------------------------------
	class MouseIterator
	{
	private:
		class MouseBehavior *m_pMouseBehavior;
		bool m_bQuit,m_bThreadRunning;
		pluto_pthread_mutex_t m_IteratorMutex;
		pthread_cond_t m_IteratorCond;

		// Pointers to keep track of states
		enum EMenuOnScreen { mos_None, mos_MainMenu, mos_MediaControl, mos_Ambiance } m_EMenuOnScreen;

	public:
		MouseIterator(MouseBehavior *pMouseBehavior);
		virtual ~MouseIterator();

		void Run(); // Runs in a separate thread
	};
}
//-----------------------------------------------------------------------------------------------------
#endif
