/*
	IRBase

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

 	Phone: +1 (877) 758-8648

	This program is distributed according to the terms of the Pluto Public License, available at:
	http://plutohome.com/index.php?section=public_license

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "IRBase.h"

#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Table_DeviceTemplate_AV.h"
#include "Infrared_Plugin/Infrared_Plugin.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "DCE/Logger.h"
#include "Serial/SerialPort.h"
#include "IR/IRDevice.h"

#include <math.h>
#include <time.h>

void 
IRBase::handleStart() {
	// get details (IR sequences, Delays, etc.) of all child devices
	g_pPlutoLogger->Write(LV_STATUS, "In IRBase::ParseDevices");
	Command_Impl *pCommand_Impl = getCommandImpl();
	if(!pCommand_Impl) {
		return;
	}

	MapCommand_Impl::iterator it;
	for(MapCommand_Impl::iterator it = pCommand_Impl->m_mapCommandImpl_Children.begin(); 
			it != pCommand_Impl->m_mapCommandImpl_Children.end(); ++it)
	{
		handleStart(it->second);
	}
}

void 
IRBase::handleStart(Command_Impl *pCommand_Impl) {
	long devid = pCommand_Impl->m_pData->m_dwPK_Device;

	g_pPlutoLogger->Write(LV_STATUS, "Requested IR cored for device: %lu", devid);
	
	map<int,string> mapClass;  
	// this will have all the Ruby code, where int is the PK_Command and string is the codeint 
	int iSize=0; char* pData = NULL; // Place holders for the 'out' parameter
	DCE::CMD_Get_Infrared_Codes_DT CMD_Get_Infrared_Codes_DT(devid, DEVICETEMPLATE_Infrared_Plugin_CONST,
				BL_SameHouse, devid, &pData, &iSize);
	getCommandImpl()->SendCommand(CMD_Get_Infrared_Codes_DT);  // Get the codes from I/R Plugin
	IRDevice irDevice;
	irDevice.SerializeRead(iSize, pData); // De-serialize the data
	
	g_pPlutoLogger->Write(LV_STATUS, "IR Code count: %d", mapClass.size());
			
	for(map<int,string>::iterator it = irDevice.m_mapCodes.begin(); it != irDevice.m_mapCodes.end(); it++ ) {
		long cmdid = (*it).first;
		if( m_bMustConvertRC5_6 && (it->second[0]=='5' || it->second[0]=='6') )
			codemap_[longPair(devid, cmdid)] = ConvertRC5_6((*it).second);
		else
			codemap_[longPair(devid, cmdid)] = (*it).second;
		g_pPlutoLogger->Write(LV_STATUS, "Loaded IR code for Device %ld, Action %ld", devid, cmdid);
	}
	MapCommand_Impl::iterator it;
	for(MapCommand_Impl::iterator it = pCommand_Impl->m_mapCommandImpl_Children.begin(); 
			it != pCommand_Impl->m_mapCommandImpl_Children.end(); ++it)
	{
		handleStart(it->second);
	}
}

void 
IRBase::handleStop()
{}

bool IRBase::Translate(MessageReplicator& inrepl, MessageReplicatorList& outrepls) {
	if(AVMessageTranslator::Translate(inrepl, outrepls)) {
		return true;
	}
	Message* pmsg = &inrepl.getMessage();
	Command_Impl *pTargetDev = AVMessageTranslator::FindTargetDevice(pmsg->m_dwPK_Device_To);
	if(!pTargetDev) {
		g_pPlutoLogger->Write(LV_WARNING, "Target Device %d Not Found.", pmsg->m_dwPK_Device_To);
		return false;
	}
	
	DeviceData_Impl *pDeviceData_Impl = pTargetDev->m_pData;
	long devtemplid = pTargetDev->m_pData->m_dwPK_DeviceTemplate, devid = pTargetDev->m_pData->m_dwPK_Device;
	
	int DigitDelay = 0;
	Row_DeviceTemplate_AV* pRowAV = getDatabase()->DeviceTemplate_AV_get()->GetRow(devtemplid);
	if(pRowAV) {
		DigitDelay = pRowAV->DigitDelay_get();
	} else {
		g_pPlutoLogger->Write(LV_STATUS, "Device has no AV properties");
	}

	/********************************************************************************************************
	COMMAND_Send_Code_CONST
	********************************************************************************************************/
	if(pmsg->m_dwID == COMMAND_Send_Code_CONST) {
		outrepls.push_back(inrepl);
		return true;
	} else
	/********************************************************************************************************
	COMMAND_Tune_to_channel_CONST
	********************************************************************************************************/
	if( pmsg->m_dwID == COMMAND_Tune_to_channel_CONST) {
		//AddChannelChangeToQueue(atoi(pmsg->m_mapParameters[COMMANDPARAMETER_ProgramID_CONST].c_str()), TargetDevice);
		static const int DigitCmd[] = { COMMAND_0_CONST, COMMAND_1_CONST, COMMAND_2_CONST, COMMAND_3_CONST, COMMAND_4_CONST,
			COMMAND_5_CONST, COMMAND_6_CONST, COMMAND_7_CONST, COMMAND_8_CONST, COMMAND_9_CONST };

		long ChannelNumber = atoi(pmsg->m_mapParameters[COMMANDPARAMETER_ProgramID_CONST].c_str());
					
		long NumDigits = 3;
		bool bSendEnter = false;
		string sNumDigits = pRowAV->NumericEntry_get();
		if (sNumDigits.size()) {
			string::size_type pos = 0;
	
			NumDigits = atoi(StringUtils::Tokenize(sNumDigits, ",", pos).c_str());
			string tok = StringUtils::ToUpper(StringUtils::Tokenize(sNumDigits, ",", pos));
	
			bSendEnter = (tok=="E" || tok=="e");
		} else {
			g_pPlutoLogger->Write(LV_WARNING, "Device id %ld has no number digits parameter.  Assuming 3 and no enter.\n", devid);
		}
		
		long TotalDigits = StringUtils::ltos(ChannelNumber).length();
		if (NumDigits < TotalDigits) {
			if (NumDigits > 0) {
				g_pPlutoLogger->Write(LV_WARNING, "Warning, number of digits specified as %d but channel is %d!", NumDigits, ChannelNumber);
			}
			NumDigits = TotalDigits;
		}
		for(long i = NumDigits; i>0; i--) {
			unsigned char digit = (ChannelNumber % (long) pow((double) 10, (double) i)) / (long) pow((double) 10, (double) (i-1));
			g_pPlutoLogger->Write(LV_STATUS, "Sending digit %d...", digit);

			MessageReplicator msgrepl(
				Message(inrepl.getMessage().m_dwPK_Device_From, inrepl.getMessage().m_dwPK_Device_To, 
								PRIORITY_NORMAL, MESSAGETYPE_COMMAND,
								DigitCmd[digit], 0),
						1, DigitDelay);
			outrepls.push_back(msgrepl);
		}
		if(bSendEnter) {
			g_pPlutoLogger->Write(LV_STATUS, "Sending <enter>...");
			MessageReplicator msgrepl(
				Message(inrepl.getMessage().m_dwPK_Device_From, inrepl.getMessage().m_dwPK_Device_To, 
								PRIORITY_NORMAL, MESSAGETYPE_COMMAND,
								COMMAND_Send_Generic_EnterGo_CONST, 0),
						1, DigitDelay);
			outrepls.push_back(msgrepl);
		}
		return true;
	} 
	
	map <longPair, string>::iterator it = codemap_.find(longPair(devid, pmsg->m_dwID));
	if(it == codemap_.end()) {
		g_pPlutoLogger->Write(LV_WARNING, "Infrared Code not found for Command %d. Will not be processed by IRBase.", pmsg->m_dwID);
		return false;
	} else {
		outrepls.push_back(inrepl);
		return true;
	}
	
	// Ok, not a system-handled action.
	return false;
}

