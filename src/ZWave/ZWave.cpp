//<-dceag-d-b->
#include "ZWave.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "PlutoUtils/MySQLHelper.h"
#include "PlutoUtils/LinuxSerialUSB.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"

#include "ZW_classcmd.h"
#include "ZWaveNode.h"
#include "ZWJobInitialize.h"
#include "ZWJobReceive.h"
#include "ZWJobPool.h"
#include "ZWJobReset.h"
#include "ZWJobSwitchChangeLevel.h"

#ifdef _WIN32 
#include <windows.h> 
#include <winbase.h> 
#define POOL_DELAY 200 
#else 
#include <unistd.h> 
#define POOL_DELAY 200000 
#endif

#define ZW_TIMEOUT 15
#define ZW_LONG_TIMEOUT 120

bool ZWave::m_PoolStarted = false;
pthread_t ZWave::m_PoolThread;

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
ZWave::ZWave(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: ZWave_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_ZWaveMutex("zwave")
	, m_ZWaveAPI(NULL)
{
    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
    m_ZWaveMutex.Init( &m_MutexAttr );

	m_ZWaveAPI = PlutoZWSerialAPI::instance();
	if( m_ZWaveAPI == NULL )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Not enough memory for ZWave API");
	}
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
ZWave::~ZWave()
//<-dceag-dest-e->
{
	CMD_Pool(false);
}

//<-dceag-getconfig-b->
bool ZWave::GetConfig()
{
	if( !ZWave_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// try to avoid trylock timeout
	if( TRYLOCK_TIMEOUT_WARNING <= m_ZWaveAPI->timeLeft() )
	{
		g_pPlutoLogger->Write(LV_ZWAVE,"Force ZWave to stop.");
		m_ZWaveAPI->stop();
	}
	
	if( !ConfirmConnection() )
	{
		g_pPlutoLogger->Write(LV_WARNING, "Cannot connect to ZWave device %s.", zwaveSerialDevice.c_str());
		// TODO HMM ?!
		// return false;
	}
	else
	{
		CMD_Report_Child_Devices();
	}
	
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool ZWave::Register()
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
void ZWave::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	// try to avoid trylock timeout
	if( TRYLOCK_TIMEOUT_WARNING <= m_ZWaveAPI->timeLeft() )
	{
		g_pPlutoLogger->Write(LV_ZWAVE,"Force ZWave to stop.");
		m_ZWaveAPI->stop();
	}
	
	if( !ConfirmConnection() )
	{
		sCMD_Result = "NO ZWAVE";
		return;
	}

	PLUTO_SAFETY_LOCK(zm,m_ZWaveMutex);

	int NodeID = atoi(pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST).c_str());
	if( NodeID > 0 && NodeID <= 233 && 
		NULL != m_ZWaveAPI->getNode( NodeID ) )
	{
		sCMD_Result = "OK";
		if( pMessage->m_dwID == COMMAND_Generic_On_CONST )
		{
			g_pPlutoLogger->Write(LV_ZWAVE, "Sending ON - %d", NodeID);
			ZWJobSwitchChangeLevel * light = new ZWJobSwitchChangeLevel(m_ZWaveAPI, 99, (unsigned char)NodeID);
			if( light != NULL )
			{
				m_ZWaveAPI->insertJob( light );
				if( !m_ZWaveAPI->start( zwaveSerialDevice.c_str() ) ||
					!m_ZWaveAPI->listen(ZW_TIMEOUT) )
				{
					sCMD_Result = "DEVICE DIDN'T RESPOND OR ZWAVE ERRORS";
					return;
				}
			}
			else
			{
				sCMD_Result = "NOT ENOUGH MEMORY";
				return;
			}
		}
		else if( pMessage->m_dwID == COMMAND_Generic_Off_CONST )
		{
			g_pPlutoLogger->Write(LV_ZWAVE, "Sending Off - %d", NodeID);
			ZWJobSwitchChangeLevel * light = new ZWJobSwitchChangeLevel(m_ZWaveAPI, 0, (unsigned char)NodeID);
			if( light != NULL )
			{
				m_ZWaveAPI->insertJob( light );
				if( !m_ZWaveAPI->start( zwaveSerialDevice.c_str() ) ||
					!m_ZWaveAPI->listen(ZW_TIMEOUT) )
				{
					sCMD_Result = "DEVICE DIDN'T RESPOND OR ZWAVE ERRORS";
					return;
				}
			}
			else
			{
				sCMD_Result = "NOT ENOUGH MEMORY";
				return;
			}
		}
		else if( pMessage->m_dwID == COMMAND_Set_Level_CONST )
		{
			int level = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str());
			g_pPlutoLogger->Write(LV_ZWAVE, "Sending LEVEL - %d || %d", level, NodeID);
			ZWJobSwitchChangeLevel * light = new ZWJobSwitchChangeLevel(m_ZWaveAPI, level, (unsigned char)NodeID);
			if( light != NULL )
			{
				m_ZWaveAPI->insertJob( light );
				if( !m_ZWaveAPI->start( zwaveSerialDevice.c_str() ) ||
					!m_ZWaveAPI->listen(ZW_TIMEOUT) )
				{
					sCMD_Result = "DEVICE DIDN'T RESPOND OR ZWAVE ERRORS";
					return;
				}
			}
			else
			{
				sCMD_Result = "NOT ENOUGH MEMORY";
				return;
			}
		}
		else
			sCMD_Result = "UNHANDLED CHILD";
	}
	else
	{
		sCMD_Result = "BAD NODE ID";
		g_pPlutoLogger->Write(LV_WARNING, "Child device doesn't have a zwave node id");
		return;
	}
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void ZWave::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

void *DoDownloadConfiguration(void *p)
{
	ZWave *pZWave = (ZWave *) p;
	pZWave->DownloadConfiguration();
	return NULL;
}

void ZWave::DownloadConfiguration()
{
//EVENT_Download_Config_Done("");
//return;

	// try to avoid trylock timeout
	if( TRYLOCK_TIMEOUT_WARNING <= m_ZWaveAPI->timeLeft() )
	{
		g_pPlutoLogger->Write(LV_ZWAVE, "Force ZWave to stop.");
		m_ZWaveAPI->stop();
	}
	
	if( !ConfirmConnection() )
	{
		g_pPlutoLogger->Write(LV_WARNING, "Cannot connect to ZWave");
		EVENT_Download_Config_Done("Cannot connect to ZWave");
		return;
	}

	PLUTO_SAFETY_LOCK(zm,m_ZWaveMutex);
	g_pPlutoLogger->Write(LV_ZWAVE, "ZWave::DownloadConfiguration trying to get list of devices");
	
	ZWJobReceive * receive = new ZWJobReceive(m_ZWaveAPI);
	ZWJobInitialize * init = new ZWJobInitialize(m_ZWaveAPI);
	if( receive != NULL && init != NULL )
	{
		m_ZWaveAPI->insertJob( receive );
		m_ZWaveAPI->insertJob( init );
		if( !m_ZWaveAPI->start( zwaveSerialDevice.c_str() ) ||
			!m_ZWaveAPI->listen(ZW_LONG_TIMEOUT) )
		{
			EVENT_Download_Config_Done("DEVICE DIDN'T RESPOND OR ZWAVE ERRORS");
			return;
		}
	}
	else
	{
		delete receive;
		receive = NULL;
		delete init;
		init = NULL;
		
		EVENT_Download_Config_Done("NOT ENOUGH MEMORY");
		return;
	}
	
	EVENT_Download_Config_Done("");
}

void ZWave::ReportChildDevices()
{
/*
EVENT_Reporting_Child_Devices("",
"1\tNode 1\t\t37\t9\n"
"2\tNode 2\t\t38\t10\n"
"3\tNode 3\t\t37\t11\n"
);
return;
*/
	// try to avoid trylock timeout
	if( TRYLOCK_TIMEOUT_WARNING <= m_ZWaveAPI->timeLeft() )
	{
		g_pPlutoLogger->Write(LV_ZWAVE, "Force ZWave to stop.");
		m_ZWaveAPI->stop();
	}
	
	if( !ConfirmConnection() )
	{
		g_pPlutoLogger->Write(LV_WARNING, "Cannot connect to ZWave");
		EVENT_Reporting_Child_Devices("Cannot connect to ZWave", "");
		return;
	}

	PLUTO_SAFETY_LOCK(zm,m_ZWaveMutex);
	g_pPlutoLogger->Write(LV_ZWAVE, "ZWave::ReportChildDevices trying to get list of devices");
	
	ZWJobInitialize * init = new ZWJobInitialize(m_ZWaveAPI);
	if( init != NULL )
	{
		m_ZWaveAPI->insertJob( init );
		if( !m_ZWaveAPI->start( zwaveSerialDevice.c_str() ) ||
			!m_ZWaveAPI->listen(ZW_LONG_TIMEOUT) )
		{
			EVENT_Reporting_Child_Devices("DEVICE DIDN'T RESPOND OR ZWAVE ERRORS", "");
			return;
		}
	}
	else
	{
		EVENT_Reporting_Child_Devices("NOT ENOUGH MEMORY", "");
		return;
	}
	
	std::string sResponse;
	char buffer[256];
	ZWaveNode * node = NULL;
	const NodesMap & nodes = m_ZWaveAPI->getNodes();
	for(NodesMapCIterator itNode=nodes.begin(); itNode!=nodes.end(); ++itNode)
	{
		node = (*itNode).second;
		if( node != NULL )
		{
			NodeType type = node->type();
			if( node->nodeID() != m_ZWaveAPI->nodeID() )
				// TODO: && specific type == SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL
			{
				switch( type.generic )
				{
					// dimmer
					case GENERIC_TYPE_SWITCH_MULTILEVEL:
						// [internal id] \t [description] \t [room name] \t [device template] \t [floorplan id] \n
						snprintf(buffer, sizeof(buffer), "%d\t\t\t38\t", node->nodeID());
						sResponse += buffer;
						sResponse += "\n";
						break;
						
					// light on/off
					case GENERIC_TYPE_SWITCH_BINARY:
						// [internal id] \t [description] \t [room name] \t [device template] \t [floorplan id] \n
						snprintf(buffer, sizeof(buffer), "%d\t\t\t37\t", node->nodeID());
						sResponse += buffer;
						sResponse += "\n";
						break;
						
					default:
						break;
				}
			}
		}
	}
	
	g_pPlutoLogger->Write(LV_ZWAVE,"ZWave::ReportChildDevices got:");
	g_pPlutoLogger->Write(LV_ZWAVE,"%s", sResponse.c_str());
	
	EVENT_Reporting_Child_Devices("", sResponse);
}

bool ZWave::ConfirmConnection(int RetryCount)
{
	PLUTO_SAFETY_LOCK(zm,m_ZWaveMutex);
	
	if( m_ZWaveAPI != NULL )
	{
		// force stopping the current ZWave command
		m_ZWaveAPI->stop();
		// force cleaning the jobs queue
		m_ZWaveAPI->clearJobs();
		
		if( PlutoZWSerialAPI::STOPPED == m_ZWaveAPI->state() &&
			( 0 == m_ZWaveAPI->nodeID() ||
		      0 == m_ZWaveAPI->homeID() ) )
		{
			zwaveSerialDevice = GetZWaveSerialDevice();
			if( !zwaveSerialDevice.empty() )
			{
				ZWJobInitialize * init = new ZWJobInitialize(m_ZWaveAPI);
				if( init != NULL )
				{
					m_ZWaveAPI->insertJob( init );
					if( m_ZWaveAPI->start( zwaveSerialDevice.c_str() ) &&
						m_ZWaveAPI->listen(ZW_TIMEOUT) )
					{
						// start ZWave pooling
						CMD_Pool(true);
						
						return true;
					}
				}
			}
			else
			{
				g_pPlutoLogger->Write(LV_ZWAVE,"ZWave::ConfirmConnection : zwave device is empty");
			}
		}
		else
		{
			return true;
		}
	}
	
	return false;
}

void *DoReportChildDevices(void *p)
{
	ZWave *pZWave = (ZWave *) p;
	pZWave->ReportChildDevices();
	return NULL;
}

//<-dceag-c756-b->

	/** @brief COMMAND: #756 - Report Child Devices */
	/** Report all the child sensors this has by firing an event 'Reporting Child Devices' */

void ZWave::CMD_Report_Child_Devices(string &sCMD_Result,Message *pMessage)
//<-dceag-c756-e->
{
	pthread_t t;
	pthread_create(&t, NULL, DoReportChildDevices, (void*)this);
}
//<-dceag-c757-b->

	/** @brief COMMAND: #757 - Download Configuration */
	/** Download new configuration data for this device */
		/** @param #9 Text */
			/** Any information the device may want to do the download */

void ZWave::CMD_Download_Configuration(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c757-e->
{
	pthread_t t;
	pthread_create(&t, NULL, DoDownloadConfiguration, (void*)this);
}

void *DoPooling(void *p)
{
	ZWave *pZWave = (ZWave *) p;
	pZWave->PoolZWaveNetwork();
	return NULL;
}

void ZWave::PoolZWaveNetwork()
{
	g_pPlutoLogger->Write(LV_DEBUG, "PoolZWaveNetwork : begin");
	
	// start with 30 sec delay
	// so that the children will be reported
	// before pooling the network
	unsigned i = 1;
	while( m_PoolStarted )
	{
		// 30 sec = 150 x 200 ms
		if( i >= 150 )
		{
			i = 0;
		}
		
		if( !i /*&& TRYLOCK_TIMEOUT_WARNING <= m_ZWaveAPI->timeLeft()*/ )
		{
			PLUTO_SAFETY_LOCK(zm,m_ZWaveMutex);
			
			ZWJobPool * pool = new ZWJobPool(m_ZWaveAPI);
			if( pool != NULL )
			{
				m_ZWaveAPI->insertJob( pool );
				if( m_ZWaveAPI->start( zwaveSerialDevice.c_str() ) &&
					m_ZWaveAPI->listen(ZW_LONG_TIMEOUT) )
				{
					// checking the current state for each children
					DeviceData_Impl *pChildDevice = NULL;
					for( VectDeviceData_Impl::const_iterator it = m_pData->m_vectDeviceData_Impl_Children.begin();
						 it != m_pData->m_vectDeviceData_Impl_Children.end(); ++it )
					{
						pChildDevice = (*it);
						if( pChildDevice != NULL )
						{
							int NodeID = atoi(pChildDevice->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST).c_str());
							ZWaveNode * node = m_ZWaveAPI->getNode(NodeID);
							if( node != NULL )
							{
								NodeType type = node->type();
								if( node->nodeID() != m_ZWaveAPI->nodeID() &&
									-1 != node->level() &&
									node->isChanged() )
								{
									switch( type.generic )
									{
										// dimmer
										case GENERIC_TYPE_SWITCH_MULTILEVEL:
											g_pPlutoLogger->Write(LV_ZWAVE, "PoolZWaveNetwork : new state for node %d", node->nodeID());
											m_pEvent->SendMessage(
												new Message(pChildDevice->m_dwPK_Device,
															DEVICEID_EVENTMANAGER,
															PRIORITY_NORMAL,
															MESSAGETYPE_EVENT,
															EVENT_Device_OnOff_CONST,
															1,
															EVENTPARAMETER_OnOff_CONST,
															0 < node->level() ? "1" : "0"));
											break;
											
										// light on/off
										case GENERIC_TYPE_SWITCH_BINARY:
											g_pPlutoLogger->Write(LV_ZWAVE, "PoolZWaveNetwork : new state for node %d", node->nodeID());
											m_pEvent->SendMessage(
												new Message(pChildDevice->m_dwPK_Device,
															DEVICEID_EVENTMANAGER,
															PRIORITY_NORMAL,
															MESSAGETYPE_EVENT,
															EVENT_Device_OnOff_CONST,
															1,
															EVENTPARAMETER_OnOff_CONST,
															0 < node->level() ? "1" : "0"));
											break;
											
										default:
											break;
									}
								}
							}
							else
							{
								g_pPlutoLogger->Write(LV_ZWAVE,"PoolZWaveNetwork : zwave node is null");
							}
						}
						else
						{
							g_pPlutoLogger->Write(LV_ZWAVE,"PoolZWaveNetwork : zwave child device is null");
						}
					}
				}
				else
				{
					g_pPlutoLogger->Write(LV_ZWAVE,"PoolZWaveNetwork : couldn't run the job");
				}
			}
			else
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"PoolZWaveNetwork : not enough memory");
			}
			
			
//			zm.Release();
		}
		i++;
		
		
#ifdef _WIN32
			Sleep(POOL_DELAY); 
#else
			usleep(POOL_DELAY); 
#endif
	}
	
	g_pPlutoLogger->Write(LV_DEBUG,"PoolZWaveNetwork : end");
}

