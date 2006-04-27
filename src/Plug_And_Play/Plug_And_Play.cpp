//<-dceag-d-b->
#include "Plug_And_Play.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "main.h"

#include "PlutoUtils/DatabaseUtils.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_PnpQueue.h"
#include "DCE/DeviceData_Router.h"

#include "DCE/DCERouter.h"
#include "DCE/DCEConfig.h"

class Plug_And_Play::PnPPrivate
{
	public:
	
		PnPPrivate();
		
		~PnPPrivate();
	
		Database_pluto_main * database;
		DeviceData_Base * pnpCoreDevice;
		DCEConfig config;
		bool core;
		
	private:
};

Plug_And_Play::PnPPrivate::PnPPrivate()
	: database(NULL),
	  pnpCoreDevice(NULL),
	  core(true)
{
	
}

Plug_And_Play::PnPPrivate::~PnPPrivate()
{
	delete database;
	database = NULL;
}


//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Plug_And_Play::Plug_And_Play(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Plug_And_Play_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	d = new PnPPrivate();
	if( d != NULL )
	{
	}
	else
	{
		// error
	}
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
Plug_And_Play::Plug_And_Play(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: Plug_And_Play_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
	d = new PnPPrivate();
	if( d != NULL )
	{
	}
	else
	{
		// error
	}
}

//<-dceag-dest-b->
Plug_And_Play::~Plug_And_Play()
//<-dceag-dest-e->
{
	delete d;
	d = NULL;
}

