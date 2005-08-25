//<-dceag-d-b->
#ifndef IRTrans_h
#define IRTrans_h

//	DCE Implemenation for #1710 IRTrans - DIGN IR Receiver

#include "Gen_Devices/IRTransBase.h"
//<-dceag-d-e->

#include "../LIRC_DCE/IRReceiverBase.h"
#include "../VFD_LCD/VFD_LCD_Base.h"
#include "IRBase/IRBase.h"

//<-dceag-decl-b->!
namespace DCE
{
	class IRTrans : public IRTrans_Command, public IRReceiverBase, public VFD_LCD_Base, public IRBase
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
		void DoUpdateDisplay(vector<string> *vectString);  // Put this message on the VFD Display
		virtual void SendIR(string Port, string IRCode); // Required from IRBase
		virtual void CreateChildren(); // Must override so we can call IRBase::Start() after creating children

//<-dceag-const-b->
public:
		// Constructors/Destructor
		IRTrans(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~IRTrans();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		virtual void OnQuit() { m_bQuit_VL = true; Command_Impl::OnQuit(); };

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


	/** @brief COMMAND: #406 - Display Message */
	/** Display a message on the lcd/vfd display */
		/** @param #9 Text */
			/** The message to display */
		/** @param #14 Type */
			/** For devices implementing VFD_LCD_Base, this is the message type defined in the header */
		/** @param #50 Name */
			/** you can give the message a name, such as "status", "error", etc */
		/** @param #102 Time */
			/** Number of seconds to display the message for */
		/** @param #103 PK_Device_List */
			/** If going to a plugin that wil relay messages to other devices (ie orbiter_plugin and orbiter), A comma delimited list of devices to display this message on.  If going to a display device directly (like vfd/lcd) this is ignored. */

	virtual void CMD_Display_Message(string sText,string sType,string sName,string sTime,string sPK_Device_List) { string sCMD_Result; CMD_Display_Message(sText.c_str(),sType.c_str(),sName.c_str(),sTime.c_str(),sPK_Device_List.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Display_Message(string sText,string sType,string sName,string sTime,string sPK_Device_List,string &sCMD_Result,Message *pMessage);


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
