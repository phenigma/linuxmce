//<-dceag-d-b->
#include "Media_Live_LCDButtons.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "led.h"
#include "libvfd.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Media_Live_LCDButtons::Media_Live_LCDButtons(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Media_Live_LCDButtons_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, VFD_LCD_Base(40,2,20)
{
	m_pDevice_Orbiter=NULL;
	m_KeyboardLoopThread_Id=0;
	m_VfdHandle=0;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Media_Live_LCDButtons::~Media_Live_LCDButtons()
//<-dceag-dest-e->
{
	g_pPlutoLogger->Write( LV_STATUS, "Joining Keyboard Loop Thread" );
	if( m_KeyboardLoopThread_Id )
		pthread_join(m_KeyboardLoopThread_Id,NULL);

	g_pPlutoLogger->Write( LV_STATUS, "Done" );
}

//<-dceag-getconfig-b->
bool Media_Live_LCDButtons::GetConfig()
{
	if( !Media_Live_LCDButtons_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	m_pDevice_Orbiter = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_Standard_Orbiter_CONST);
	if( m_pDevice_Orbiter )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Media_Live_LCDButtons::GetConfig Orbiter: %d", m_pDevice_Orbiter->m_dwPK_Device);
		if(pthread_create( &m_KeyboardLoopThread_Id, NULL, KeyboardLoop, (void*)"/dev/input/event0") )
		{
			g_pPlutoLogger->Write( LV_CRITICAL, "Cannot create PNP thread" );
		}
	}

	m_VfdHandle=StartLCD();

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Media_Live_LCDButtons::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

//<-dceag-createinst-b->!

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Media_Live_LCDButtons::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Media_Live_LCDButtons::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/


//<-dceag-c406-b->

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
		/** @param #103 List PK Device */
			/** If going to a plugin that wil relay messages to other devices (ie orbiter_plugin and orbiter), A comma delimited list of devices to display this message on.  If going to a display device directly (like vfd/lcd) this is ignored. */

void Media_Live_LCDButtons::CMD_Display_Message(string sText,string sType,string sName,string sTime,string sList_PK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c406-e->
{
	if( !m_VfdHandle )
		return;
	NewMessage(atoi(sType.c_str()),sName,sText,atoi(sTime.c_str()));
}

//<-dceag-c837-b->

	/** @brief COMMAND: #837 - Show Media Playback State */
	/**  */
		/** @param #5 Value To Assign */
			/** Empty = no media playing, otherwise a speed, 0=pause, 1000=normal forward, -4000 = 4x reverse, etc. */

void Media_Live_LCDButtons::CMD_Show_Media_Playback_State(string sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c837-e->
{
	if( !m_VfdHandle )
		return;
}

void Media_Live_LCDButtons::DoUpdateDisplay(vector<string> *vectString)
{
	string s;
	for(vector<string>::iterator it=vectString->begin();it!=vectString->end();++it)
		s+= *it + "\n";
s="This is line 1\nThis is line 2";
	g_pPlutoLogger->Write(LV_STATUS,"Media_Live_LCDButtons::DoUpdateDisplay %s",s.c_str());
	VFDSetString(m_VfdHandle, VFD_STR_REGION_1, 0, (unsigned char *) s.c_str());
}

