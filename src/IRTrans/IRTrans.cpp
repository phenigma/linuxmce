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

#ifdef WIN32
typedef int WSAEVENT;
#endif

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

extern "C"
{
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
#include "ccf.h"
	
	int libmain (int argc,char *argv[]);
	void DoExecuteNetCommand (int client,NETWORKCOMMAND *com,STATUSBUFFER *stat);
	void LCDBrightness (int val);
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

//<-dceag-const-b->!
// The primary constructor when the class is created as a stand-alone device
IRTrans::IRTrans(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
        : IRTrans_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
        , IRReceiverBase(this), VFD_LCD_Base(40,2,20)
//<-dceag-const-e->
{
	IRBase::setCommandImpl(this);
	m_bIRServerRunning=false;
	g_pIRTrans=this;
}

//<-dceag-getconfig-b->
bool IRTrans::GetConfig()
{
	if( !IRTrans_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	if( !m_Virtual_Device_Translator.GetConfig(m_pData) )
		return false;

	IRReceiverBase::GetConfig(m_pData);

	FileUtils::DelDir("remotes");
	system("mkdir remotes");
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
	return true;
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
	// Let the IR Base class try to handle the message
	if ( /*!pDeviceData_Base->m_bIsEmbedded && */ IRBase::ProcessMessage(pMessage))
	{
		g_pPlutoLogger->Write(LV_STATUS,"Message %d processed by IRBase class",pMessage->m_dwID);
		sCMD_Result = "OK";
		return;
	}

	sCMD_Result = "UNHANDLED CHILD";
}

// Must override so we can call IRBase::Start() after creating children
void IRTrans::CreateChildren()
{
	IRTrans_Command::CreateChildren();
	Start();
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

	char TTYPort[20];
	strcpy(TTYPort,"/dev/ttyUSBX");
	char *argv[]={"IRTrans","-loglevel","4","-debug_code","-no_lirc", "-no_web",TTYPort};
	for(char cUSB='1';cUSB<='9';cUSB++)
	{
		TTYPort[11]=cUSB;
		g_pPlutoLogger->Write(LV_STATUS,"Looking on %s",argv[6]);
		if( libmain(7,argv)==0 )
			break;
		else
			g_pPlutoLogger->Write(LV_STATUS,"IRTrans not found on %s",argv[6]);
	}
	LCDBrightness(5);
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
}

void IRTrans::DoUpdateDisplay(vector<string> *vectString)
{
	if( !vectString || vectString->size()==0 )
		return;

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
	memset(lcdCommand.framebuffer,32,200);
	lcdCommand.framebuffer[199]=0;
	
	strncpy((char *) lcdCommand.framebuffer,(*vectString)[0].c_str(),min(40,(*vectString)[0].size()));
	if( vectString->size()>1 )
		strncpy((char *) (lcdCommand.framebuffer+40),(*vectString)[1].c_str(),min(40,(*vectString)[0].size()));
string s1=(*vectString)[0];
string s2=vectString->size()>1 ? (*vectString)[1] : "";
g_pPlutoLogger->Write(LV_WARNING,"\x1b[2J;%s\n%s",s1.c_str(),s2.c_str());

	g_pPlutoLogger->Write(LV_WARNING,"setting display to: %s",(char *) lcdCommand.framebuffer);
#ifdef LINUX
	DoExecuteNetCommand (0,(NETWORKCOMMAND *)&lcdCommand,&statusBuffer);
#endif
}

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
		/** @param #103 PK_Device_List */
			/** If going to a plugin that wil relay messages to other devices (ie orbiter_plugin and orbiter), A comma delimited list of devices to display this message on.  If going to a display device directly (like vfd/lcd) this is ignored. */

void IRTrans::CMD_Display_Message(string sText,string sType,string sName,string sTime,string sPK_Device_List,string &sCMD_Result,Message *pMessage)
//<-dceag-c406-e->
{
	NewMessage(atoi(sType.c_str()),sName,sText,atoi(sTime.c_str()));
}

void IRTrans::SendIR(string Port, string IRCode)
{
	g_pPlutoLogger->Write(LV_STATUS,"IRTrans Sending: %s",IRCode.c_str());
#ifdef LINUX
	int res;
	IRDATA ird;
		
	res = DecodeCCF ((char *) IRCode.c_str(),&ird,START);
	if (res <= 0) {
		g_pPlutoLogger->Write(LV_CRITICAL,"Illegal Pronto command %s",IRCode.c_str());
		return;
	}

	ird.address = 0;
	ird.target_mask = 0xffff;
	DoSendIR (&ird,0,0);
#endif
}
//<-dceag-c191-b->

	/** @brief COMMAND: #191 - Send Code */
	/** Sends an I/R code to a device. */
		/** @param #9 Text */
			/** The I/R code -- usually in Pronto format */

void IRTrans::CMD_Send_Code(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c191-e->
{
	SendIR("",sText);
}

//<-dceag-c194-b->

	/** @brief COMMAND: #194 - Toggle Power */
	/** Set relay state (0 or 1) */
		/** @param #8 On/Off */
			/** Depending on each device On/Off can be interpreted differently, but in genereal On/Off has a value of 1 for on and 0 for Off */

void IRTrans::CMD_Toggle_Power(string sOnOff,string &sCMD_Result,Message *pMessage)
//<-dceag-c194-e->
{
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

void IRTrans::CMD_Learn_IR(string sOnOff,int iPK_Text,int iPK_Command_Input,string &sCMD_Result,Message *pMessage)
//<-dceag-c245-e->
{
}
