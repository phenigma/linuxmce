//<-dceag-d-b->
#ifndef Speech_h
#define Speech_h

//	DCE Implemenation for #59 Speech

#include "Gen_Devices/SpeechBase.h"
#ifndef WIN32
	#include <s2types.h>
	#include <ad.h>
	#include <cont_ad.h>
	#include <err.h>
#endif
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class Speech : public Speech_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Speech(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Speech();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
		void SpeechRun(void);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Speech(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

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
