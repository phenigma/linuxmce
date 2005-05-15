/**
 *
 * @file Notification.h
 * @brief header file for the Notification class
 *
 */
 
//<-dceag-d-b->
#ifndef Notification_h
#define Notification_h

#include "PlutoUtils/MultiThreadIncludes.h"

class Row_Alert;

namespace DCE
{
	class Notification 
	{
		// Private member variables 
	    pluto_pthread_mutex_t m_AlertMutex;
		Row_Alert *m_pRow_Alert;
		class Security_Plugin *m_pSecurity_Plugin;
		class Router *m_pRouter;

		Notification(Security_Plugin *pSecurity_Plugin,Router *pRouter,Row_Alert *pRow_Alert);
	};
}
#endif
