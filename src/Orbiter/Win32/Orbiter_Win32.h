#ifndef __ORBITER_WIN32_H__
#define __ORBITER_WIN32_H__
//-----------------------------------------------------------------------------------------------------
#include "Orbiter.h"
using namespace DCE;
//-----------------------------------------------------------------------------------------------------
namespace DCE
{
	class Orbiter_Win32 : public Orbiter
	{
	public:

		Orbiter_Win32(int DeviceID, int PK_DeviceTemplate, string ServerAddress, string sLocalDirectory,  
			bool bLocalMode, int iImageWidth, int iImageHeight, bool bFullScreen);

		~Orbiter_Win32();	

		bool SelfUpdate();
	};
}
//-----------------------------------------------------------------------------------------------------
#endif __ORBITER_WIN32_H__
