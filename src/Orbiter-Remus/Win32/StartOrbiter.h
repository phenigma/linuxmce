#ifndef __START_ORBITER_CE_H__
#define __START_ORBITER_CE_H__

#include <string>
using namespace std;

void StartOrbiter(
	int PK_Device,
	int PK_DeviceTemplate,
	string sRouter_IP,
	string sLocalDirectory,
	bool bLocalMode,
	int Width,
	int Height,
	bool bFullScreen
);

#endif //__START_ORBITER_CE_H__