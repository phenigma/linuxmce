#include "PlutoUtils/CommonIncludes.h"	
#include "Router.h"
#include "../pluto_dce/Table_MasterDeviceList.h"
#include "../pluto_dce/Table_Command.h"
#include "../pluto_dce/Table_C_CommandParameter.h"
#include "../pluto_dce/Table_C_FloorplanType.h"
#include "../pluto_dce/Table_C_EventParameter.h"
#include "../pluto_dce/Table_C_FloorplanObjectType_Color.h"
#include "../pluto_dce/Table_C_DeviceParameter.h"
#include "../pluto_dce/Table_C_HouseMode.h"
#include "../pluto_dce/Table_DeviceCategory.h"
#include "../pluto_dce/Table_Text.h"
#include "../pluto_dce/Table_EventList.h"
#include "../pluto_dce/Table_DesignObj.h"
#include "../pluto_dce/Table_Variable.h"
#include "../pluto_dce/Table_C_FloorplanObjectType.h"
#include <math.h>
#include "DCEMI_StandardHandlers.h"

DCEMI_StandardHandlers::DCEMI_StandardHandlers(class Router *pRouter)
 : PlugIn(pRouter)
{
}

int DCEMI_StandardHandlers::Register() 
{
	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_StandardHandlers::DeviceGroups))
		,0,0,0,0,MESSAGETYPE_DEVICEGROUPS,0);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_StandardHandlers::InitialParmValue))
		,0,0,0,0,MESSAGETYPE_INITIAL_PARM_VALUE,0);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_StandardHandlers::ClosestRelative))
		,0,0,0,0,MESSAGETYPE_CLOSEST_RELATIVE,0);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_StandardHandlers::Family))
		,0,0,0,0,MESSAGETYPE_FAMILY,0);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_StandardHandlers::AssociatedDevices))
		,0,0,0,0,MESSAGETYPE_ASSOCIATED_DEVICES,0);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_StandardHandlers::Children))
		,0,0,0,0,MESSAGETYPE_CHILDREN,0);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_StandardHandlers::MySqlQuery))
		,0,0,0,0,MESSAGETYPE_MYSQLQUERY,0);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_StandardHandlers::SetBoot))
		,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_SET_BOOT_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_StandardHandlers::Reboot))
		,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_REBOOT_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_StandardHandlers::SetReadyStatus))
		,0,0,0,0,MESSAGETYPE_EVENT,EVENTLIST_SET_READY_STATUS_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_StandardHandlers::RequestPublicIP))
		,0,0,0,0,MESSAGETYPE_REQUEST,REQUESTTYPE_PUBLIC_IP);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_StandardHandlers::RequestCommandGroupDescription))
		,0,0,0,0,MESSAGETYPE_REQUEST,REQUESTTYPE_COMMAND_GROUP_DESCRIPTION);

	return 0;
}

bool DCEMI_StandardHandlers::DeviceGroups(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo) 
{
	string ReturnValue = StringUtils::itos((int) m_pRouter->m_mapDeviceGroup.size()) + "|";
	map<int,DeviceGroup *>::iterator itDeviceGroups;
	for( itDeviceGroups=m_pRouter->m_mapDeviceGroup.begin(); itDeviceGroups!=m_pRouter->m_mapDeviceGroup.end();++itDeviceGroups)
	{
		DeviceGroup *pDG = (*itDeviceGroups).second;
		if( !pDG )  // This shouldn't happen
		{
			ReturnValue += "0|";
			continue;
		}
		ReturnValue += StringUtils::itos(pDG->m_PKID_DeviceGroup) + "|";
		ReturnValue += pDG->m_sDescription + "|";
		ReturnValue += StringUtils::itos((int) pDG->m_vectDevices.size()) + "|";
		for(int i=0;i<(int) pDG->m_vectDevices.size();++i)
		{
			Device_Router *pD = pDG->m_vectDevices[i];
			ReturnValue += StringUtils::itos(pD->m_iPKID_Device) + "|";
		}
	}
	pSocket->SendMessage(new Message(0, pMessage->m_DeviceIDFrom, PRIORITY_NORMAL, MESSAGETYPE_REPLY, 0, 1, 0, ReturnValue.c_str()));
	pMessage->m_bRespondedToMessage=true;
	return false; // No further processing necessary
}