void ZWave::CMD_Pool(bool start)
{
	if( start )
	{
		if( !m_PoolStarted )
		{
			if( !pthread_create(&m_PoolThread, NULL, DoPooling, (void*)this) )
			{
				m_PoolStarted = true;
			}
			else
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"CMD_Pool : couldn't create the thread");
			}
		}
	}
	else
	{
		if( m_PoolStarted )
		{
			m_PoolStarted = false;
			pthread_join(m_PoolThread, NULL);
		}
	}
}

//<-dceag-c760-b->

	/** @brief COMMAND: #760 - Send Command To Child */
	/** After reporting new child devices, there may be children we want to test, but we haven't done a quick reload router and can't send them messages directly.  This way we can send 'live' messages to children */
		/** @param #10 ID */
			/** The internal ID used for this device--not the Pluto device ID. */
		/** @param #154 PK_Command */
			/** The command to send */
		/** @param #202 Parameters */
			/** Parameters for the command in the format:
PK_CommandParameter|Value|... */

void ZWave::CMD_Send_Command_To_Child(string sID,int iPK_Command,string sParameters,string &sCMD_Result,Message *pMessage)
//<-dceag-c760-e->
{
	if( !ConfirmConnection() )
	{
		sCMD_Result = "NO ZWAVE or ZWAVE BUSY";
		return;
	}

	PLUTO_SAFETY_LOCK(zm, m_ZWaveMutex);
	g_pPlutoLogger->Write(LV_ZWAVE, "ZWave::CMD_Send_Command_To_Child");
	
	int NodeID = atoi(sID.c_str());
	if( NodeID > 0 && NodeID <= 233 && 
		NULL != m_ZWaveAPI->getNode( NodeID ) )
	{
		sCMD_Result = "OK";
		if( iPK_Command == COMMAND_Generic_On_CONST )
		{
			g_pPlutoLogger->Write(LV_ZWAVE,"Sending ON - %s", sID.c_str());
			ZWJobSwitchChangeLevel * light = new ZWJobSwitchChangeLevel(m_ZWaveAPI, 99, (unsigned char)NodeID);
			if( light != NULL )
			{
				m_ZWaveAPI->insertJob( light );
				if( !m_ZWaveAPI->start( zwaveSerialDevice.c_str() ) ||
					!m_ZWaveAPI->listen(ZW_TIMEOUT) )
				{
					sCMD_Result = "DEVICE DIDN'T RESPOND OR ZWAVE ERRORS";
					return;
				}
			}
			else
			{
				sCMD_Result = "NOT ENOUGH MEMORY";
				return;
			}
		}
		else if( iPK_Command == COMMAND_Generic_Off_CONST )
		{
			g_pPlutoLogger->Write(LV_ZWAVE,"Sending Off - %s", sID.c_str());
			ZWJobSwitchChangeLevel * light = new ZWJobSwitchChangeLevel(m_ZWaveAPI, 0, (unsigned char)NodeID);
			if( light != NULL )
			{
				m_ZWaveAPI->insertJob( light );
				if( !m_ZWaveAPI->start( zwaveSerialDevice.c_str() ) ||
					!m_ZWaveAPI->listen(ZW_TIMEOUT) )
				{
					sCMD_Result = "DEVICE DIDN'T RESPOND OR ZWAVE ERRORS";
					return;
				}
			}
			else
			{
				sCMD_Result = "NOT ENOUGH MEMORY";
				return;
			}
		}
	}
	else
	{
		sCMD_Result = "BAD NODE ID";
		g_pPlutoLogger->Write(LV_WARNING,"Child device doesn't have a zwave node id");
	}
}

