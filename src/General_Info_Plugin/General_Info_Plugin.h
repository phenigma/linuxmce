//<-dceag-d-b->
#ifndef General_Info_Plugin_h
#define General_Info_Plugin_h

//	DCE Implemenation for #27 General Info Plug-in

#include "Gen_Devices/General_Info_PluginBase.h"
//<-dceag-d-e->

#include "DeviceData_Router.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"

class Database_pluto_main;

namespace DCE
{
	class OH_Orbiter;

	class LastApplication
	{
	public:
		string m_sName;
		int m_iPK_QuickStartTemplate;
	};
};

//<-dceag-decl-b->
namespace DCE
{
	class General_Info_Plugin : public General_Info_Plugin_Command
	{
//<-dceag-decl-e->
		typedef pair<int, string> pairPKDescription;
		typedef map<string, pairPKDescription> mapMacPKDescription;
//<-dceag-const-b->
public:
		// Constructors/Destructor
		General_Info_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~General_Info_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
	
	// Private member variables
	Database_pluto_main *m_pDatabase_pluto_main;
    class Datagrid_Plugin *m_pDatagrid_Plugin;
    class Orbiter_Plugin *m_pOrbiter_Plugin;
    class Event_Plugin *m_pEvent_Plugin;
    pluto_pthread_mutex_t m_GipMutex; // Other classes may need this
    pthread_mutexattr_t m_MutexAttr;
	bool m_bRerunConfigWhenDone;
	map<int,bool> m_mapMediaDirectors_PendingConfig;
	map<int, LastApplication *> m_mapLastApplication;
	LastApplication *m_mapLastApplication_Find(int PK_Device_MD) { map<int,class LastApplication *>::iterator it = m_mapLastApplication.find(PK_Device_MD); return it==m_mapLastApplication.end() ? NULL : (*it).second; }

	// Private methods
	void SetNetBoot(DeviceData_Router *pDevice,bool bNetBoot);
	bool PendingConfigs()
	{
		PLUTO_SAFETY_LOCK(gm,m_GipMutex);
		for(map<int,bool>::iterator it=m_mapMediaDirectors_PendingConfig.begin();it!=m_mapMediaDirectors_PendingConfig.end();++it)
		{
			if( it->second )
				return true;
		}
		return false;
	}
	bool OkayToCreateDevice(int iPK_DHCPDevice,int iPK_DeviceTemplate,string sMac_address,string sIP_Address,OH_Orbiter *pOH_Orbiter,string sData);
	bool OkayToCreateDevice_AlarmPanel(int iPK_DHCPDevice,int iPK_DeviceTemplate,string sMac_address,string sIP_Address,OH_Orbiter *pOH_Orbiter,string sData);
	bool OkayToCreateDevice_NetworkStorage(int iPK_DHCPDevice,int iPK_DeviceTemplate,string sMac_address,string sIP_Address,OH_Orbiter *pOH_Orbiter,string sData);

	void PostCreateDevice(int iPK_Device,int iPK_DeviceTemplate,OH_Orbiter *pOH_Orbiter);
	void PostCreateDevice_AlarmPanel(int iPK_Device,int iPK_DeviceTemplate,OH_Orbiter *pOH_Orbiter);
	void PostCreateSecurityDevice(int iPK_Device,int iPK_DeviceTemplate,OH_Orbiter *pOH_Orbiter);
	void PostCreateDevice_NetworkStorage(int iPK_Device,int iPK_DeviceTemplate,OH_Orbiter *pOH_Orbiter);
	void PostCreateDevice_Cameras(int iPK_Device,int iPK_DeviceTemplate,OH_Orbiter *pOH_Orbiter);

	// Private methods
	list<pair<string, string> > GetUserBookmarks(string sPK_User);
	list<int> m_listNewPnpDevicesWaitingForARoom;
	void SetRoomForDevice(Row_Device *pRow_Device,Row_Room *pRow_Room);

public:
	// Public member variables

