//<-dceag-d-b->
#ifndef General_Info_Plugin_h
#define General_Info_Plugin_h

//	DCE Implemenation for #27 General Info Plug-in

#include "Gen_Devices/General_Info_PluginBase.h"
//<-dceag-d-e->

#include "DeviceData_Router.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"

class Database_pluto_main;
//<-dceag-decl-b->
namespace DCE
{
	class General_Info_Plugin : public General_Info_Plugin_Command
	{
//<-dceag-decl-e->
//<-dceag-const-b->
public:
		// Constructors/Destructor
		General_Info_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~General_Info_Plugin();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
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

public:
	// Public member variables

    class DataGridTable *PendingTasks( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
    class DataGridTable *QuickStartApps( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );
    class DataGridTable *MRUDocuments( string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage );

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


//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
