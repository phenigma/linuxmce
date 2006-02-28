//
// C++ Interface: PlutoZWSerialAPI
//
//
// Author:	Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//  		Edgar Grimberg <edgar.g@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
//<-dceag-d-b->
#ifndef ZWave_h
#define ZWave_h

//	DCE Implemenation for #1754 ZWave

#include "Gen_Devices/ZWaveBase.h"
//<-dceag-d-e->

#include "DCE/PlainClientSocket.h"
#include "PlutoZWSerialAPI.h"

//<-dceag-decl-b->
namespace DCE
{
	class ZWave : public ZWave_Command
	{
//<-dceag-decl-e->
		// Private member variables
	    pluto_pthread_mutex_t m_ZWaveMutex; 
	    pthread_mutexattr_t m_MutexAttr;
		
		// ZWave API
		PlutoZWSerialAPI * m_ZWaveAPI;
		
		/** Flag used by the pooling thread.*/
		static bool m_PoolStarted;
		
		/** Pooling thread */
		static pthread_t m_PoolThread;

		// Private methods
		
		/** Returns the serial/USB device.*/
		string GetZWaveSerialDevice();
		
		/** Starts or stops the ZWave pooling thread.*/
		void CMD_Pool(bool start);
		
		bool ConfirmConnection(int iRetryCount=0);
		
public:
	
		// Public member variables
		
		// Public methods
		
		/** Get the nodes state, one by one.*/
		void PoolZWaveNetwork();

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
	string DATA_Get_COM_Port_on_PC();

			*****EVENT***** accessors inherited from base class
	void EVENT_Device_OnOff(bool bOnOff);
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


	/** @brief COMMAND: #770 - HD */
	/** Test comand. Erase at the end */

	virtual void CMD_HD() { string sCMD_Result; CMD_HD(sCMD_Result,NULL);};
	virtual void CMD_HD(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #771 - AV 4 */
	/** Test command only. 
Turn relay on/off */

	virtual void CMD_AV_4() { string sCMD_Result; CMD_AV_4(sCMD_Result,NULL);};
	virtual void CMD_AV_4(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #776 - Reset */
	/** Reset Zwave device. */
		/** @param #51 Arguments */
			/** Argument string
NOEMON or CANBUS */

	virtual void CMD_Reset(string sArguments) { string sCMD_Result; CMD_Reset(sArguments.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Reset(string sArguments,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #787 - Open */
	/** Test comand. Erase at the end */

	virtual void CMD_Open() { string sCMD_Result; CMD_Open(sCMD_Result,NULL);};
	virtual void CMD_Open(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #788 - StatusReport */
	/** Test comand. Erase at the end
Asq a report */
		/** @param #51 Arguments */
			/** Argument string */

	virtual void CMD_StatusReport(string sArguments) { string sCMD_Result; CMD_StatusReport(sArguments.c_str(),sCMD_Result,NULL);};
	virtual void CMD_StatusReport(string sArguments,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
