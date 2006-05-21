#ifndef Pnp_PreCreateOptions_h
#define Pnp_PreCreateOptions_h

using namespace std;

class Database_pluto_main;
class Row_DeviceTemplate;

namespace DCE
{
	class OH_Orbiter;
	class PnpQueueEntry;

    /** @brief
    */
    class Pnp_PreCreateOptions
    {
		class PnpQueue *m_pPnpQueue;
		Database_pluto_main *m_pDatabase_pluto_main;
	public:
		Pnp_PreCreateOptions(class PnpQueue *pPnpQueue,class Plug_And_Play_Plugin *pPlug_And_Play_Plugin);

		bool OkayToCreateDevice(PnpQueueEntry *pPnpQueueEntry);
		bool OkayToCreateDevice_AlarmPanel(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate);
		bool OkayToCreateDevice_NetworkStorage(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate);
	};
}

#endif
