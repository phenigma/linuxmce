#ifndef __SELF_UPDATE_H__
#define __SELF_UPDATE_H__

#include <string>
#include "Orbiter.h"
//-----------------------------------------------------------------------------------------------------
class OrbiterSelfUpdate
{
	string m_sOrbiterFilePath;
    string m_sUpdateBinaryPath;
	Orbiter *m_pOrbiter;

public:
	OrbiterSelfUpdate(Orbiter *pOrbiter);
	~OrbiterSelfUpdate();

	static void GetProcessFilePath(char *pProcessFilePath);
	//bool IsTheNewOrbiter();
	string GetOrbiterCheckSum();
	bool UpdateAvailable();
	bool DownloadUpdateBinary();
	bool CreateCommunicationFile();
	bool SpawnUpdateBinaryProcess();
	bool LastUpdateFailed();

	bool Run();
};
//-----------------------------------------------------------------------------------------------------
#endif //__SELF_UPDATE_H__
