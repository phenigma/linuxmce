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
		friend class MouseBehavior;
		friend class VolumeMouseHandler;
		friend class LightMouseHandler;
		friend class MediaMouseHandler;
		friend class KeyboardMouseHandler;

		class MouseBehavior *m_pMouseBehavior;
		class Orbiter *m_pOrbiter;
		bool m_bQuit,m_bThreadRunning;
		pluto_pthread_mutex_t m_IteratorMutex;
		pthread_cond_t m_IteratorCond;

		// Pointers to keep track of states
		enum EIteratorFunction { if_None, if_Volume, if_Light, if_MediaTracks, if_Keyboard } m_EIteratorFunction;
		int m_dwParm; // Parm for the function
		int m_dwFrequency_Ms;  // Frequency for doing the iteration in milliseconds
		int m_dwTime_Last_Iterated;  // When we last did our iteration

	public:
		MouseIterator(MouseBehavior *pMouseBehavior);
		virtual ~MouseIterator();

		void Run(); // Runs in a separate thread
		void SetIterator(EIteratorFunction eIteratorFunction,int dwParm,int dwFrequency_Ms);
		void DoIteration();
	};
}
//-----------------------------------------------------------------------------------------------------
#endif
