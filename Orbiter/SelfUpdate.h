#ifndef __SELF_UPDATE_H__
#define __SELF_UPDATE_H__

#include <string>
#include "Orbiter.h"
//-----------------------------------------------------------------------------------------------------
class OrbiterSelfUpdate
{
	Orbiter *m_pOrbiter;
	string m_sOrbiterFilePath;

public:
	OrbiterSelfUpdate(Orbiter *pOrbiter);
	~OrbiterSelfUpdate();

	void GetProcessFilePath(char *pProcessFilePath);
	bool IsTheNewOrbiter();
	string GetOrbiterCheckSum();

	bool Run();
};
//-----------------------------------------------------------------------------------------------------
#endif //__SELF_UPDATE_H__
