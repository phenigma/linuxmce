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


	/** @brief COMMAND: #195 - Dial */
	/** This will cause the device to dial a number. */
		/** @param #75 PhoneNumber */
			/** Phone number to dial. */

	virtual void CMD_Dial(string sPhoneNumber) { string sCMD_Result; CMD_Dial(sPhoneNumber.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Dial(string sPhoneNumber,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #227 - Originate */
	/** This will cause the device to originate a call */
		/** @param #75 PhoneNumber */
			/** Phone number to dial */
		/** @param #79 OriginatorNumber */
			/** Phone number from which the call is placed */
		/** @param #80 OriginatorType */
			/** Phone type (SIP/Zap) */
		/** @param #81 CallerID */
			/** Caller ID */

	virtual void CMD_Originate(string sPhoneNumber,string sOriginatorNumber,string sOriginatorType,string sCallerID) { string sCMD_Result; CMD_Originate(sPhoneNumber.c_str(),sOriginatorNumber.c_str(),sOriginatorType.c_str(),sCallerID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Originate(string sPhoneNumber,string sOriginatorNumber,string sOriginatorType,string sCallerID,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