bool DCEMI_StandardHandlers::InitialParmValue(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo) 
{
	if( pDeviceTo )
	{
		string Parm;

		if (pMessage->m_ID == DATAPARM_DESCRIPTION)
			Parm = pDeviceTo->m_sDescription;
		else
			Parm = pDeviceTo->mapParameters_Find(pMessage->m_ID);
		pSocket->SendMessage(new Message(0, pMessage->m_DeviceIDFrom, PRIORITY_NORMAL, MESSAGETYPE_REPLY, 0, 1, pMessage->m_ID, Parm.c_str()));
	}
	else
		pSocket->SendMessage(new Message(0, pMessage->m_DeviceIDFrom, PRIORITY_NORMAL, MESSAGETYPE_REPLY, 0, 1, pMessage->m_ID, "Invalid device"));
	pMessage->m_bRespondedToMessage=true;
	return false; // No further processing necessary
}

bool DCEMI_StandardHandlers::ClosestRelative(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo) 
{
	string sStartingDevice = pMessage->m_Parameters[NEAREST_SIBLING_STARTINGDEVICE];
	int DeviceID =  (sStartingDevice.length() > 0) ? atoi(sStartingDevice.c_str()) : pMessage->m_DeviceIDFrom;
	int MasterDeviceType = atoi(pMessage->m_Parameters[NEAREST_SIBLING_MASTERTYPE].c_str());
	pSocket->SendMessage(new Message(0,0,0,0,0,1,1,StringUtils::itos(m_pRouter->FindClosestRelative(MasterDeviceType, DeviceID)).c_str()));
	pMessage->m_bRespondedToMessage=true;
	return false; // No further processing necessary
}

bool DCEMI_StandardHandlers::Family(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo) 
{
	int DeviceID = pMessage->m_ID;
	Device_Router *pDevice = m_pRouter->m_mapDevice_Router_Find(DeviceID);
	if( pDevice )
	{
        DeviceID = pDevice->m_iPKID_Device_ControlledVia;
		if( DeviceID )
			pDevice = m_pRouter->m_mapDevice_Router_Find(DeviceID);

	}
	string Response=StringUtils::itos(DeviceID) + "|";
	if( pDevice && DeviceID )
	{
		m_pRouter->OutputChildren(pDevice,Response);
	}
	else
		Response="0";
	pSocket->SendMessage(new Message(0,0,0,0,0,1,1, Response.c_str()));
	pMessage->m_bRespondedToMessage=true;
	return false; // No further processing necessary
}

bool DCEMI_StandardHandlers::AssociatedDevices(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo) 
{
	int DeviceID = pMessage->m_ID;
	Device_Router *pDevice = m_pRouter->m_mapDevice_Router_Find(DeviceID);
	string Response="";
	if( pDevice )
	{
		map<int,class DeviceRelation *>::iterator it;
		for(it = pDevice->m_mapDeviceRelation.begin();it != pDevice->m_mapDeviceRelation.end();++it )
		{
			Device_Router *p = (*it).second->pDevice;
			if( p )
				Response += StringUtils::itos(p->m_iPKID_Device) + "," + StringUtils::itos(p->m_iPKID_MasterDeviceList) + ",";
		}
	}
	pSocket->SendMessage(new Message(0,0,0,0,0,1,0, Response.c_str()));
	pMessage->m_bRespondedToMessage=true;
	return false; // No further processing necessary
}

bool DCEMI_StandardHandlers::Children(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo) 
{
	int DeviceID = pMessage->m_ID;
	Device_Router *pDevice = m_pRouter->m_mapDevice_Router_Find(DeviceID);
	string Response="";
	if( pDevice )
	{
		m_pRouter->OutputChildren(pDevice,Response);
	}
	else
		Response="0";
	pSocket->SendMessage(new Message(0,0,0,0,0,1,1, Response.c_str()));
	pMessage->m_bRespondedToMessage=true;
	return false; // No further processing necessary
}