    class DataGridTable *PendingTasks( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
    class DataGridTable *QuickStartApps( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
    class DataGridTable *MRUDocuments( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
    class DataGridTable *Rooms( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
	class DataGridTable *RoomTypes(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage);
	class DataGridTable *ChildrenInfo(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage);
	class DataGridTable *AvailableSerialPorts(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage);
	class DataGridTable *LightsTypes(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage);
	class DataGridTable *InstalledAVDevices(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage);
	class DataGridTable *SensorType(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage);
	class DataGridTable *TypesOfPhones( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *PNPDevices( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *BookmarkList(string GridID, string Parms, void * ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage);
	class DataGridTable *UsersGrid( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *CountriesGrid( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *DevicesOfCategory( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *DeviceTemplatesOfCategory( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *AVWhatDelay( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *AVDiscret( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *AVMediaType( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );
	class DataGridTable *AVInputNotListed( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );

	void GetAppServerAndOsdForMD(DeviceData_Router *pDevice_MD,DeviceData_Router **pDevice_AppServer,DeviceData_Router **pDevice_Orbiter_OSD);
	Message *BuildMessageToSpawnApp(DeviceData_Router *pDevice_OrbiterRequesting,DeviceData_Router *pDevice_MD,DeviceData_Router *pDevice_AppServer,DeviceData_Router *pDevice_Orbiter_OSD,
		string sBinary,string sArguments,string sDescription,int PK_QuickStartTemplate=0);

	bool NewMacAddress( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
	bool ReportingChildDevices( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
	bool LowSystemDiskSpace( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );


	// Helper functions
	void DoneCheckingForUpdates();
	Row_Device *ProcessChildDevice(Row_Device *pRow_Device,string sLine);

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


	/** @brief COMMAND: #68 - Get Device Data */
	/** Return the device data for a device. */
		/** @param #2 PK_Device */
			/** The device which you want data from */
		/** @param #5 Value To Assign */
			/** The value. */
		/** @param #52 PK_DeviceData */
			/** What parameter to get. */
		/** @param #53 UseDefault */
			/** Report the default value, rather than requesting a live value. */

	virtual void CMD_Get_Device_Data(int iPK_Device,int iPK_DeviceData,bool bUseDefault,string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Device_Data(iPK_Device,iPK_DeviceData,bUseDefault,sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_Device_Data(int iPK_Device,int iPK_DeviceData,bool bUseDefault,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #71 - Request File */
	/** Get the contents of a file from the server */
		/** @param #13 Filename */
			/** The file to get */
		/** @param #19 Data */
			/** The file's contents */

	virtual void CMD_Request_File(string sFilename,char **pData,int *iData_Size) { string sCMD_Result; CMD_Request_File(sFilename.c_str(),pData,iData_Size,sCMD_Result,NULL);};
	virtual void CMD_Request_File(string sFilename,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #79 - Add Unknown Device */
	/** Adds an unknown device into the database.  These are devices that are not part of the Pluto system. */
		/** @param #9 Text */
			/** A description of the device */
		/** @param #10 ID */
			/** The IP Address */
		/** @param #47 Mac address */
			/** The MAC address of the device */

	virtual void CMD_Add_Unknown_Device(string sText,string sID,string sMac_address) { string sCMD_Result; CMD_Add_Unknown_Device(sText.c_str(),sID.c_str(),sMac_address.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Add_Unknown_Device(string sText,string sID,string sMac_address,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #239 - Request File And Checksum */
	/** Get the contents of a file from the server and the checksum of the file */
		/** @param #13 Filename */
			/** The file to get */
		/** @param #19 Data */
			/** The file's contents */
		/** @param #91 Checksum */
			/** The checksum of the file */
		/** @param #92 Checksum Only */
			/** If this is true, this command will return only the checksum of the file, Data will be null. */

	virtual void CMD_Request_File_And_Checksum(string sFilename,char **pData,int *iData_Size,string *sChecksum,bool *bChecksum_Only) { string sCMD_Result; CMD_Request_File_And_Checksum(sFilename.c_str(),pData,iData_Size,sChecksum,bChecksum_Only,sCMD_Result,NULL);};
	virtual void CMD_Request_File_And_Checksum(string sFilename,char **pData,int *iData_Size,string *sChecksum,bool *bChecksum_Only,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #246 - Set Device Data */
	/** Gets the device data for a device */
		/** @param #2 PK_Device */
			/** The device to set */
		/** @param #5 Value To Assign */
			/** The value to assign */
		/** @param #52 PK_DeviceData */
			/** The device data */

	virtual void CMD_Set_Device_Data(int iPK_Device,string sValue_To_Assign,int iPK_DeviceData) { string sCMD_Result; CMD_Set_Device_Data(iPK_Device,sValue_To_Assign.c_str(),iPK_DeviceData,sCMD_Result,NULL);};
	virtual void CMD_Set_Device_Data(int iPK_Device,string sValue_To_Assign,int iPK_DeviceData,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #247 - Get Device State */
	/** Gets a device's current state */

	virtual void CMD_Get_Device_State() { string sCMD_Result; CMD_Get_Device_State(sCMD_Result,NULL);};
	virtual void CMD_Get_Device_State(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #248 - Get Device Status */
	/** Gets the status for a device */

	virtual void CMD_Get_Device_Status() { string sCMD_Result; CMD_Get_Device_Status(sCMD_Result,NULL);};
	virtual void CMD_Get_Device_Status(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #272 - Restart DCERouter */
	/** Causes DCERouter to exit and restart. */
		/** @param #21 Force */

	virtual void CMD_Restart_DCERouter(string sForce) { string sCMD_Result; CMD_Restart_DCERouter(sForce.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Restart_DCERouter(string sForce,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #274 - Set Room For Device */
	/** Updates the record in the database for a given device putting in a certain room. */
		/** @param #2 PK_Device */
			/** The device */
		/** @param #50 Name */
			/** If PK_Room is empty, a new room with this name will be created */
		/** @param #57 PK_Room */
			/** The room */

	virtual void CMD_Set_Room_For_Device(int iPK_Device,string sName,int iPK_Room) { string sCMD_Result; CMD_Set_Room_For_Device(iPK_Device,sName.c_str(),iPK_Room,sCMD_Result,NULL);};
	virtual void CMD_Set_Room_For_Device(int iPK_Device,string sName,int iPK_Room,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #322 - Wake Device */
	/** Sends a Wake on LAN to the specified device. */
		/** @param #2 PK_Device */
			/** The device to wake up */

	virtual void CMD_Wake_Device(int iPK_Device) { string sCMD_Result; CMD_Wake_Device(iPK_Device,sCMD_Result,NULL);};
	virtual void CMD_Wake_Device(int iPK_Device,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #323 - Halt Device */
	/** Halts, or suspends, the given device. */
		/** @param #2 PK_Device */
			/** The device to halt */
		/** @param #21 Force */
			/** If Force is not specified this will do a suspend if the device supports suspend/resume, otherwise it will do a halt.  Force:  "H"=halt, "S"=suspend, "D"=Display off, "R"=reboot, "N"=net boot, "V"=hard drive boot */

	virtual void CMD_Halt_Device(int iPK_Device,string sForce) { string sCMD_Result; CMD_Halt_Device(iPK_Device,sForce.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Halt_Device(int iPK_Device,string sForce,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #365 - Get Room Description */
	/** Given a room or device ID, returns the description.  If device ID, also returns the room ID. */
		/** @param #2 PK_Device */
			/** The ID of the device.  Specify this or the room ID. */
		/** @param #9 Text */
			/** The description of the room */
		/** @param #57 PK_Room */
			/** The ID of the room.  Specify this or the device ID. */

	virtual void CMD_Get_Room_Description(int iPK_Device,string *sText,int *iPK_Room) { string sCMD_Result; CMD_Get_Room_Description(iPK_Device,sText,iPK_Room,sCMD_Result,NULL);};
	virtual void CMD_Get_Room_Description(int iPK_Device,string *sText,int *iPK_Room,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #371 - Is Daytime */
	/** Returns true or false to indicate if it is daytime (ie between sunrise and sunset) */
		/** @param #119 True/False */
			/** Returns true if it is daytime. */

	virtual void CMD_Is_Daytime(bool *bTrueFalse) { string sCMD_Result; CMD_Is_Daytime(bTrueFalse,sCMD_Result,NULL);};
	virtual void CMD_Is_Daytime(bool *bTrueFalse,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #395 - Check for updates */
	/** Check all PC's in the system to see if there are available updates on any of them by having all AppServer's run /usr/pluto/bin/Config_Device_Changes.sh */

	virtual void CMD_Check_for_updates() { string sCMD_Result; CMD_Check_for_updates(sCMD_Result,NULL);};
	virtual void CMD_Check_for_updates(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #396 - Check for updates done */
	/** An App Server finished running /usr/pluto/bin/Config_Device_Changes.sh and notifies the g.i. plugin. */

	virtual void CMD_Check_for_updates_done() { string sCMD_Result; CMD_Check_for_updates_done(sCMD_Result,NULL);};
	virtual void CMD_Check_for_updates_done(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #697 - Set Active Application */
	/** Tell General Info Plugin what computing application is running on an MD */
		/** @param #2 PK_Device */
			/** The media director */
		/** @param #50 Name */
			/** The name of the application */
		/** @param #146 PK_QuickStartTemplate */
			/** The quick start template */

	virtual void CMD_Set_Active_Application(int iPK_Device,string sName,int iPK_QuickStartTemplate) { string sCMD_Result; CMD_Set_Active_Application(iPK_Device,sName.c_str(),iPK_QuickStartTemplate,sCMD_Result,NULL);};
	virtual void CMD_Set_Active_Application(int iPK_Device,string sName,int iPK_QuickStartTemplate,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #700 - New Plug and Play Device */
	/** A new pnp device has been added */
		/** @param #47 Mac address */
			/** The Mac Address */
		/** @param #58 IP Address */
			/** The IP Address */
		/** @param #109 Data String */
			/** Extra device data to create the device */
		/** @param #150 PK_DHCPDevice */
			/** The template for the device */

	virtual void CMD_New_Plug_and_Play_Device(string sMac_address,string sIP_Address,string sData_String,int iPK_DHCPDevice) { string sCMD_Result; CMD_New_Plug_and_Play_Device(sMac_address.c_str(),sIP_Address.c_str(),sData_String.c_str(),iPK_DHCPDevice,sCMD_Result,NULL);};
	virtual void CMD_New_Plug_and_Play_Device(string sMac_address,string sIP_Address,string sData_String,int iPK_DHCPDevice,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #718 - Create Device */
	/** Creates a new device of the given template */
		/** @param #2 PK_Device */
			/** The new device number */
		/** @param #44 PK_DeviceTemplate */
			/** The template */
		/** @param #47 Mac address */
			/** The mac address */
		/** @param #57 PK_Room */
			/** The room for the device.  0=no room, -1=ask user */
		/** @param #58 IP Address */
			/** The IP of the device */
		/** @param #109 Data String */
			/** Extra device data to be assigned when creating the device. It should look like this:
devicedata_id1|devicedata_value1|devicedata_id2|devicedata_value2| etc. */
		/** @param #150 PK_DHCPDevice */
			/** Only needed if this is a dhcp pnp device */
		/** @param #156 PK_Device_ControlledVia */
			/** The controlled via */
		/** @param #198 PK_Orbiter */
			/** The orbiter which should be used to prompt the user for any extra information.  Zero means all orbiters */
		/** @param #201 PK_Device_Related */
			/** Will make the new device relate to this one if possible */

	virtual void CMD_Create_Device(int iPK_DeviceTemplate,string sMac_address,int iPK_Room,string sIP_Address,string sData_String,int iPK_DHCPDevice,int iPK_Device_ControlledVia,int iPK_Orbiter,int iPK_Device_Related,int *iPK_Device) { string sCMD_Result; CMD_Create_Device(iPK_DeviceTemplate,sMac_address.c_str(),iPK_Room,sIP_Address.c_str(),sData_String.c_str(),iPK_DHCPDevice,iPK_Device_ControlledVia,iPK_Orbiter,iPK_Device_Related,iPK_Device,sCMD_Result,NULL);};
	virtual void CMD_Create_Device(int iPK_DeviceTemplate,string sMac_address,int iPK_Room,string sIP_Address,string sData_String,int iPK_DHCPDevice,int iPK_Device_ControlledVia,int iPK_Orbiter,int iPK_Device_Related,int *iPK_Device,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #719 - Delete Device */
	/** Deletes a device */
		/** @param #2 PK_Device */
			/** The device to delete */

	virtual void CMD_Delete_Device(int iPK_Device) { string sCMD_Result; CMD_Delete_Device(iPK_Device,sCMD_Result,NULL);};
	virtual void CMD_Delete_Device(int iPK_Device,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #752 - Check Mounts */
	/** Re-mount all network storage devices */

	virtual void CMD_Check_Mounts() { string sCMD_Result; CMD_Check_Mounts(sCMD_Result,NULL);};
	virtual void CMD_Check_Mounts(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #765 - Set Device Relations */
	/** Set which devices are related to which */
		/** @param #2 PK_Device */
			/** The device */
		/** @param #103 List PK Device */
			/** The devices it relates to */
		/** @param #205 Reverse */
			/** If true, the device list are the source devices and the PK_Device is the relation */

	virtual void CMD_Set_Device_Relations(int iPK_Device,string sList_PK_Device,bool bReverse) { string sCMD_Result; CMD_Set_Device_Relations(iPK_Device,sList_PK_Device.c_str(),bReverse,sCMD_Result,NULL);};
	virtual void CMD_Set_Device_Relations(int iPK_Device,string sList_PK_Device,bool bReverse,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #789 - Force Update Packages */
	/** For the system to upgrade */

	virtual void CMD_Force_Update_Packages() { string sCMD_Result; CMD_Force_Update_Packages(sCMD_Result,NULL);};
	virtual void CMD_Force_Update_Packages(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #790 - Get_iPK_DeviceFromUID */
	/** Get the device ID from the UID from the USB port. */
		/** @param #206 UID */
			/** UID from USB port */

	virtual void CMD_Get_iPK_DeviceFromUID(string sUID) { string sCMD_Result; CMD_Get_iPK_DeviceFromUID(sUID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Get_iPK_DeviceFromUID(string sUID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #791 - Set Enable Status */
	/** This command will enable or disable a device. A reload router will be needed in order to take effect. */
		/** @param #2 PK_Device */
			/** The id of the device to be enabled/disabled */
		/** @param #208 Enable */
			/** If this is true ( = 1), the device will be enabled. Otherwise, the device will be disabled. */

	virtual void CMD_Set_Enable_Status(int iPK_Device,bool bEnable) { string sCMD_Result; CMD_Set_Enable_Status(iPK_Device,bEnable,sCMD_Result,NULL);};
	virtual void CMD_Set_Enable_Status(int iPK_Device,bool bEnable,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #792 - Get All HAL Model ID */
	/** Returns a list of HAL Model ID-s */

	virtual void CMD_Get_All_HAL_Model_ID() { string sCMD_Result; CMD_Get_All_HAL_Model_ID(sCMD_Result,NULL);};
	virtual void CMD_Get_All_HAL_Model_ID(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #800 - InitAVDeviceTemplateSettings */
	/** Save default values in database for a deviceTemplate */
		/** @param #44 PK_DeviceTemplate */
			/** Template id */

	virtual void CMD_InitAVDeviceTemplateSettings(int iPK_DeviceTemplate) { string sCMD_Result; CMD_InitAVDeviceTemplateSettings(iPK_DeviceTemplate,sCMD_Result,NULL);};
	virtual void CMD_InitAVDeviceTemplateSettings(int iPK_DeviceTemplate,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	private:
		mapMacPKDescription m_mapMacPKDescription;
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
