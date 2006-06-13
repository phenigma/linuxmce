#include "Orbiter_Win32.h"
#include "SelfUpdate.h"
#include "MouseBehavior_Win32.h"
//-----------------------------------------------------------------------------------------------------
Orbiter_Win32::Orbiter_Win32(int DeviceID, int PK_DeviceTemplate, string ServerAddress, string sLocalDirectory,  
		bool bLocalMode, int iImageWidth, int iImageHeight, bool bFullScreen)
		: Orbiter(DeviceID, PK_DeviceTemplate, ServerAddress, sLocalDirectory,  
			bLocalMode, iImageWidth, iImageHeight, NULL, bFullScreen)
{
#ifdef ENABLE_MOUSE_BEHAVIOR
	m_pMouseBehavior = new MouseBehavior_Win32(this);
#endif	
}
//-----------------------------------------------------------------------------------------------------
Orbiter_Win32::~Orbiter_Win32()
{

}
//-----------------------------------------------------------------------------------------------------
bool Orbiter_Win32::SelfUpdate()
{
	OrbiterSelfUpdate orbiterSelfUpdate(this);
	return orbiterSelfUpdate.Run();
}
//-----------------------------------------------------------------------------------------------------
