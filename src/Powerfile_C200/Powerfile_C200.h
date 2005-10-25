//<-dceag-d-b->
#ifndef Powerfile_C200_h
#define Powerfile_C200_h

//	DCE Implemenation for #1737 Powerfile C200

#include "Gen_Devices/Powerfile_C200Base.h"
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class Powerfile_C200 : public Powerfile_C200_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Powerfile_C200(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Powerfile_C200();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Powerfile_C200(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
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


	/** @brief COMMAND: #701 - Load from Slot into Drive */
	/** Load disc from "Storage Element" (Slot) to "Data Transfer Element" (Drive) */
		/** @param #151 Slot Number */
			/** "Storage Element" (Slot) to transfer disc from */
		/** @param #152 Drive Number */
			/** "Data Transfer Element" (Disc Unit) to transfer disc to */

	virtual void CMD_Load_from_Slot_into_Drive(int iSlot_Number,int iDrive_Number) { string sCMD_Result; CMD_Load_from_Slot_into_Drive(iSlot_Number,iDrive_Number,sCMD_Result,NULL);};
	virtual void CMD_Load_from_Slot_into_Drive(int iSlot_Number,int iDrive_Number,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #702 - Unload from Drive into Slot */
	/** Unload disc from "Data Transfer Element" (Drive) to "Storage Element" (Slot) */
		/** @param #151 Slot Number */
			/** "Storage Element" (Slot) to transfer disc to */
		/** @param #152 Drive Number */
			/** "Data Transfer Element" (Disc Unit) to transfer disc from */

	virtual void CMD_Unload_from_Drive_into_Slot(int iSlot_Number,int iDrive_Number) { string sCMD_Result; CMD_Unload_from_Drive_into_Slot(iSlot_Number,iDrive_Number,sCMD_Result,NULL);};
	virtual void CMD_Unload_from_Drive_into_Slot(int iSlot_Number,int iDrive_Number,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #703 - Get Jukebox Status */
	/** Get jukebox slot and unit usage */
		/** @param #153 Jukebox Status */
			/** Jukebox Status in the following format: <Unit>=<State>[,<Unit>=<State>...]
where:
<Unit>=D0,D1 for the disc units, S1..200 for the disc storage slots
<State>=Empty or Full; occupied disc units may return Full-<number> where <number> is the source slot */

	virtual void CMD_Get_Jukebox_Status(string *sJukebox_Status) { string sCMD_Result; CMD_Get_Jukebox_Status(sJukebox_Status,sCMD_Result,NULL);};
	virtual void CMD_Get_Jukebox_Status(string *sJukebox_Status,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
