//<-dceag-d-b->
#include "Tira.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_DeviceCategory.h"
#ifndef WIN32
#include "TiraApi.h"
#endif

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Tira::Tira(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Tira_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, IRReceiverBase(this)
{
	IRBase::setCommandImpl(this);
	m_bIRServerRunning=false;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Tira::~Tira()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool Tira::GetConfig()
{
	if( !Tira_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	IRReceiverBase::GetConfig(m_pData);
	// Find all our sibblings that are remote controls 
	for(Map_DeviceData_Base::iterator itD=m_pData->m_AllDevices.m_mapDeviceData_Base.begin();
		itD!=m_pData->m_AllDevices.m_mapDeviceData_Base.end();++itD)
	{
		DeviceData_Base *pDevice = itD->second;
		if( pDevice->m_dwPK_Device_ControlledVia==m_pData->m_dwPK_Device_ControlledVia &&
			pDevice->m_dwPK_DeviceCategory==DEVICECATEGORY_IRTrans_Remote_Controls_CONST )
		{
			string sType;
			DCE::CMD_Get_Device_Data_Cat CMD_Get_Device_Data_Cat2(m_dwPK_Device,DEVICECATEGORY_General_Info_Plugins_CONST,true,BL_SameHouse,
				pDevice->m_dwPK_Device,DEVICEDATA_Remote_Layout_CONST,true,&sType);
			SendCommand(CMD_Get_Device_Data_Cat2);

			string sConfiguration;
			DCE::CMD_Get_Device_Data_Cat CMD_Get_Device_Data_Cat(m_dwPK_Device,DEVICECATEGORY_General_Info_Plugins_CONST,true,BL_SameHouse,
				pDevice->m_dwPK_Device,DEVICEDATA_Configuration_CONST,true,&sConfiguration);

			if( SendCommand(CMD_Get_Device_Data_Cat) && sConfiguration.size() )
			{
				vector<string> vectCodes;
				StringUtils::Tokenize(sConfiguration,"\n",vectCodes);
				for(size_t s=0;s<vectCodes.size();++s)
				{
					string::size_type pos=0;
					string sButton = StringUtils::Tokenize(vectCodes[s]," ",pos);
					while(pos<vectCodes[s].size())
					{
						string sCode = StringUtils::Tokenize(vectCodes[s]," ",pos);
						m_mapCodesToButtons[sCode] = sButton;
						g_pPlutoLogger->Write(LV_STATUS,"Code: %s will fire button %s",sCode.c_str(),sButton.c_str());
					}
				}
			}
		}
	}

#ifndef WIN32
	int res = LoadTiraDLL();
	if ( res != 0 ) 
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Unable to load Tira DLL");
		return true;  // Not much to do, return true so we don't try to reload over and over
	}
	p_tira_init();

	for(int i=0;i<9;++i)
	{
		g_pPlutoLogger->Write(LV_STATUS,"Trying to start Tira on port %d",i);
		res = p_tira_start(i);
		if ( res == 0 ) 
			break;
	}
	
	if( res!=0 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Unable to start Tira on any port");
		return true;  // Not much to do, return true so we don't try to reload over and over
	}

    res = p_tira_set_handler(OurCalback);
	if( res!=0 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Unable to register our callback");
		return true;  // Not much to do, return true so we don't try to reload over and over
	}
#endif

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Tira::Register()
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
void Tira::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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
void Tira::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!
/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c191-b->

	/** @brief COMMAND: #191 - Send Code */
	/** Sends an I/R code to a device. */
		/** @param #9 Text */
			/** The I/R code -- usually in Pronto format */

void Tira::CMD_Send_Code(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c191-e->
{
	SendIR("",sText);
}

//<-dceag-c194-b->

	/** @brief COMMAND: #194 - Toggle Power */
	/** Set relay state (0 or 1) */
		/** @param #8 On/Off */
			/** Depending on each device On/Off can be interpreted differently, but in genereal On/Off has a value of 1 for on and 0 for Off */

void Tira::CMD_Toggle_Power(string sOnOff,string &sCMD_Result,Message *pMessage)
//<-dceag-c194-e->
{
	cout << "Need to implement command #194 - Toggle Power" << endl;
	cout << "Parm #8 - OnOff=" << sOnOff << endl;
}

//<-dceag-c245-b->

	/** @brief COMMAND: #245 - Learn IR */
	/** Put gc100 into IR Learning mode */
		/** @param #8 On/Off */
			/** Turn IR Learning mode on or off
0, 1 */
		/** @param #25 PK_Text */
			/** If specified, the text object  which should contain the result of the learn command */
		/** @param #71 PK_Command_Input */
			/** Command ID for which the learning is done for */

void Tira::CMD_Learn_IR(string sOnOff,int iPK_Text,int iPK_Command_Input,string &sCMD_Result,Message *pMessage)
//<-dceag-c245-e->
{
	cout << "Need to implement command #245 - Learn IR" << endl;
	cout << "Parm #8 - OnOff=" << sOnOff << endl;
	cout << "Parm #25 - PK_Text=" << iPK_Text << endl;
	cout << "Parm #71 - PK_Command_Input=" << iPK_Command_Input << endl;
}

//<-dceag-c687-b->

	/** @brief COMMAND: #687 - Set Screen Type */
	/** Sent by Orbiter when the screen changes to tells the i/r receiver what type of screen is displayed so it can adjust mappings if necessary. */
		/** @param #48 Value */
			/** a character: M=Main Menu, m=other menu, R=Pluto Remote, r=Non-pluto remote, F=File Listing */

void Tira::CMD_Set_Screen_Type(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c687-e->
{
	cout << "Need to implement command #687 - Set Screen Type" << endl;
	cout << "Parm #48 - Value=" << iValue << endl;
}


void Tira::SendIR(string Port, string IRCode)
{
	g_pPlutoLogger->Write(LV_STATUS,"Tira Sending: %s",IRCode.c_str());

#ifndef WIN32
    int res = p_tira_transmit(2, /* repeat 3 times*/
                            -1, /* Use embedded frequency value*/
                            IRCode.c_str(),
                            IRCode.size());

    if ( res != 0 ) 
		g_pPlutoLogger->Write(LV_CRITICAL,"Tira Sending: %s",IRCode.c_str());
#endif
}

// Must override so we can call IRBase::Start() after creating children
void Tira::CreateChildren()
{
	Tira_Command::CreateChildren();
	Start();
}