//<-dceag-getconfig-b->
bool Plug_And_Play::GetConfig()
{
	if( !Plug_And_Play_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	if( d != NULL )
	{
		if( d->core )
		{
			d->database = new Database_pluto_main( );
			if( d->database == NULL ||
	/*			!d->database->Connect(	m_pRouter->sDBHost_get(), m_pRouter->sDBUser_get(), m_pRouter->sDBPassword_get(),
										m_pRouter->sDBName_get(), m_pRouter->iDBPort_get()) )*/
				!d->database->Connect(	d->config.m_sDBHost, d->config.m_sDBUser, d->config.m_sDBPassword,
										d->config.m_sDBName, d->config.m_iDBPort ) )
			{
				g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to database!" );
				m_bQuit = true;
				return false;
			}
		}
	}
	else
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Not enough memory!" );
		m_bQuit = true;
		return false;
	}
	
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Plug_And_Play::Register()
//<-dceag-reg-e->
{
//    RegisterMsgInterceptor(( MessageInterceptorFn )( &Plug_And_Play::ConfigPre ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Device_ConfigPre_CONST );
//    RegisterMsgInterceptor(( MessageInterceptorFn )( &Plug_And_Play::ConfigOn ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Device_ConfigOn_CONST );
//    RegisterMsgInterceptor(( MessageInterceptorFn )( &Plug_And_Play::ConfigPost ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Device_ConfigPost_CONST );

	return Connect(PK_DeviceTemplate_get());
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
Plug_And_Play_Command *Create_Plug_And_Play(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new Plug_And_Play(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Plug_And_Play::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Plug_And_Play::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void Plug_And_Play::SomeFunction()
{
	// If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
	// to do all your registration, such as creating message interceptors

	// If you use an IDE with auto-complete, after you type DCE:: it should give you a list of all
	// commands and requests, including the parameters.  See "AllCommandsRequests.h"

	// Examples:
	
	// Send a specific the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.
	DCE::CMD_Simulate_Mouse_Click CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);
	SendCommand(CMD_Simulate_Mouse_Click);

	// Send the message to orbiters 32898 and 27283 (ie a device list, hence the _DL)
	// And we want a response, which will be "OK" if the command was successfull
	string sResponse;
	DCE::CMD_Simulate_Mouse_Click_DL CMD_Simulate_Mouse_Click_DL(m_dwPK_Device,"32898,27283",55,77)
	SendCommand(CMD_Simulate_Mouse_Click_DL,&sResponse);

	// Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
	// Note the _Cat for category
	DCE::CMD_Simulate_Mouse_Click_Cat CMD_Simulate_Mouse_Click_Cat(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77)
    SendCommand(CMD_Simulate_Mouse_Click_Cat);

	// Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
	// Note the _DT.
	DCE::CMD_Simulate_Mouse_Click_DT CMD_Simulate_Mouse_Click_DT(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);
	SendCommand(CMD_Simulate_Mouse_Click_DT);

	// This command has a normal string parameter, but also an int as an out parameter
	int iValue;
	DCE::CMD_Get_Signal_Strength CMD_Get_Signal_Strength(m_dwDeviceID, DestDevice, sMac_address,&iValue);
	// This send command will wait for the destination device to respond since there is
	// an out parameter
	SendCommand(CMD_Get_Signal_Strength);  

	// This time we don't care about the out parameter.  We just want the command to 
	// get through, and don't want to wait for the round trip.  The out parameter, iValue,
	// will not get set
	SendCommandNoResponse(CMD_Get_Signal_Strength);  

	// This command has an out parameter of a data block.  Any parameter that is a binary
	// data block is a pair of int and char *
	// We'll also want to see the response, so we'll pass a string for that too

	int iFileSize;
	char *pFileContents
	string sResponse;
	DCE::CMD_Request_File CMD_Request_File(m_dwDeviceID, DestDevice, "filename",&pFileContents,&iFileSize,&sResponse);
	SendCommand(CMD_Request_File);

	// If the device processed the command (in this case retrieved the file),
	// sResponse will be "OK", and iFileSize will be the size of the file
	// and pFileContents will be the file contents.  **NOTE**  We are responsible
	// free deleting pFileContents.


	// To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

	// Get our IP address from our data
	string sIP = DATA_Get_IP_Address();

	// Set our data "Filename" to "myfile"
	DATA_Set_Filename("myfile");

	// Fire the "Finished with file" event, which takes no parameters
	EVENT_Finished_with_file();
	// Fire the "Touch or click" which takes an X and Y parameter
	EVENT_Touch_or_click(10,150);
}
*/
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/


//<-dceag-c798-b->

	/** @brief COMMAND: #798 - PlugAndPlayAddDevice */
	/** Adds or enables the newly discovered device */
		/** @param #44 PK_DeviceTemplate */
			/** The device template ID for the discovered device */
		/** @param #58 IP Address */
			/** The IP address where was the new device detected. */
		/** @param #70 Tokens */
			/** Extra parameters in the form of | (pipe) separated tokens */
		/** @param #214 PNPSerialNo */
			/** The serial number of the device, computed by the PNP discovery daemons */
		/** @param #215 PK_PnpProtocol */
			/** The PNP protocol of the daemon that discovered the device */
		/** @param #216 Identifier */
			/** The device model and producer identifier */
		/** @param #217 Capabilities */
			/** A variable that defines the daemons' capability to perform custom operations */
		/** @param #218 PK_CommMethod */
			/** Communication method */
		/** @param #219 Path */
			/** The path where should run the scripts and binaries used by PnP. */

void Plug_And_Play::CMD_PlugAndPlayAddDevice(int iPK_DeviceTemplate,string sIP_Address,string sTokens,string sPNPSerialNo,int iPK_PnpProtocol,string sIdentifier,int iCapabilities,int iPK_CommMethod,string sPath,string &sCMD_Result,Message *pMessage)
//<-dceag-c798-e->
{
	if( d->core )
	{
		//clear the result string
		sCMD_Result.clear();
		
		//check the queue to see if the device is not in processing
		if( iPK_DeviceTemplate < 0 )
		{
			// set the right state
			// PNP_DEVICE_DETECTED
		}
		else
		{
			// PNP_DEVICE_PROMPT_FOR_PARAMETERS
		}
		
		string sSQL = string("where SerialNumber = ") + sPNPSerialNo;
		
		vector<Row_PnpQueue*> vectRow_PnpQueue;
		vector<Row_PnpQueue*>::iterator vectRow_PnpQueue_iterator;
		
		Table_PnpQueue *p_Table_PnpQueue = d->database->PnpQueue_get();
		if( p_Table_PnpQueue != NULL )
		{
			p_Table_PnpQueue->GetRows(sSQL, &vectRow_PnpQueue);
			if( vectRow_PnpQueue.empty() )
			{
				// add it to the queue
				Row_PnpQueue * row = p_Table_PnpQueue->AddRow();
				if( row != NULL )
				{
					row->FK_DeviceTemplate_set( iPK_DeviceTemplate );
					row->FK_Device_set( 0 );
					row->FK_CommMethod_set( iPK_CommMethod  );
					row->FK_PnpProtocol_set( iPK_PnpProtocol );
					row->DetectedDate_set( "Data" );
					row->SerialNumber_set( sPNPSerialNo );
					row->Identifier_set( sIdentifier );
					row->Path_set( sPath );
					// TODO
					// row->IP_set( sIP_Address );
					// row->ExtraParameters_set( sTokens );
					
					if( !p_Table_PnpQueue->Commit() )
					{
						// error
						g_pPlutoLogger->Write(LV_CRITICAL, "cannot commit to database");
						return;
					}
				}
				else
				{
					// error
					g_pPlutoLogger->Write(LV_CRITICAL, "cannot add row");
					return;
				}
			}
			else
			{
				// nothing ??
				g_pPlutoLogger->Write(LV_CRITICAL,  "no table" );				
				return;
			}
		}
		
		//check the device table to see if there is already a device with the specified serial no
	}
	else
	{
		// find the Core then find the PnP device from Core
		// and proxy the messages to it
		if( d->pnpCoreDevice == NULL )
		{
			DeviceData_Base * pDevice_Core = 
				m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory( DEVICECATEGORY_Core_CONST );
			if( pDevice_Core != NULL )
			{
				d->pnpCoreDevice = pDevice_Core->FindFirstRelatedDeviceOfTemplate( DEVICETEMPLATE_Plug_And_Play_CONST );
				if( d->pnpCoreDevice != NULL )
				{
					// send the message
				}
			}
		}
		else
		{
			// send the message
			DCE::CMD_PlugAndPlayAddDevice cmd(	m_dwPK_Device, d->pnpCoreDevice->m_dwPK_Device,
												iPK_DeviceTemplate,
												sIP_Address,
												sTokens,
												sPNPSerialNo,
												iPK_PnpProtocol,
												sIdentifier,
												iCapabilities,
												iPK_CommMethod,
												sPath );
//			DCE::CMD_ cmd(m_dwPK_Device, d->pnpCoreDevice->m_dwPK_Device, ..)
//			SendCommand(cmd);
		}
	}
	
	//check the queue to see if the device is not in processing
	if( iPK_DeviceTemplate < 0 )
	{
		// set the right state
		// PNP_DEVICE_DETECTED
	}
	else
	{
		// PNP_DEVICE_PROMPT_FOR_PARAMETERS
	}
	
	string sSQL = string("where SerialNumber = ") + sPNPSerialNo;
	
	vector<Row_PnpQueue*> vectRow_PnpQueue;
	vector<Row_PnpQueue*>::iterator vectRow_PnpQueue_iterator;
	
	Table_PnpQueue *p_Table_PnpQueue = d->database->PnpQueue_get();
	if( p_Table_PnpQueue != NULL )
	{
		p_Table_PnpQueue->GetRows(sSQL, &vectRow_PnpQueue);
		if( vectRow_PnpQueue.empty() )
		{
			// add it to the queue
			Row_PnpQueue * row = p_Table_PnpQueue->AddRow();
			if( row != NULL )
			{
				row->FK_DeviceTemplate_set( iPK_DeviceTemplate );
				row->FK_Device_set( 0 );
				row->FK_CommMethod_set( iPK_CommMethod  );
				row->FK_PnpProtocol_set( iPK_PnpProtocol );
				row->DetectedDate_set( "Data" );
				row->SerialNumber_set( sPNPSerialNo );
				row->Identifier_set( sIdentifier );
				row->Path_set( sPath );
				// TODO
				// row->IP_set( sIP_Address );
				// row->ExtraParameters_set( sTokens );
				
				if( !p_Table_PnpQueue->Commit() )
				{
					// error
					g_pPlutoLogger->Write(LV_CRITICAL, "cannot commit to database");
				}
			}
			else
			{
				g_pPlutoLogger->Write(LV_CRITICAL, "cannot add row");
				// error
			}
		}
		else
		{
			g_pPlutoLogger->Write(LV_CRITICAL,  "no table" );
			// nothing ??
		}
	}
	

	//check the device table to see if there is already a device with the specified serial no
	
}

//<-dceag-c799-b->

	/** @brief COMMAND: #799 - PlugAndPlayRemoveDevice */
	/** Disables the newly discovered device */
		/** @param #214 PNPSerialNo */
			/** The serial number detected by the PNP discovery layers */

void Plug_And_Play::CMD_PlugAndPlayRemoveDevice(string sPNPSerialNo,string &sCMD_Result,Message *pMessage)
//<-dceag-c799-e->
{
	//clear the result string
	sCMD_Result.clear();
	if( d->core )
	{
	}
	else
	{
	}
}

void Plug_And_Play::CheckQueue()
{

}

void Plug_And_Play::Pre_Config_Response( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	if(pMessage->m_dwID != EVENT_PnP_Pre_Config_Response_CONST)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "the event is not EVENT_PnP_Pre_Config_Response_CONST");
		return;
	}
	//get the response from the pre_config function
}

void Plug_And_Play::Do_Config_Response( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	if(pMessage->m_dwID != EVENT_PnP_Do_Config_Response_CONST)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "the event is not EVENT_PnP_Do_Config_Response_CONST");
		return;
	}
	//get the response from the do_config function
}

