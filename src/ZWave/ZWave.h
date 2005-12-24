//<-dceag-d-b->
#ifndef ZWave_h
#define ZWave_h

//	DCE Implemenation for #1754 ZWave

#include "Gen_Devices/ZWaveBase.h"
//<-dceag-d-e->

#include "DCE/PlainClientSocket.h"

//<-dceag-decl-b->
namespace DCE
{
	class ZWave : public ZWave_Command
	{
//<-dceag-decl-e->
		// Private member variables
		PlainClientSocket *m_pPlainClientSocket;
	    pluto_pthread_mutex_t m_ZWaveMutex; 
	    pthread_mutexattr_t m_MutexAttr;

		// Private methods
		bool ConfirmConnection();
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		ZWave(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~ZWave();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->!

		void DownloadConfiguration();
		void ReportChildDevices();

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Remote_Phone_IP();

			*****EVENT***** accessors inherited from base class
	void EVENT_Reporting_Child_Devices(string sError_Message,string sText);
	void EVENT_Download_Config_Done(string sError_Message);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #756 - Report Child Devices */
	/** Report all the child sensors this has by firing an event 'Reporting Child Devices' */

	virtual void CMD_Report_Child_Devices() { string sCMD_Result; CMD_Report_Child_Devices(sCMD_Result,NULL);};
	virtual void CMD_Report_Child_Devices(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #757 - Download Configuration */
	/** Download new configuration data for this device */
		/** @param #9 Text */
			/** Any information the device may want to do the download */

	virtual void CMD_Download_Configuration(string sText) { string sCMD_Result; CMD_Download_Configuration(sText.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Download_Configuration(string sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #760 - Send Command To Child */
	/** After reporting new child devices, there may be children we want to test, but we haven't done a quick reload router and can't send them messages directly.  This way we can send 'live' messages to children */
		/** @param #10 ID */
			/** The internal ID used for this device--not the Pluto device ID. */
		/** @param #154 PK_Command */
			/** The command to send */
		/** @param #202 Parameters */
			/** Parameters for the command in the format:
PK_CommandParameter|Value|... */

	virtual void CMD_Send_Command_To_Child(string sID,int iPK_Command,string sParameters) { string sCMD_Result; CMD_Send_Command_To_Child(sID.c_str(),iPK_Command,sParameters.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Send_Command_To_Child(string sID,int iPK_Command,string sParameters,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