void 
IRBase::DispatchMessage(Message* pmsg) {
	g_pPlutoLogger->Write(LV_STATUS, "In IRBase::DispatchMessage");
	
	string irport, ircode;
	int cmd = 0, devid = pmsg->m_dwPK_Device_To;

	Command_Impl *pTargetDev = AVMessageDispatcher::FindTargetDevice(devid);
	if(!pTargetDev) {
		g_pPlutoLogger->Write(LV_WARNING, "Could not find target device %d...", devid);
		return;
	}
	
	if(pmsg->m_dwID == COMMAND_Send_Code_CONST) {
		irport = pmsg->m_mapParameters[COMMANDPARAMETER_Absolute_Channel_CONST],
		ircode = pmsg->m_mapParameters[COMMANDPARAMETER_Tokens_CONST];
		
		if(ircode.empty()) {
			if(pmsg->m_mapParameters.count(COMMANDPARAMETER_ID_CONST) > 0) {
				// Is an action ID specified?
				cmd = atoi(pmsg->m_mapParameters[COMMANDPARAMETER_ID_CONST].c_str());
			} else {
				g_pPlutoLogger->Write(LV_WARNING, "Command Send Code does not contain any info about infrared code...");
				return;
			}
		} else {
			SendIR(irport, ircode);
		}
	} else {
		cmd = pmsg->m_dwID;
		irport = pTargetDev->m_pData->m_mapParameters[DEVICEDATA_PortChannel_Number_CONST];
	}
		
	map <longPair, string>::iterator it = codemap_.find(longPair(devid, cmd));
	if(it == codemap_.end()) {
		g_pPlutoLogger->Write(LV_WARNING, "Could not find Infrared Code for Command %d.", cmd);
		return;
	} else {
		ircode = (*it).second;
	}
	
	if(!ircode.empty()) {
		g_pPlutoLogger->Write(LV_STATUS, "Sending Infrared Code for dev <%d> cmd <%d>, channel <%s>, code <%s>", 
					devid, cmd, irport.c_str(), ircode.c_str());
		SendIR(irport, ircode);
	} else {
		g_pPlutoLogger->Write(LV_WARNING, "Infrared Code Data is Empty...");
	}
}