//<-dceag-c776-b->

	/** @brief COMMAND: #776 - Reset */
	/** Reset device. */
		/** @param #51 Arguments */
			/** Argument string
NOEMON or CANBUS */

void ZWave::CMD_Reset(string sArguments,string &sCMD_Result,Message *pMessage)
//<-dceag-c776-e->
{
	// try to avoid trylock timeout
	if( TRYLOCK_TIMEOUT_WARNING <= m_ZWaveAPI->timeLeft() )
	{
		g_pPlutoLogger->Write(LV_ZWAVE,"Force ZWave to stop.");
		m_ZWaveAPI->stop();
	}
	
	if( !ConfirmConnection() )
	{
		sCMD_Result = "NO ZWAVE";
		return;
	}

	PLUTO_SAFETY_LOCK(zm,m_ZWaveMutex);

	ZWJobReset * reset = new ZWJobReset(m_ZWaveAPI);
	if( reset != NULL )
	{
		m_ZWaveAPI->insertJob( reset );
		if( !m_ZWaveAPI->start( zwaveSerialDevice.c_str() ) || !m_ZWaveAPI->listen(ZW_TIMEOUT) )
		{
			sCMD_Result = "DEVICE DIDN'T RESPOND OR ZWAVE ERRORS";
			return;
		}
	}
	else
	{
		sCMD_Result = "NOT ENOUGH MEMORY";
		return;
	}
	
	sCMD_Result = "OK";
}

//<-dceag-c788-b->

	/** @brief COMMAND: #788 - StatusReport */
	/** Test comand. Erase at the end
Asq a report */
		/** @param #51 Arguments */
			/** Argument string */

void ZWave::CMD_StatusReport(string sArguments,string &sCMD_Result,Message *pMessage)
//<-dceag-c788-e->
{
	sCMD_Result = "Not implemented";
}

string ZWave::GetZWaveSerialDevice()
{
	string sPort = TranslateSerialUSB(DATA_Get_COM_Port_on_PC());
	
	if(sPort.find("/dev/") == 0)
	{
		sPort.erase(0, strlen("/dev/"));
	}
	
	if(sPort.length() > 0)
	{
		g_pPlutoLogger->Write(LV_ZWAVE, "Using serial port: %s.", sPort.c_str());
	}
	
	return sPort;
}
