//<-dceag-d-b->
#ifndef General_Info_Plugin_h
#define General_Info_Plugin_h

//	DCE Implemenation for #27 General Info Plug-in

#include "Gen_Devices/General_Info_PluginBase.h"
//<-dceag-d-e->

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

	// Private methods
public:
	// Public member variables

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


	/** @brief COMMAND: #67 - Spawn Application */
	/** Spawns on application */
		/** @param #13 Filename */
			/** The name of the executable file to spawn */
		/** @param #50 Name */
			/** A name that we'll remember the application by for future kill commands */
		/** @param #51 Arguments */
			/** Command arguments */
		/** @param #94 SendOnFailure */
			/** Send this messages if the process exited with failure error code. */
		/** @param #95 SendOnSuccess */
			/** Send this messages if the process exited with success error code. */

	virtual void CMD_Spawn_Application(string sFilename,string sName,string sArguments,string sSendOnFailure,string sSendOnSuccess) { string sCMD_Result; CMD_Spawn_Application(sFilename.c_str(),sName.c_str(),sArguments.c_str(),sSendOnFailure.c_str(),sSendOnSuccess.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Spawn_Application(string sFilename,string sName,string sArguments,string sSendOnFailure,string sSendOnSuccess,string &sCMD_Result,Message *pMessage);


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


	/** @brief COMMAND: #268 - Reboot */
	/** Reboots the given computer. */
		/** @param #2 PK_Device */
			/** The computer to reboot.  This is ignored when AppServer receives the command--it reboots itself.  When General Info Plugin receives it, it reboots the mentioned computer. */

	virtual void CMD_Reboot(int iPK_Device) { string sCMD_Result; CMD_Reboot(iPK_Device,sCMD_Result,NULL);};
	virtual void CMD_Reboot(int iPK_Device,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #272 - Restart DCERouter */
	/** Causes DCERouter to exit and restart. */

	virtual void CMD_Restart_DCERouter() { string sCMD_Result; CMD_Restart_DCERouter(sCMD_Result,NULL);};
	virtual void CMD_Restart_DCERouter(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #274 - Set Room For Device */
	/** Updates the record in the database for a given device putting in a certain room. */
		/** @param #2 PK_Device */
			/** The device */
		/** @param #57 PK_Room */
			/** The room */

	virtual void CMD_Set_Room_For_Device(int iPK_Device,int iPK_Room) { string sCMD_Result; CMD_Set_Room_For_Device(iPK_Device,iPK_Room,sCMD_Result,NULL);};
	virtual void CMD_Set_Room_For_Device(int iPK_Device,int iPK_Room,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
