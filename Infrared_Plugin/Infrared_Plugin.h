//<-dceag-d-b->
#ifndef Infrared_Plugin_h
#define Infrared_Plugin_h

//	DCE Implemenation for #39 Infrared Plug-in

#include "Gen_Devices/Infrared_PluginBase.h"
//<-dceag-d-e->

#include "DCERouter/DCERouter.h"
#include "pluto_main/Database_pluto_main.h"

//<-dceag-decl-b->
namespace DCE
{
	class Infrared_Plugin : public Infrared_Plugin_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Infrared_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Infrared_Plugin();
		virtual bool Register();
//<-dceag-const-e->
		
		Database_pluto_main *m_pDatabase_pluto_main;
		
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


	/** @brief COMMAND: #188 - Get Infrared Codes */
	/** Retrieves all the infrared codes for a given device. */
		/** @param #2 PK_Device */
			/** The device to retrieve the infrared codes for. */
		/** @param #5 Value To Assign */
			/** A tab delimited list of all the commands and infrared codes for the device.  The format is:
\t{\tData} */

	virtual void CMD_Get_Infrared_Codes(int iPK_Device,string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Infrared_Codes(iPK_Device,sValue_To_Assign,sCMD_Result,NULL);};
	virtual void CMD_Get_Infrared_Codes(int iPK_Device,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->