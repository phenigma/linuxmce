//<-dceag-d-b->
#ifndef EIB_h
#define EIB_h

//	DCE Implemenation for #49 EIB

#include "Gen_Devices/EIBBase.h"
//<-dceag-d-e->

#include "messagepool.h"

//<-dceag-decl-b->!
namespace DCE
{
	
	class EIB : public EIB_Command, public EIBBUS::MessagePoolInterceptor
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		EIB(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~EIB();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
        virtual bool Connect(int iPK_DeviceTemplate);
//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		EIB(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

protected:
	void handleTelegram(const EIBBUS::TelegramMessage *pt);


private:
	EIBBUS::MessagePool m_msgPool;

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


//<-dceag-h-e->
	};

	
//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
