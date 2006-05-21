#ifndef PnpQueueEntry_h
#define PnpQueueEntry_h

using namespace std;
class Database_pluto_main;
class Row_PnpQueue;
class Row_DHCPDevice;
class Row_Device;

namespace DCE
{
    /** @brief
    */
    class PnpQueueEntry
    {
		friend class PnpQueue;
		friend class Plug_And_Play_Plugin;
		friend class Pnp_PreCreateOptions;
		friend class Pnp_PostCreateOptions;

		typedef enum { pnpqe_blocked_none, pnpqe_blocked_prompting_device_template, pnpqe_blocked_prompting_options, pnpqe_blocked_installing_software } EBlockedState;
		EBlockedState m_EBlockedState;
		time_t m_tTimeBlocked;
		Database_pluto_main *m_pDatabase_pluto_main;
		Row_PnpQueue *m_pRow_PnpQueue;
		class OH_Orbiter *m_pOH_Orbiter; // The Orbiter to use for displaying messages
		map<int,Row_DHCPDevice *> m_mapPK_DHCPDevice_possible;
		int m_iPK_DHCPDevice,m_dwPK_Device_TopLevel;
		map<int,string> m_mapPK_DeviceData;
		Row_Device *m_pRow_Device_Reported;

	public:
		// Constructor for device detected
		PnpQueueEntry(Database_pluto_main *pDatabase_pluto_main,
			string sDeviceData,
			string sIPAddress,
			string sMacAddress,
			int PK_CommMethod,
			int PK_DeviceTemplate,
			int PK_Device_Reported,
			int PK_PnpProtocol,
			string sPnpSerialNumber,
			string sText,
			string sVendorModelId);

		// Constructor for device removed
		PnpQueueEntry(Database_pluto_main *pDatabase_pluto_main,
			string sDeviceData,
			string sIPAddress,
			string sMacAddress,
			int PK_CommMethod,
			int PK_Device_Created,
			int PK_Device_Reported,
			int PK_DeviceTemplate,
			int PK_PnpProtocol,
			string sPnpSerialNumber,
			string sText,
			string sVendorModelId);

		void Stage_set(int Stage);

		void Block(EBlockedState eBlockedState);
		string DeviceDataAsString();
		void ParseDeviceData(string sDeviceData);
		void FindTopLevelDevice();
		void AssignDeviceData(Row_Device *pRow_Device);
	};
}

#endif
