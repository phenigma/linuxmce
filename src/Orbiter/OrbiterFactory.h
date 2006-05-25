#ifndef __ORBITER_FACTORY_H__
#define __ORBITER_FACTORY_H__

#include "Orbiter.h"

namespace DCE
{
	class OrbiterFactory
	{
	public:
		static Orbiter *CreateOrbiter(int DeviceID,  int PK_DeviceTemplate, string ServerAddress,  string sLocalDirectory,  
			bool bLocalMode,  int iImageWidth,  int iImageHeight, pluto_pthread_mutex_t* pExternalScreenMutex = NULL);
	};
}

#endif //__ORBITER_FACTORY_H__