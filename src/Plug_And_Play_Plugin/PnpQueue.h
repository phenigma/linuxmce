#ifndef PnpQueue_h
#define PnpQueue_h

using namespace std;
class Database_pluto_main;
class Row_PnpQueue;

#define TIMEOUT_PROMPTING_USER					120

#define	PNP_DETECT_STAGE_DETECTED				100
#define	PNP_DETECT_STAGE_CONFIRMING_POSSIBLE_DT	101
#define	PNP_DETECT_STAGE_PROMPTING_USER_FOR_DT	102
#define	PNP_DETECT_STAGE_PROMPTING_USER_FOR_OPT	103
#define	PNP_DETECT_STAGE_ADD_DEVICE				104
#define	PNP_DETECT_STAGE_ADD_SOFTWARE			105
#define	PNP_DETECT_STAGE_START_DEVICE			106
#define	PNP_DETECT_STAGE_DONE					107

#define PNP_REMOVE_STAGE_REMOVED				200
#define PNP_REMOVE_STAGE_DONE					201

class Database_pluto_main;

#include "Pnp_PreCreateOptions.h"

namespace DCE
{
    /** @brief
    */
    class PnpQueue
    {
		friend class Plug_And_Play_Plugin;
		friend class Pnp_PreCreateOptions;
		map<int,class PnpQueueEntry *> m_mapPnpQueueEntry;
		PnpQueueEntry *m_mapPnpQueueEntry_Find(int PK_PnpQueue) { map<int,class PnpQueueEntry *>::iterator it = m_mapPnpQueueEntry.find(PK_PnpQueue); return it==m_mapPnpQueueEntry.end() ? NULL : (*it).second; }
		class Plug_And_Play_Plugin *m_pPlug_And_Play_Plugin;
		Database_pluto_main *m_pDatabase_pluto_main;
		bool m_bThreadRunning;
		Pnp_PreCreateOptions m_Pnp_PreCreateOptions;

	public:
		PnpQueue(class Plug_And_Play_Plugin *pPlug_And_Play_Plugin);
		~PnpQueue();
		void Run();  // The main loop

		// Main entry points
		void NewEntry(PnpQueueEntry *pPnpQueueEntry);

		// Helper functions
		bool LocateDevice(PnpQueueEntry *pPnpQueueEntry);
		bool CheckForDeviceTemplateOnWeb(PnpQueueEntry *pPnpQueueEntry);
		void RunPnpDetectionScript(PnpQueueEntry *pPnpQueueEntry);

		// The various process functions,
		bool Process(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Detected(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Confirm_Possible_DT(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Prompting_User_For_DT(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Prompting_User_For_Options(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Add_Device(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Add_Software(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Start_Device(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Remove_Stage_Removed(PnpQueueEntry *pPnpQueueEntry);
	};
}

#endif
