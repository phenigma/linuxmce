//<-dceag-d-b->
#ifndef Motion_Wrapper_h
#define Motion_Wrapper_h

//	DCE Implemenation for #64 Motion Wrapper

#include "Gen_Devices/Motion_WrapperBase.h"
//<-dceag-d-e->

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>

#include "DCE/DeviceData_Router.h"
class Database_pluto_main;

//<-dceag-decl-b->
namespace DCE
{
	class Motion_Wrapper : public Motion_Wrapper_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Motion_Wrapper(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Motion_Wrapper();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		virtual bool Connect(int iPK_DeviceTemplate);

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Motion_Wrapper(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Type();
	int DATA_Get_Number_of_ports();
	int DATA_Get_Video_Standard();
	int DATA_Get_Video_Input_Type();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


//<-dceag-h-e->
	private:
		bool AddChildDeviceToConfigFile(std::ofstream& conffile, DeviceData_Impl* pDeviceData, size_t i);
		
	private:
		DeviceData_Router* find_Device(int iPK_Device);
			
	private:
		pid_t motionpid_;
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
