//<-dceag-d-b->
#ifndef LIRC_DCE_h
#define LIRC_DCE_h

//	DCE Implemenation for #77 LIRC DCE

#include "Gen_Devices/LIRC_DCEBase.h"
//<-dceag-d-e->
#include "DCE/DeviceData_Router.h"
#include <lirc/lirc_client.h>
class Database_pluto_main;

//<-dceag-decl-b->
namespace DCE
{
	class LIRC_DCE : public LIRC_DCE_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables
		int lirc_leech(void);

//<-dceag-const-b->
public:
		// Constructors/Destructor
		LIRC_DCE(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~LIRC_DCE();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		LIRC_DCE(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Configuration();
	string DATA_Get_Mapping();
	string DATA_Get_Serial_Port();
	string DATA_Get_LIRC_Driver();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