void Plug_And_Play::Post_Config_Response( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	if(pMessage->m_dwID != EVENT_PnP_Post_Config_Response_CONST)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "the event is not EVENT_PnP_Post_Config_Response_CONST");
		return;
	}
	//get the response from the post_config function
}


void Plug_And_Play::Set_Device_Template( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	int iPK_DeviceTemplate = 0;
	int iPK_PnpQueue = 0;

	//get the device template and the pnp queue
//	class DeviceData_Router* p_deviceData_RouterFrom = (class DeviceData_Router*)pDeviceFrom;
//	class DeviceData_Router* p_deviceData_RouterTo = (class DeviceData_Router*)pDeviceTo;

	if(pMessage->m_dwID != EVENT_PnP_Set_Device_Template_CONST)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "the event is not EVENT_PnP_Set_Device_Template_CONST");
		return;
	}
	
	string templateID = pMessage->m_mapParameters[COMMANDPARAMETER_PK_DeviceTemplate_CONST];
	string pnpQueueID = pMessage->m_mapParameters[EVENTPARAMETER_PK_PnpQueue_CONST];

	iPK_DeviceTemplate = atoi(templateID.c_str());
	iPK_PnpQueue = atoi(pnpQueueID.c_str());

	Table_PnpQueue *table_PnpQueue = d->database->PnpQueue_get();
	if(table_PnpQueue != NULL)
	{
		Row_PnpQueue* row_PnpQueue = table_PnpQueue->GetRow(iPK_PnpQueue);
		if(row_PnpQueue != NULL)
		{
			row_PnpQueue->FK_DeviceTemplate_set(iPK_DeviceTemplate);
			if(!table_PnpQueue->Commit())
			{
				//error
				g_pPlutoLogger->Write(LV_CRITICAL, "cannot commit to database");
			}
		}
	}
}
