//<-dceag-d-b->
#ifndef LIRC_DCE_h
#define LIRC_DCE_h

//	DCE Implemenation for #77 LIRC DCE

#include "Gen_Devices/LIRC_DCEBase.h"
//<-dceag-d-e->

#include "IRReceiverBase.h"
#include "DCE/DeviceData_Router.h"
#ifndef WIN32
#include <lirc/lirc_client.h>
#endif
class Database_pluto_main;


//<-dceag-decl-b->!
namespace DCE
{
	class LIRC_DCE : public LIRC_DCE_Command, IRReceiverBase
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables
		int lirc_leech(int DeviceID);
		map<string,int> m_mapNameToDevice;

//<-dceag-const-b->!
public:
		// Constructors/Destructor
		LIRC_DCE(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~LIRC_DCE();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		Router *pRoute;
		int m_DeviceID;
		pthread_t m_LeechingThread;

//<-dceag-const2-b->!

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Device();
	string DATA_Get_Serial_Port();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #687 - Set Screen Type */
	/** Sent by Orbiter when the screen changes to tells the i/r receiver what type of screen is displayed so it can adjust mappings if necessary. */
		/** @param #48 Value */
			/** a character: M=Main Menu, m=other menu, R=Pluto Remote, r=Non-pluto remote, F=File Listing */

	virtual void CMD_Set_Screen_Type(int iValue) { string sCMD_Result; CMD_Set_Screen_Type(iValue,sCMD_Result,NULL);};
	virtual void CMD_Set_Screen_Type(int iValue,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
