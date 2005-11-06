//<-dceag-d-b->
#ifndef ZWave_h
#define ZWave_h

//	DCE Implemenation for #1750 ZWave

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

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		ZWave(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

		void SyncDeviceList();

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Remote_Phone_IP();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #709 - Discover New Devices */
	/** Scans for new devices and adds them to the system */

	virtual void CMD_Discover_New_Devices() { string sCMD_Result; CMD_Discover_New_Devices(sCMD_Result,NULL);};
	virtual void CMD_Discover_New_Devices(string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
