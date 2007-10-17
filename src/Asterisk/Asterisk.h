/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
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
		pthread_t voicemailThread;
		std::map <int,int> dev2ext;
		// Private methods
		bool IsAsteriskServerRunning();
public:
		// Public member variables
		int GetMBQuit() {return m_bQuit_get();};
		virtual void PostConnect();
//<-dceag-const-b->
public:
		// Constructors/Destructor
		Asterisk(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Asterisk();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		virtual bool Connect(int iPK_DeviceTemplate);
		virtual void CreateChildren();

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	bool DATA_Get_Manual_configuration();

			*****EVENT***** accessors inherited from base class
	void EVENT_PBX_CommandResult(int iCommandID,int iResult,string sMessage);
	void EVENT_PBX_Ring(string sPhoneExtension,string sPhoneCallID,string sPhoneCallerID);
	void EVENT_Incoming_Call();
	void EVENT_Voice_Mail_Changed(string sValue,int iPK_Users);
	void EVENT_PBX_Hangup(string sPhoneExtension);
	void EVENT_Extensions_Status(string sText);
	void EVENT_Calls_Status(string sText);
	void EVENT_PBX_LINK(string sPhoneExtension,string sPhoneCallID,string sPhoneCallerID);

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
		/** @param #85 CommandID */
			/** CommandID which will be passed back when getting results */

	virtual void CMD_PBX_Originate(string sPhoneNumber,string sPhoneType,string sPhoneExtension,string sPhoneCallerID,int iCommandID) { string sCMD_Result; CMD_PBX_Originate(sPhoneNumber.c_str(),sPhoneType.c_str(),sPhoneExtension.c_str(),sPhoneCallerID.c_str(),iCommandID,sCMD_Result,NULL);};
	virtual void CMD_PBX_Originate(string sPhoneNumber,string sPhoneType,string sPhoneExtension,string sPhoneCallerID,int iCommandID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #235 - PBX_Transfer */
	/** Transfer a call to other phone */
		/** @param #83 PhoneExtension */
			/** Phone extension to redirect to */
		/** @param #85 CommandID */
			/** CommandID which will be passed back when getting results */
		/** @param #87 PhoneCallID */
			/** Call ID which will be transferred */
		/** @param #196 IsConference */
			/** Transfer to a conferrence room ? */

	virtual void CMD_PBX_Transfer(string sPhoneExtension,int iCommandID,string sPhoneCallID,bool bIsConference) { string sCMD_Result; CMD_PBX_Transfer(sPhoneExtension.c_str(),iCommandID,sPhoneCallID.c_str(),bIsConference,sCMD_Result,NULL);};
	virtual void CMD_PBX_Transfer(string sPhoneExtension,int iCommandID,string sPhoneCallID,bool bIsConference,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #237 - PBX_Hangup */
	/** Hangs up a call */
		/** @param #85 CommandID */
			/** Comman ID wichi will be passed back in result */
		/** @param #87 PhoneCallID */
			/** Call ID to be hanged up */

	virtual void CMD_PBX_Hangup(int iCommandID,string sPhoneCallID) { string sCMD_Result; CMD_PBX_Hangup(iCommandID,sPhoneCallID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_PBX_Hangup(int iCommandID,string sPhoneCallID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #922 - Send Asterisk Status */
	/** Ask for current extensions and calls status. */

	virtual void CMD_Send_Asterisk_Status() { string sCMD_Result; CMD_Send_Asterisk_Status(sCMD_Result,NULL);};
	virtual void CMD_Send_Asterisk_Status(string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
