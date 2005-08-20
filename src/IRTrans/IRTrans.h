//<-dceag-d-b->
#ifndef IRTrans_h
#define IRTrans_h

//	DCE Implemenation for #1710 IRTrans - DIGN case

#include "Gen_Devices/IRTransBase.h"
//<-dceag-d-e->

#include "../LIRC_DCE/IRReceiverBase.h"
#include "../VFD_LCD/VFD_LCD_Base.h"

//<-dceag-decl-b->!
namespace DCE
{
	class IRTrans : public IRTrans_Command, IRReceiverBase, VFD_LCD_Base
	{
//<-dceag-decl-e->
		// Private member variables
		bool m_bIRServerRunning;
		map<string,int> m_mapNameToDevice;

		// Private methods
public:
		// Public member variables
		void StartIRServer();
		void GotIRCommand(const char *pRemote,const char *pCommand);
		void DoUpdateDisplay(string sMessage);  // Put this message on the VFD Display

//<-dceag-const-b->
public:
		// Constructors/Destructor
		IRTrans(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~IRTrans();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

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
