//<-dceag-d-b->
#ifndef Asterisk_h
#define Asterisk_h

//	DCE Implemenation for #45 Asterisk

#include "Gen_Devices/AsteriskBase.h"
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class Asterisk : public Asterisk_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Asterisk(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Asterisk();
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

			*****EVENT***** accessors inherited from base class
	void EVENT_Ring(string sPhoneNumber);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #233 - PBX_Originate */
	/** Originate a call */
		/** @param #75 PhoneNumber */
			/** Phone id from which to place the call */
		/** @param #82 PhoneType */
			/** Phone type from which to place the call */
		/** @param #83 PhoneExtension */
			/** Extention to dial */
		/** @param #84 PhoneCallerID */
			/** Caller id */

	virtual void CMD_PBX_Originate(string sPhoneNumber,string sPhoneType,string sPhoneExtension,string sPhoneCallerID) { string sCMD_Result; CMD_PBX_Originate(sPhoneNumber.c_str(),sPhoneType.c_str(),sPhoneExtension.c_str(),sPhoneCallerID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_PBX_Originate(string sPhoneNumber,string sPhoneType,string sPhoneExtension,string sPhoneCallerID,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
