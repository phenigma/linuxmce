//<-dceag-d-b->
#ifndef Generic_Serial_Device_h
#define Generic_Serial_Device_h

//	DCE Implemenation for #74 Generic Serial Device

#include "Gen_Devices/Generic_Serial_DeviceBase.h"
//<-dceag-d-e->

#include "Serial/GenericIODevice.h"

#include "RubySerialIOManager.h"

class Database_pluto_main;

//<-dceag-decl-b->!
namespace DCE
{
	class Generic_Serial_Device : public Generic_Serial_Device_Command
	{
//<-dceag-decl-e->
		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Generic_Serial_Device(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Generic_Serial_Device();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		virtual bool Connect(int iPK_DeviceTemplate );
//		virtual bool ReceivedMessage(class Message *pMessageOriginal);
		virtual void Transmit(const char *pData,int iSize);

		virtual void RunThread();
		
//<-dceag-const2-b->!

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

public:
		DeviceData_Impl* RecursiveFindChildDevice(unsigned dwPK_Device, DeviceData_Impl* pDeviceData_Impl);

private:
		RubySerialIOManager sermanager_;
		Database_pluto_main *m_pdbPlutoMain;
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
