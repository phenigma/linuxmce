//<-dceag-d-b->
#ifndef CM11A_h
#define CM11A_h

//	DCE Implemenation for #48 CM11A

#include "Gen_Devices/CM11ABase.h"
//<-dceag-d-e->

#include "devicepoll.h"

//<-dceag-decl-b->
namespace DCE
{
	class CM11A : public CM11A_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		CM11A(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~CM11A();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		virtual bool Connect(int iPK_DeviceTemplate);

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_COM_Port();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


//<-dceag-h-e->

private:
		CM11ADEV::DevicePoll devpoll;
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
