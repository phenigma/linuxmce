
//<-dceag-d-b->
#ifndef Lighting_Plugin_h
#define Lighting_Plugin_h

//	DCE Implemenation for #32 Lighting Plug-in

#include "Gen_Devices/Lighting_PluginBase.h"
//<-dceag-d-e->

#include "DCE/DCERouter.h"
#include "Orbiter/Floorplan.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "Orbiter_Plugin/FollowMe_Device.h"
class Database_pluto_main;

typedef pair<long, long> longPair;

//<-dceag-decl-b->!
namespace DCE
{
	class Lighting_Plugin : public Lighting_Plugin_Command, public DataGridGeneratorPlugIn, public FloorplanInfoProvider, public FollowMe_Device
	{
//<-dceag-decl-e->
	// Private member variables
		map<int,longPair> m_mapRoom_CommandGroup;
		ListDeviceData_Router *m_pListDeviceData_Router_Lights;

	// Private methods
public:
	// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Lighting_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Lighting_Plugin();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
	
	class Datagrid_Plugin *m_pDatagrid_Plugin;
	class Orbiter_Plugin *m_pOrbiter_Plugin;
	Database_pluto_main *m_pDatabase_pluto_main;

	bool LightingFollowMe( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
	bool LightingCommand( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
	virtual void GetFloorplanDeviceInfo(DeviceData_Router *pDeviceData_Router,EntertainArea *pEntertainArea,int iFloorplanObjectType,int &iPK_FloorplanObjectType_Color,int &Color,string &sDescription,string &OSD,int &PK_DesignObj_Toolbar);
	void PreprocessLightingMessage(DeviceData_Router *pDevice,Message *pMessage);
	int GetLightingLevel(DeviceData_Router *pDevice,int iLevel_Default=0);

	// Datagrids
	class DataGridTable *LightingScenariosGrid( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );

	// Follow-me
	virtual void FollowMe_EnteredRoom(int iPK_Event, int iPK_Orbiter, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left);
	virtual void FollowMe_LeftRoom(int iPK_Event, int iPK_Orbiter, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left);

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #184 - Set Level */
	/** Went sent by an orbiter, the plugin will adjust all lights in the Orbiter's room */
		/** @param #76 Level */
			/** The level to set, as a value between 0 (off) and 100 (full).  It can be preceeded with a - or + indicating a relative value.  +20 means up 20%. */

	virtual void CMD_Set_Level(string sLevel) { string sCMD_Result; CMD_Set_Level(sLevel.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Level(string sLevel,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
