//<-dceag-d-b->
#include "LIRC_DCE.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "Message.h"
#include "DCERouter.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

//<-dceag-const-b->!
// The primary constructor when the class is created as a stand-alone device
LIRC_DCE::LIRC_DCE(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: LIRC_DCE_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter), m_Virtual_Device_Translator(m_pData)
//<-dceag-const-e->
{
	vector<string> vectMapping,vectConfiguration;
	string sMapping = DATA_Get_Mapping();
	StringUtils::Tokenize(sMapping,"\r\n",vectMapping);	

	// Find all our sibblings that are remote controls
	for(Map_DeviceData_Base::iterator itD=m_pData->m_AllDevices.m_mapDeviceData_Base.begin();
		itD!=m_pData->m_AllDevices.m_mapDeviceData_Base.end();++itD)
	{
		DeviceData_Base *pDevice = itD->second;
		if( pDevice->m_dwPK_Device_ControlledVia==m_pData->m_dwPK_Device_ControlledVia &&
			pDevice->m_dwPK_DeviceCategory==DEVICECATEGORY_LIRC_Remote_Controls_CONST )
		{
			string sConfiguration;
			DCE::CMD_Get_Device_Data_Cat CMD_Get_Device_Data_Cat(m_dwPK_Device,DEVICECATEGORY_General_Info_Plugins_CONST,true,BL_SameHouse,
				pDevice->m_dwPK_Device,DEVICEDATA_Configuration_CONST,true,&sConfiguration);
			if( SendCommand(CMD_Get_Device_Data_Cat) && sConfiguration.size() )
				StringUtils::Tokenize(sConfiguration,"\r\n",vectConfiguration);
		}
	}

	bool bCodesBegan=false;
	for(size_t s=0;s<vectConfiguration.size();++s)
	{
		if( vectConfiguration[s].size()==0 || vectConfiguration[s][0]=='#' )
			continue;
		if( !bCodesBegan )
		{
			bCodesBegan = StringUtils::ToUpper(vectConfiguration[s]).find("BEGIN CODES")!=string::npos;
			continue;
		}
		if( StringUtils::ToUpper(vectConfiguration[s]).find("END CODES")!=string::npos )
			break;

		// We've got a valid code.  Trim it, and find the first white space
		StringUtils::TrimSpaces(vectConfiguration[s]);

		string::size_type pos_firstspace=vectConfiguration[s].find(' ');
		string::size_type pos_tab=vectConfiguration[s].find('\t');
		if( pos_tab!=string::npos && (pos_firstspace==string::npos || pos_tab<pos_firstspace) )
			pos_firstspace = pos_tab;  // This is now the white space

		m_mapMessages[StringUtils::ToUpper(vectConfiguration[s].substr(0,pos_firstspace))]=NULL;
	}
	//Pana aici avem in m_mapMessages o list cu Tokens-urile din Configuation file
	//se apeleaza probabil m_mapMessages["VolUP"] = *pointer ro Message;

	// Now find messages for those tokens
	map<string,Message *>::iterator it;
	for(size_t s=0;s<vectMapping.size();++s)
	{
	cout << vectMapping[s] << endl;
		string::size_type pos=0;
		while(pos<vectMapping[s].size() && pos!=string::npos)
		{
			string sToken = StringUtils::Tokenize(vectMapping[s],";|",pos);
			if( sToken.size()>0 && (it=m_mapMessages.find(StringUtils::ToUpper(sToken)))!=m_mapMessages.end() )
			{
				// We got ourselves a valid one
				pos=vectMapping[s].find('|');
				if( pos!=string::npos )  // Should always be true unless data is malformed
				{
					int iNumberOfArguments;
					char **pArgs = StringUtils::ConvertStringToArgs(vectMapping[s].substr(pos+1),iNumberOfArguments);
					if( iNumberOfArguments )  // Should always be true
					{
						g_pPlutoLogger->Write(LV_STATUS,"LIRC button: %s will fire: %s",sToken.c_str(),vectMapping[s].substr(pos+1).c_str());
						Message *pMessage = new Message(iNumberOfArguments,pArgs,m_dwPK_Device);
// TODO: Dan, if you want hex codes rather than strings, we can do the conversion here
						m_mapMessages[ (*it).first ] = pMessage;
					}
				}
			}
		}
	}

	for(map<string,Message *>::iterator it=m_mapMessages.begin();it!=m_mapMessages.end();++it)
		if( it->second==NULL )
			g_pPlutoLogger->Write(LV_STATUS,"No message for LIRC button: %s",it->first.c_str());

	FILE *fp;
	string sCOM1 = "1";
	string sCOM2 = "2";
	string sLIRCDriver = DATA_Get_Device();
	string sSerialPort = DATA_Get_Serial_Port();
	
	g_pPlutoLogger->Write(LV_STATUS, "Making hardware config");
	if(sSerialPort == sCOM1) {
		sSerialPort = "/dev/ttyS0";
		g_pPlutoLogger->Write(LV_STATUS, "->Found Serial 1 in database");
	} else if(sSerialPort == sCOM2) {
		sSerialPort = "/dev/ttyS1";
		g_pPlutoLogger->Write(LV_STATUS, "->Found Serial 2 in database");
	} else {
		struct stat buf;
		
		if (stat("/dev/lirc", &buf) != -1 && S_ISCHR(buf.st_mode))
		{
			sSerialPort = "/dev/lirc";
		}
		else if (stat("/dev/lirc/0", &buf) != -1 && S_ISCHR(buf.st_mode))
		{
			sSerialPort = "/dev/lirc/0";
		}
		else
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "No SerialPort selected and no default entry found. Exiting.");
			exit(0);
		}

		g_pPlutoLogger->Write(LV_STATUS, "No SerialPort selected, selecting default %s", sSerialPort.c_str());
	}
	
	if(sLIRCDriver == "") {
		g_pPlutoLogger->Write(LV_STATUS, "No Driver selected, using 'default' as default");
		sLIRCDriver = "default";
	}
	
	g_pPlutoLogger->Write(LV_STATUS, "->Using Driver %s in database",sLIRCDriver.c_str());
