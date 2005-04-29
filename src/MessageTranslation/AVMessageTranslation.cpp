//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "AVMessageTranslation.h"

#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Table_DeviceTemplate_AV.h"

namespace DCE {

bool 
AVMessageTranslator::Translate(MessageReplicator& inrepl, MessageReplicatorList& outrepls) {
	if(DefaultMessageTranslator::Translate(inrepl, outrepls)) {
		return true;
	}
	
	Message* pmsg = &inrepl.getMessage();
	
	Command_Impl* pTargetDev = DefaultMessageTranslator::FindTargetDevice(inrepl.getMessage().m_dwPK_Device_To);
	if(!pTargetDev) {
		g_pPlutoLogger->Write(LV_WARNING, "Target Device %d Not Found.", inrepl.getMessage().m_dwPK_Device_To);
		return false;
	}

	int IR_ModeDelay = 0;
	long devtemplid = pTargetDev->m_pData->m_dwPK_DeviceTemplate, devid = pTargetDev->m_pData->m_dwPK_Device;

	Row_DeviceTemplate_AV* pRowAV = getDatabase()->DeviceTemplate_AV_get()->GetRow(devtemplid);
	if(pRowAV) {
		IR_ModeDelay = pRowAV->IR_ModeDelay_get();
	} else {
		g_pPlutoLogger->Write(LV_STATUS, "Device has no AV properties");
	}
	
	/**************************************************************************************
	COMMAND_Input_Select_CONST
	**************************************************************************************/
	if(pmsg->m_dwID == COMMAND_Input_Select_CONST) {
		map<long, string>::iterator param = inrepl.getMessage().m_mapParameters.find(COMMANDPARAMETER_PK_Command_Input_CONST);
		if (param != inrepl.getMessage().m_mapParameters.end()) {
			int cmd = atoi((*param).second.c_str());
			MessageReplicator msgrepl(
				Message(inrepl.getMessage().m_dwPK_Device_From, inrepl.getMessage().m_dwPK_Device_To, 
								PRIORITY_NORMAL, MESSAGETYPE_COMMAND, cmd, 0),
				1, IR_ModeDelay);
			g_pPlutoLogger->Write(LV_STATUS, "Added <Input Select> translated to %d with delay %d, for Device %d.", cmd, IR_ModeDelay, devid);
			outrepls.push_back(msgrepl);
			return true;
		} else {
			g_pPlutoLogger->Write(LV_WARNING, "PK_Command_Input dparameter not found.");
		}
	}
	/********************************************************************************************************
	COMMAND_Pause_Media_CONST
	
	If we get a pause, set the flag to true.  If the next command for this device is also 
	a pause, translate it to a play.
	TODO: Make a paramter in DeviceTemplate_AV: Translate double pause's to play
	********************************************************************************************************/
	if( pmsg->m_dwID == COMMAND_Pause_Media_CONST)	{
		g_pPlutoLogger->Write(LV_STATUS,"Translate pause media->pause");

		bool ret = false;
		if(lastcmdwaspause_[devid] == true) {
			ret = true;
			MessageReplicator msgrepl(
				Message(inrepl.getMessage().m_dwPK_Device_From, inrepl.getMessage().m_dwPK_Device_To, 
								PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Play_CONST, 0));
			outrepls.push_back(msgrepl);
			g_pPlutoLogger->Write(LV_STATUS, "Pause translated to Play.");
		}
		lastcmdwaspause_[devid] = !lastcmdwaspause_[devid];
		return ret;
	} else 
	/********************************************************************************************************
	COMMAND_Play_Media_CONST
	********************************************************************************************************/
	if( pmsg->m_dwID == COMMAND_Play_Media_CONST) {
		lastcmdwaspause_[devid] = false;
		g_pPlutoLogger->Write(LV_STATUS,"Translate play media->play");
		return false;
	} else
	/********************************************************************************************************
	COMMAND_Change_Playback_Speed_CONST
	********************************************************************************************************/
	if(pmsg->m_dwID == COMMAND_Change_Playback_Speed_CONST) {
		string sValue = pmsg->m_mapParameters[COMMANDPARAMETER_MediaPlaybackSpeed_CONST];
		int iValue = 0;
		if(sValue == "+") {
			iValue = 1;
		} else
		if(sValue == "-") {
			iValue = -1;
		} else {
			iValue = atoi(sValue.c_str());
		}
		
		if(iValue != 0) {
			g_pPlutoLogger->Write(LV_STATUS,"Translate playback speed into scan value: %d", iValue);
			MessageReplicator msgrepl(
				Message(inrepl.getMessage().m_dwPK_Device_From, inrepl.getMessage().m_dwPK_Device_To, 
								PRIORITY_NORMAL, MESSAGETYPE_COMMAND,
								iValue > 0 ? COMMAND_Scan_FwdFast_Fwd_CONST : COMMAND_Scan_BackRewind_CONST, 0),
						abs(iValue));
			return true;
		} else {
			g_pPlutoLogger->Write(LV_WARNING, "Cannot Translate 0 into scan value.");
			return false;
		}
	} else 
	/********************************************************************************************************
	COMMAND_Jump_Position_In_Playlist_CONST
	********************************************************************************************************/
	if( pmsg->m_dwID == COMMAND_Jump_Position_In_Playlist_CONST) {
		string sValue = pmsg->m_mapParameters[COMMANDPARAMETER_Value_To_Assign_CONST];
		int iValue = 0;
		if(sValue == "+") {
			iValue = 1;
		} else
		if(sValue == "-") {
			iValue = -1;
		} else {
			iValue = atoi(sValue.c_str());
		}
	
		if(iValue != 0) {
			g_pPlutoLogger->Write(LV_STATUS,"Translated jump into skip value: %d", iValue);
			MessageReplicator msgrepl(
				Message(inrepl.getMessage().m_dwPK_Device_From, inrepl.getMessage().m_dwPK_Device_To, 
								PRIORITY_NORMAL, MESSAGETYPE_COMMAND,
								iValue > 0 ? COMMAND_Skip_Fwd_ChannelTrack_Greater_CONST : COMMAND_Skip_Back_ChannelTrack_Lower_CONST, 0),
						abs(iValue));
			outrepls.push_back(msgrepl);
			return true;
		} else {
			g_pPlutoLogger->Write(LV_WARNING, "Cannot Translate 0 into skip value.");
			return false;
 		}
	} else 
	/********************************************************************************************************
	COMMAND_Vol_Up_CONST / COMMAND_Vol_Down_CONST
	********************************************************************************************************/
	if(pmsg->m_dwID == COMMAND_Vol_Up_CONST || pmsg->m_dwID == COMMAND_Vol_Down_CONST)
	{
		/*!this was present in old code, not sure it must remain*/
		/*
		outrepls->DiscardMessages(COMMAND_Vol_Up_CONST, TargetDevice);
		outrepls->DiscardMessages(COMMAND_Vol_Down_CONST, TargetDevice);
		*/
		/**/
		return false;
	}	
	
	return false;
}

};