bool DCEMI_StandardHandlers::MySqlQuery(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo) 
{
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	string ReturnValue;
	string query = pMessage->m_Parameters[C_COMMANDPARAMETER_TEXT_CONST];
	if( (result_set.r=m_pRouter->mysql_query_result(query)) )
	{
		while ((row = mysql_fetch_row(result_set.r)))
		{
			for(size_t i=0;i<result_set.r->field_count;++i)
			{
				ReturnValue += row[i];
				if( i<result_set.r->field_count-1 )
					ReturnValue += "\t";
			}
			ReturnValue += "\n";
		}
	}
	pSocket->SendMessage(new Message(0, pMessage->m_DeviceIDFrom, PRIORITY_NORMAL, MESSAGETYPE_REPLY, 0, 1, 0, ReturnValue.c_str()));
	pMessage->m_bRespondedToMessage=true;
	return false; // No further processing necessary
}



bool DCEMI_StandardHandlers::SetBoot(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo) 
{
	Device_Router *pDevice=NULL;
	int PKID_Device=atoi(pMessage->m_Parameters[C_COMMANDPARAMETER_PKID_DEVICE_CONST].c_str());
	if( PKID_Device )
		pDevice = m_pRouter->m_mapDevice_Router_Find(PKID_Device);

	if( pDevice && pDevice->m_iPKID_DeviceCategory!=DEVICECATEGORY_HTPC_CONST )
		pDevice = (Device_Router *) pDevice->m_pDevice_ControlledVia;

	if( !pDevice || pDevice->m_IPAddr=="" )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Got set boot to device: %d with no IP",
			pMessage->m_DeviceIDTo);
	}
	else
	{
		unsigned int IPAddress=0;
		string::size_type pos=0;
		for(int i=3;i>=0;--i)
		{
			int num = atoi(StringUtils::Tokenize(pDevice->m_IPAddr,".",pos).c_str());
			int factor = (int) pow((double)256,i);
			int x=factor*num;
			IPAddress += x;

		}
		char Hex[100];
		sprintf(Hex,"%X",IPAddress);
		string sHex;
		if( strlen(Hex)==7 )
			sHex = string("0") + Hex;
		else
			sHex=Hex;

		char *data_block=NULL;
		int total_size=0;
		int Windows = atoi(pMessage->m_Parameters[C_COMMANDPARAMETER_ID_CONST].c_str());
		FILE *file = fopen(("/usr/local/tftpd/pxelinux.cfg/" + sHex).c_str(),"rb");

		if( file )
		{
			fseek(file, 0L, SEEK_END);
			total_size=ftell(file);
			if( total_size==0 )
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"Cannot open empty boot file: %s",
					sHex.c_str());
			}
			else
			{
				fseek(file, 0L, SEEK_SET);
				data_block=(char *)malloc(total_size+1);
				*data_block=0;  // NULL terminate it in case we don't read in anything
				size_t bytesread=fread(data_block,1,total_size,file);
				data_block[bytesread]=0;
			}
			fclose(file);
		}
		if( total_size && data_block )
		{
			if( Windows )
			{
				char *p = strstr(data_block,"KERNEL ");
				if( p )
					*p = '#';
				p = strstr(data_block,"#OCALBOOT ");
				if( p )
					*p = 'L';
			}
			else
			{
				char *p = strstr(data_block,"#ERNEL ");
				if( p )
					*p = 'K';
				p = strstr(data_block,"LOCALBOOT ");
				if( p )
					*p = '#';
			}
			file = fopen(("/usr/local/tftpd/pxelinux.cfg/" + sHex).c_str(),"wb");
			if( file )
			{
				fwrite(data_block,1,total_size,file);
				fclose(file);
			}
			if( Windows )
			{
				// Send a reboot also
				m_pRouter->DispatchMessage(new Message(DEVICEID_DCEROUTER,pDevice->m_iPKID_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,
					COMMAND_REBOOT_CONST,1,C_COMMANDPARAMETER_RESET_CONST,"0"));  // Don't set it back to Linux
			}
		}
	}
	return true;
}

