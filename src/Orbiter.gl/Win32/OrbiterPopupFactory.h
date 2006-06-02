#pragma once
//--------------------------------------------------------------------------------------------------------------
#include "../PlutoUtils/MultiThreadIncludes.h"
#include "../PlutoUtils/singleton.h"
using namespace cpp;
using namespace cpp::Threading;
//--------------------------------------------------------------------------------------------------------------
#include "CallBackTypes.h"
#include "dialog_types.h"
//--------------------------------------------------------------------------------------------------------------
class PopupCallBackData;
//--------------------------------------------------------------------------------------------------------------
class OrbiterPopupFactoryImpl
{
public:
	OrbiterPopupFactoryImpl(void);
	~OrbiterPopupFactoryImpl(void);

	void Activate();
	void Deactivate();
	bool WorkerThreadAborted();

private:

	pthread_t m_WorkerThreadID;
	bool m_bAbortWorkerThread;
};
//--------------------------------------------------------------------------------------------------------------
typedef Singleton<OrbiterPopupFactoryImpl, CreateUsingNew<OrbiterPopupFactoryImpl>, 
	DefaultLifetime, MultiThreaded> OrbiterPopupFactory;
//--------------------------------------------------------------------------------------------------------------


