#ifndef PnpQueue_h
#define PnpQueue_h

using namespace std;
class Database_pluto_main;
class Row_PnpQueue;

#define TIMEOUT_PROMPTING_USER					120  // If the user doesn't respond in this many seconds, try asking again
#define TIMEOUT_DETECTION_SCRIPT				20	// Maximum number of seconds to wait for a detction script to return

#define	PNP_DETECT_STAGE_DETECTED				10
#define	PNP_DETECT_STAGE_CONFIRMING_POSSIBLE_DT	11
#define	PNP_DETECT_STAGE_RUNNING_DETECTION_SCRIPTS	12
#define	PNP_DETECT_STAGE_PROMPTING_USER_FOR_DT	13
#define	PNP_DETECT_STAGE_PROMPTING_USER_FOR_OPT	14
#define	PNP_DETECT_STAGE_ADD_DEVICE				15
#define	PNP_DETECT_STAGE_ADD_SOFTWARE			16
#define	PNP_DETECT_STAGE_START_DEVICE			17
#define	PNP_DETECT_STAGE_DONE					18

#define PNP_REMOVE_STAGE_REMOVED				20
#define PNP_REMOVE_STAGE_DONE					21

class Database_pluto_main;
class Row_Device;

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
		void ReleaseQueuesBlockedFromPromptingState(PnpQueueEntry *pPnpQueueEntry);
		bool BlockIfOtherQueuesAtPromptingState(PnpQueueEntry *pPnpQueueEntry);
		void ReadOutstandingQueueEntries();
		void DetermineOrbitersForPrompting(PnpQueueEntry *pPnpQueueEntry);
		string GetDescription(PnpQueueEntry *pPnpQueueEntry);
		bool DeviceMatchesCriteria(Row_Device *pRow_Device,PnpQueueEntry *pPnpQueueEntry);
		void SetDisableFlagForDeviceAndChildren(Row_Device *pRow_Device,bool bDisabled);

		// The various process functions,
		bool Process(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Detected(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Confirm_Possible_DT(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Running_Detction_Scripts(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Prompting_User_For_DT(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Prompting_User_For_Options(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Add_Device(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Add_Software(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Start_Device(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Remove_Stage_Removed(PnpQueueEntry *pPnpQueueEntry);
	};
}

#endif
