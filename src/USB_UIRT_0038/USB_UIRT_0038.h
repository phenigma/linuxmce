//<-dceag-d-b->
#ifndef USB_UIRT_0038_h
#define USB_UIRT_0038_h

//	DCE Implemenation for #1783 USB UIRT 0038

#include "Gen_Devices/USB_UIRT_0038Base.h"
//<-dceag-d-e->

#include "../LIRC_DCE/IRReceiverBase.h"
#include "IRBase/IRBase.h"

#define LEARNING_TIMEOUT 10 // Stop learning after this many seconds
	
//<-dceag-decl-b->!
namespace DCE
{
	class USB_UIRT_0038 : public USB_UIRT_0038_Command, public IRReceiverBase, public IRBase
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		USB_UIRT_0038(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~USB_UIRT_0038();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
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


	/** @brief COMMAND: #191 - Send Code */
	/** The I/R code -- usually in Pronto format */
		/** @param #9 Text */
			/** The I/R code -- usually in Pronto format */

	virtual void CMD_Send_Code(string sText) { string sCMD_Result; CMD_Send_Code(sText.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Send_Code(string sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #245 - Learn IR */
	/** The next IR code received is to be learned in Pronto format and fire a Store IR Code command to the I/R Plugin when done */
		/** @param #2 PK_Device */
			/** You can specify the device to learn for here, or you can send the command to the device itself and leave this blank */
		/** @param #8 On/Off */
			/** Turn IR Learning mode on or off
0, 1 */
		/** @param #25 PK_Text */
			/** If specified, the text object  which should contain the result of the learn command */
		/** @param #154 PK_Command */
			/** Command ID for which the learning is done for */

	virtual void CMD_Learn_IR(int iPK_Device,string sOnOff,int iPK_Text,int iPK_Command) { string sCMD_Result; CMD_Learn_IR(iPK_Device,sOnOff.c_str(),iPK_Text,iPK_Command,sCMD_Result,NULL);};
	virtual void CMD_Learn_IR(int iPK_Device,string sOnOff,int iPK_Text,int iPK_Command,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #687 - Set Screen Type */
	/** Sent by Orbiter when the screen changes to tells the i/r receiver what type of screen is displayed so it can adjust mappings if necessary. */
		/** @param #48 Value */
			/** a character: M=Main Menu, m=other menu, R=Pluto Remote, r=Non-pluto remote, N=navigable OSD on media dev, f=full screen media app, F=File Listing, c=computing list, C=Computing full screen */

	virtual void CMD_Set_Screen_Type(int iValue) { string sCMD_Result; CMD_Set_Screen_Type(iValue,sCMD_Result,NULL);};
	virtual void CMD_Set_Screen_Type(int iValue,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
