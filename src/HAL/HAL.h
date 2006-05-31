//<-dceag-d-b->
#ifndef HAL_h
#define HAL_h

//	DCE Implemenation for #1808 HAL

#include "Gen_Devices/HALBase.h"
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class HAL : public HAL_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:

		/**The plug and play protocol*/
		enum PnPProtocol {	UPNP = 1, 
							Proprietary = 2,
							Xap = 3,
							HAL_USB = 4,
							DHCP = 5};
							
		/**The communication method*/
		enum CommMethod {	Infrared = 1,
							RS232 = 2,
							Ethernet = 3,
							USB = 4,
							FireWire = 5,
							Bluetooth = 6};
		
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		HAL(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~HAL();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		HAL(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

		bool sendMessage(std::string params, std::string &returnValue);
		
		bool sendMessage(Message * pMsg, std::string &returnValue);

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	bool DATA_Get_Only_One_Per_PC();

			*****EVENT***** accessors inherited from base class
	void EVENT_Device_Detected(string sMac_Address,string sText,string sIP_Address,int iPK_DeviceTemplate,string sVendorModelID,int iPK_CommMethod,int iPK_PnpProtocol,string sPNP_Serial_Number,string sDeviceData,string sCategory);
	void EVENT_Device_Removed(string sMac_Address,string sText,int iPK_Device,string sIP_Address,int iPK_DeviceTemplate,string sVendorModelID,int iPK_CommMethod,int iPK_PnpProtocol,string sPNP_Serial_Number,string sDeviceData,string sCategory);

			*****COMMANDS***** we need to implement
	*/

//<-dceag-h-e->
private:

	class HalPrivate;
	HalPrivate * d;
	
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
