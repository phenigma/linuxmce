//<-dceag-d-b->
#ifndef Generic_Input_Ouput_h
#define Generic_Input_Ouput_h

//	DCE Implemenation for #43 Generic Input Ouput

#include "Gen_Devices/Generic_Input_OuputBase.h"
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class Generic_Input_Ouput : public Generic_Input_Ouput_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Generic_Input_Ouput(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Generic_Input_Ouput();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_Channel();
	int DATA_Get_InputOrOutput();
	int DATA_Get_Default_State();

			*****EVENT***** accessors inherited from base class
	void EVENT_Pin_Changed(bool bOnOff);

			*****COMMANDS***** we need to implement
	*/

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->