/*
	system("rm -f /etc/lirc/hardware.conf");
	g_pPlutoLogger->Write(LV_STATUS, "->Writing hardware Config file",sLIRCDriver.c_str());
	fp = fopen("/etc/lirc/hardware.conf","wt");
	if(fp == NULL) {
		g_pPlutoLogger->Write(LV_STATUS, "->Can't open file",sLIRCDriver.c_str());
		return;
	}
	fprintf(fp,"# /etc/lirc/hardware.conf\n");
	fprintf(fp,"#\n");
	fprintf(fp,"# Arguments which will be used when launching lircd\n");
	fprintf(fp,"LIRCD_ARGS=\"\"\n");
	fprintf(fp,"#Dont start lircmd even if there seams to be a good config file\n");
	fprintf(fp,"START_LIRCMD=false\n");
	fprintf(fp,"#Try to load appropriate kernel modules\n");
	fprintf(fp,"LOAD_MODULES=true\n");
	fprintf(fp,"# Run \"lircd --driver=help\" for a list of supported drivers.\n");
	fprintf(fp,"DRIVER=\"%s\"\n",sLIRCDriver.c_str());
	fprintf(fp,"DEVICE=\"%s\"\n",sSerialPort.c_str());
	fprintf(fp,"MODULES=\"lirc_dev lirc_mceusb\"\n");
	fprintf(fp,"# Default configuration files for your hardware if any\n");
	fprintf(fp,"LIRCD_CONF=\"lircd.conf\"\n");
	fprintf(fp,"LIRCMD_CONF=\"lircmd.conf\"\n");
	fclose(fp);
	g_pPlutoLogger->Write(LV_STATUS, "Making Software config");
*/
	
	remove("/etc/lircd.conf");
	fp = fopen("/etc/lircd.conf", "wt");
	for(size_t s = 0; s < vectConfiguration.size(); ++s)
	{
		fprintf(fp, "%s\n", vectConfiguration[s].c_str());
	}
	fclose(fp);

	system("killall -9 lircd");
	system("modprobe lirc_dev");
	system("modprobe lirc_mceusb");
	system((string("lircd") + " -H " + sLIRCDriver + " -d " + sSerialPort + " /etc/lircd.conf").c_str());
//TODO: Check if it started*/
	pRoute = pRouter;
	lirc_leech(DeviceID);	
	g_pPlutoLogger->Write(0, "Over and out"); 
}

//<-dceag-const2-b->!
// The constructor when the class is created as an embedded instance within another stand-alone device
LIRC_DCE::LIRC_DCE(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: LIRC_DCE_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter), m_Virtual_Device_Translator(m_pData)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
LIRC_DCE::~LIRC_DCE()
//<-dceag-dest-e->
{
	for(map<string,Message *>::iterator it=m_mapMessages.begin();it!=m_mapMessages.end();++it)
		delete (*it).second;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool LIRC_DCE::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
LIRC_DCE_Command *Create_LIRC_DCE(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new LIRC_DCE(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void LIRC_DCE::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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
void LIRC_DCE::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

int LIRC_DCE::lirc_leech(int DeviceID) {
#ifndef WIN32
	
	if(lirc_init("irexec",1)==-1) return 1;

	char *code;
	struct timeval last_time = {0, 0}, this_time, time_diff;

	g_pPlutoLogger->Write(LV_STATUS, "Leeching"); 
	while(lirc_nextcode(&code)==0)
	{
		gettimeofday(&this_time, NULL);
		time_diff = this_time - last_time;

		string sCode = string(code);
		vector<string> vectTrimmed;
		
		StringUtils::Tokenize(sCode," ",vectTrimmed);
		int iRepeat = atoi(vectTrimmed[1].c_str());
		string sToken = vectTrimmed[2];
		g_pPlutoLogger->Write(LV_STATUS, "Key Pressed. %s (%s), repeat %d, time diff %lds%ldus",
				sToken.c_str(), sCode.c_str(), iRepeat, time_diff.tv_sec, time_diff.tv_usec);
		
		// ignore repeats and keys received in less than 100ms since the previous (processed) key
		if (iRepeat != 0 || (time_diff.tv_sec == 0 && time_diff.tv_usec < 100000))
			continue;
		last_time = this_time;
		
		map<string,Message *>::iterator it=m_mapMessages.find(StringUtils::ToUpper(sToken));
		if(it!=m_mapMessages.end() && it->second )
			QueueMessageToRouter(it->second);
		else
			g_pPlutoLogger->Write(LV_WARNING, "Error Invalid Key."); 
		free(code);
	}
	lirc_deinit();
#endif
	return 0;
}
