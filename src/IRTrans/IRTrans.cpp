//<-dceag-d-b->
#include "IRTrans.h"
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

#ifdef LINUX
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <netdb.h>
#include <stdint.h>

typedef int DWORD;
typedef int WSAEVENT;


extern int hCom;

SOCKET local_socket;

#define LIRCD                   "/dev/lircd"
#define PERMISSIONS             0666


#endif

#include "remote.h"
#include "dbstruct.h"
#include "network.h"
#include "errcode.h"
#include "fileio.h"
#include "lowlevel.h"
#include "server.h"
#include "global.h"
#include "webserver.h"
#include "flash.h"
#include "xap.h"


extern "C"
{
	int libmain (int argc,char *argv[]);
	void DoExecuteNetCommand (int client,NETWORKCOMMAND *com,STATUSBUFFER *stat);
}
extern void (*CallBackFn)(const char *pRemote,const char *pCommand);
IRTrans *g_pIRTrans=NULL;

void *DoStartIRServer(void *pVoid)
{
	IRTrans *pIRTrans = (IRTrans *) pVoid;
	pIRTrans->StartIRServer();
	return NULL;
}

void DoGotIRCommand(const char *pRemote,const char *pCommand)
{
	g_pIRTrans->GotIRCommand(pRemote,pCommand);
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
IRTrans::IRTrans(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: IRTrans_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
	, IRReceiverBase(this,m_pData), VFD_LCD_Base(2,40)
//<-dceag-const-e->
{
	m_bIRServerRunning=false;
	g_pIRTrans=this;
	// Find all our sibblings that are remote controls 
	for(Map_DeviceData_Base::iterator itD=m_pData->m_AllDevices.m_mapDeviceData_Base.begin();
		itD!=m_pData->m_AllDevices.m_mapDeviceData_Base.end();++itD)
	{
		FileUtils::DelDir("remotes");
		system("mkdir remotes");

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
				const char *pConfig = sConfiguration.c_str();
				FILE *fp = fopen(("remotes/" + StringUtils::itos(pDevice->m_dwPK_Device) + ".rem").c_str(),"wb");
				fwrite(pConfig,1,sConfiguration.size(),fp);
				fclose(fp);
				string sUpper = StringUtils::ToUpper(sConfiguration);
				string::size_type pos_name=0;
				while( (pos_name=sUpper.find("[NAME]",pos_name))!=string::npos )
				{
					// Be sure there's nothing but white space before this
					string::size_type pos_space=pos_name-1;
					while( pos_space>0 && (pConfig[pos_space]==' ' || pConfig[pos_space]=='\t') )
						pos_space--;

					if( pos_space>0 && pConfig[pos_space]!='\r' && pConfig[pos_space]!='\n' )
					{
						pos_name++;
						continue; // It's a name embedded somewhere in the file, not the name we want
					}

					pos_name += 6; // skip the name
					while( pConfig[pos_name] && (pConfig[pos_name]==' ' || pConfig[pos_name]=='\t') )
						pos_name++;

					pos_space = pos_name +1;
					while( pConfig[pos_space] && pConfig[pos_space]!='\n' && pConfig[pos_space]!='\r' && pConfig[pos_space]!='\t' )
						pos_space++;
					char cRemoteLayout = sType.size() ? sType[0] : 'W';
					m_mapNameToDevice[ sConfiguration.substr(pos_name,pos_space-pos_name) ] = pDevice->m_dwPK_Device;
					m_mapRemoteLayout[pDevice->m_dwPK_Device]= cRemoteLayout;
					g_pPlutoLogger->Write(LV_STATUS,"Added remote %s device %d layout %c",sConfiguration.substr(pos_name,pos_space-pos_name).c_str(),pDevice->m_dwPK_Device,cRemoteLayout);
					break;
				}
			}
		}
	}
	pthread_t pthread_id; 
	if(pthread_create( &pthread_id, NULL, DoStartIRServer, (void*) this) )
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot start IRServer thread");
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
IRTrans::~IRTrans()
//<-dceag-dest-e->
{
	
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool IRTrans::Register()
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
void IRTrans::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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
void IRTrans::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/



//<-dceag-c687-b->

	/** @brief COMMAND: #687 - Set Screen Type */
	/** Sent by Orbiter when the screen changes to tells the i/r receiver what type of screen is displayed so it can adjust mappings if necessary. */
		/** @param #48 Value */
			/** a character: M=Main Menu, m=other menu, R=Pluto Remote, r=Non-pluto remote, F=File Listing */

void IRTrans::CMD_Set_Screen_Type(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c687-e->
{
	m_cCurrentScreen=(char) iValue;
	g_pPlutoLogger->Write(LV_STATUS,"Screen type now %c",m_cCurrentScreen);
}

void IRTrans::StartIRServer()
{
#ifndef WIN32
	CallBackFn=&DoGotIRCommand;
	m_bIRServerRunning=true;
	char *argv[]={"IRTrans","-loglevel","4","-debug_code","-no_lirc", "-no_web","/dev/ttyUSB0"};
	libmain(7,argv);
#endif
	m_bIRServerRunning=false;
}

void IRTrans::GotIRCommand(const char *pRemote,const char *pCommand)
{
	int PK_Device = m_mapNameToDevice[pRemote];
	if( !PK_Device )
		g_pPlutoLogger->Write(LV_CRITICAL,"Got command %s from unknown remote %s",pCommand,pRemote);
	else
	{
		g_pPlutoLogger->Write(LV_STATUS,"Got IR Command %s from remote %d",pCommand,PK_Device);
		ReceivedCode(PK_Device,pCommand);
	}
	DoUpdateDisplay(pCommand);
}

void IRTrans::DoUpdateDisplay(string sMessage)
{
	LCDCOMMAND lcdCommand;
	STATUSBUFFER statusBuffer;

	lcdCommand.netcommand=COMMAND_LCD;
	lcdCommand.mode=3;
	lcdCommand.lcdcommand=LCD_TEXT;
	lcdCommand.timeout=3;
	lcdCommand.adress=76;
	lcdCommand.protocol_version=200;
	lcdCommand.wid=40;
	lcdCommand.hgt=4;
	memset(lcdCommand.framebuffer,0,200);
	strcpy((char *) lcdCommand.framebuffer,"012345   10 12345   20 12345   30 12345   40 12345   50 12345   60 12345   70 12345   80 12345   90 12345  100 12345   10 12345   20 12345  130 12345   40 12345   50 12345   60 12345  170 12345  180 12345  190");
//	strcpy((char *) lcdCommand.framebuffer,sMessage.c_str());

	DoExecuteNetCommand (0,(NETWORKCOMMAND *)&lcdCommand,&statusBuffer);
}