bool DCEMI_StandardHandlers::Reboot(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo) 
{
	string IP="";
	if( pMessage->m_DeviceIDTo==DEVICEID_DCEROUTER )
	{
		string FileName = "/usr/local/pluto/bootpluto/shutdown_" + StringUtils::itos(m_pRouter->m_ServerDeviceID);
		FILE *file = fopen(FileName.c_str(),"wb");
		fwrite("Shutdown.",8,1,file);
		fclose(file);

		string cmd = "/usr/local/pluto/sbin/rebootFromPluto PlutoServer";
		g_pPlutoLogger->Write(LV_CRITICAL,"sending %s",cmd.c_str());
		system(cmd.c_str());
		m_pRouter->m_bQuit=true;
	}
	else if( pDeviceTo )
	{
		IP=pDeviceTo->m_Parameters[C_DEVICEPARAMETER_IP_ADDR_CONST];
		if( IP!="" )
		{
			string FileName = "/usr/local/pluto/bootpluto/shutdown_" + StringUtils::itos(pDeviceTo->m_iPKID_Device);
			FILE *file = fopen(FileName.c_str(),"wb");
			fwrite("Shutdown.",8,1,file);
			fclose(file);

			// If called while rebooting into Windows, Reset will be 0
			string Reset=pMessage->m_Parameters[C_COMMANDPARAMETER_RESET_CONST];
			if( Reset!="0" )
			{
				g_pPlutoLogger->Write(LV_WARNING,"setting back to linux");
				// Be sure we're booting as a linux box
				m_pRouter->DispatchMessage(new Message(DEVICEID_DCEROUTER,DEVICEID_DCEROUTER,
					PRIORITY_NORMAL,MESSAGETYPE_COMMAND,
					COMMAND_SET_BOOT_CONST,2,C_COMMANDPARAMETER_PKID_DEVICE_CONST,StringUtils::itos(pDeviceTo->m_iPKID_Device).c_str(),
					C_COMMANDPARAMETER_ID_CONST,"0"));
			}

			string cmd = ( string("/usr/bin/ssh ") + IP + " \"/usr/local/pluto/sbin/rebootFromPluto PlutoServerViaSSH\"");
			g_pPlutoLogger->Write(LV_WARNING,"sending %s",cmd.c_str());
			system(cmd.c_str());
		}
	}
	return true;
}


bool DCEMI_StandardHandlers::SetReadyStatus(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo) 
{
	if( !pDeviceFrom )
		g_pPlutoLogger->Write(LV_CRITICAL,"Got a ready event from an unknown device");
	else
	{
		pDeviceFrom->m_bIsReady = (pMessage->m_Parameters[C_EVENTPARAMETER_ID_CONST]=="1");
		g_pPlutoLogger->Write(LV_STATUS,"Device: %d %s ready: %s",pDeviceFrom->m_iPKID_Device,pDeviceFrom->m_sDescription.c_str(),
			(pDeviceFrom->m_bIsReady ? "Y" : "N"));
	}
	return true;
}


bool DCEMI_StandardHandlers::RequestPublicIP(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo) 
{
	pSocket->SendMessage(new Message(DEVICEID_DCEROUTER, pMessage->m_DeviceIDFrom, PRIORITY_NORMAL, MESSAGETYPE_REPLY, REQUESTTYPE_PUBLIC_IP, 1, C_DEVICEPARAMETER_IP_ADDR_CONST, m_pRouter->GetPublicIP().c_str()));
	pMessage->m_bRespondedToMessage=true;
	return false;
}

bool DCEMI_StandardHandlers::RequestCommandGroupDescription(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo) 
{
	int id = atoi(pMessage->m_Parameters[C_COMMANDPARAMETER_PKID_ACTIONGROUP_CONST].c_str());
	map<int,CommandGroup *>::iterator iag = m_pRouter->m_mapCommandGroup.find(id);
	string agDesc;
	if (iag!=m_pRouter->m_mapCommandGroup.end())
		agDesc = (*iag).second->m_Description;
	
	Message *pResponse = new Message(DEVICEID_DCEROUTER, pMessage->m_DeviceIDFrom, PRIORITY_NORMAL, MESSAGETYPE_REPLY, REQUESTTYPE_COMMAND_GROUP_DESCRIPTION, 1, C_COMMANDPARAMETER_TEXT_CONST, agDesc.c_str());
	pSocket->SendMessage(pResponse);
	pMessage->m_bRespondedToMessage=true;
	return false;
}

