#include "OrbiterFactory.h"

namespace DCE
{
	/*static*/ Orbiter *OrbiterFactory::CreateOrbiter(int DeviceID,  int PK_DeviceTemplate, string ServerAddress,  
	string sLocalDirectory,  bool bLocalMode, int iImageWidth, int iImageHeight, pluto_pthread_mutex_t* pExternalScreenMutex/* = NULL*/)
	{
		//todo : add linux specific implementation maybe?
		return new Orbiter(DeviceID, PK_DeviceTemplate, ServerAddress, sLocalDirectory,  
			bLocalMode, iImageWidth, iImageHeight, pExternalScreenMutex);
	}
}

