//<-dceag-d-b->
#ifndef CDDB_Identifier_h
#define CDDB_Identifier_h

//	DCE Implemenation for #72 CDDB Identifier

#include "Gen_Devices/CDDB_IdentifierBase.h"
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class CDDB_Identifier : public CDDB_Identifier_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		CDDB_Identifier(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~CDDB_Identifier();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		CDDB_Identifier(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
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


	/** @brief COMMAND: #314 - Identify Media */
	/** New media has been inserted and needs to be identified.  Any third party Media Identifier device can provide a description of the media. */
		/** @param #10 ID */
			/** Information about the media in XML format.  See http://plutohome.com/support/index.php?section=document&docID=188 */
		/** @param #13 Filename */
			/** The media that needs to be identified, such as /dev/cdrom under Linux, or E: under Windows */

	virtual void CMD_Identify_Media(string sFilename,string *sID) { string sCMD_Result; CMD_Identify_Media(sFilename.c_str(),sID,sCMD_Result,NULL);};
	virtual void CMD_Identify_Media(string sFilename,string *sID,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
