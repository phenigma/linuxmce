//<-dceag-d-b->
#ifndef Generic_Analog_Camera_h
#define Generic_Analog_Camera_h

//	DCE Implemenation for #66 Generic Analog Camera

#include "Gen_Devices/Generic_Analog_CameraBase.h"
//<-dceag-d-e->
#include "DCE/DeviceData_Router.h"
class Database_pluto_main;

//<-dceag-decl-b->
namespace DCE
{
	class Generic_Analog_Camera : public Generic_Analog_Camera_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Generic_Analog_Camera(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Generic_Analog_Camera();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Generic_Analog_Camera(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_Port_Number();
	int DATA_Get_Motion_Option();
	int DATA_Get_Sensitivity();
	int DATA_Get_Noise();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


//<-dceag-h-e->
		private:
		    DeviceData_Router* find_Device(int iPK_Device);
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
