//<-dceag-d-b->
#ifndef MythTV_Backend_Proxy_h
#define MythTV_Backend_Proxy_h

//	DCE Implemenation for #63 MythTV Backend Proxy

#include "Gen_Devices/MythTV_Backend_ProxyBase.h"
//<-dceag-d-e->

#include "masterproxyserver.h"

//<-dceag-decl-b->
namespace DCE
{
	class MythTV_Backend_Proxy : public MythTV_Backend_Proxy_Command, public MYTHTV::ProxyEventHandler
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		MythTV_Backend_Proxy(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~MythTV_Backend_Proxy();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		
		virtual bool Connect();

		
//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		MythTV_Backend_Proxy(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

protected:
		void ChannelChanged(const char *host, const char* channelid);

private:
		MYTHTV::MasterProxyServer proxy_;
		
		
public:
//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class
	void EVENT_MythTV_Channel_Changed(string sFrontEnd_IP_Address,string sChannelID);

			*****COMMANDS***** we need to implement
	*/


//<-dceag-h-e->
	};
	
//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
