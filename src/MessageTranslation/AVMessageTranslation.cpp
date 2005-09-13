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
#include "DCE/Logger.h"
#include "DCE/DCEConfig.h"
#include "AVMessageTranslation.h"

#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Table_DeviceTemplate_AV.h"
#include "pluto_main/Table_Command.h"
#include "pluto_main/Table_DeviceTemplate_Input.h"

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

	if (input_commands_.empty())
	{
		vector<class Row_Command*> input_row_list;
		getDatabase()->Command_get()->GetRows("where `FK_CommandCategory`=22",&input_row_list);
		for(unsigned int i=0; i<input_row_list.size();i++ )
		{
			int cmd=input_row_list[i]->PK_Command_get();
			input_commands_.push_back(cmd);
			vector<class Row_DeviceTemplate_Input*> command_order_list;
			char buff[100];
			sprintf(buff,"where `FK_Command`=%d",cmd);
			
			getDatabase()->DeviceTemplate_Input_get()->GetRows(buff,&command_order_list);
			for(unsigned int j=0;j<command_order_list.size();j++)
			{
				vector<int> *v;
				if(device_input_command_order_.find(command_order_list[j]->FK_DeviceTemplate_get()) == device_input_command_order_.end())
				{
					v=new vector<int>(input_row_list.size()+1,0); /* +1 for ending 0 */
					device_input_command_order_[command_order_list[j]->FK_DeviceTemplate_get()]=v;
				}
				(*device_input_command_order_[command_order_list[j]->FK_DeviceTemplate_get()])[command_order_list[j]->OrderNo_get()]=cmd;
			}
		}
		
	}
	
	g_pPlutoLogger->Write(LV_STATUS,"AVMessageTranslator::Translate begin");
	
	g_pPlutoLogger->Write(LV_STATUS,"    Status : CMD=%d, TP=%d, TI=%d",pmsg->m_dwID,pRowAV->TogglePower_get(),pRowAV->ToggleInput_get());
	
	if((pRowAV->TogglePower_get() == 1) && ((pmsg->m_dwID == COMMAND_Generic_On_CONST) || (pmsg->m_dwID == COMMAND_Generic_Off_CONST)))
	{	

		if((laststatus_power_[devid] && (pmsg->m_dwID == COMMAND_Generic_Off_CONST)) ||
		   (!laststatus_power_[devid] && (pmsg->m_dwID == COMMAND_Generic_On_CONST)))
		{
			MessageReplicator msgrepl(
					Message(inrepl.getMessage().m_dwPK_Device_From, inrepl.getMessage().m_dwPK_Device_To, 
									PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Toggle_Power_CONST, 0));
			outrepls.push_back(msgrepl);
			laststatus_power_[devid] = !laststatus_power_[devid];
			g_pPlutoLogger->Write(LV_STATUS, "Command <%d> translated to <%d>",pmsg->m_dwID,COMMAND_Toggle_Power_CONST);
			return true;
		}
	}
	if((pRowAV->TogglePower_get() == 0) && (pmsg->m_dwID == COMMAND_Toggle_Power_CONST))
	{
		int cmd=0;
		if(laststatus_power_[devid])
			cmd=COMMAND_Generic_Off_CONST;
		else
			cmd=COMMAND_Generic_On_CONST;
		
		MessageReplicator msgrepl(
				Message(inrepl.getMessage().m_dwPK_Device_From, inrepl.getMessage().m_dwPK_Device_To, 
								PRIORITY_NORMAL, MESSAGETYPE_COMMAND,cmd, 0));
		outrepls.push_back(msgrepl);
		laststatus_power_[devid] = (cmd==COMMAND_Generic_On_CONST);
		g_pPlutoLogger->Write(LV_STATUS, "Command <%d> translated to <%d>",pmsg->m_dwID,cmd);
		return true;
	}
	/**************************************************************************************
	COMMAND_Input_Select_CONST
	**************************************************************************************/
	vector<int> commandorder=*(device_input_command_order_[devtemplid]);
	if(pmsg->m_dwID == COMMAND_Input_Select_CONST ) {
		map<long, string>::iterator param = inrepl.getMessage().m_mapParameters.find(COMMANDPARAMETER_PK_Command_Input_CONST);
		if (param != inrepl.getMessage().m_mapParameters.end()) {
			int cmd = atoi((*param).second.c_str());
			if(pRowAV->ToggleInput_get() == 0)
			{
				MessageReplicator msgrepl(
					Message(inrepl.getMessage().m_dwPK_Device_From, inrepl.getMessage().m_dwPK_Device_To, 
									PRIORITY_NORMAL, MESSAGETYPE_COMMAND, cmd, 0),
					1, IR_ModeDelay);
				g_pPlutoLogger->Write(LV_STATUS, "Command <Input Select> translated to <%d>", cmd);
				outrepls.push_back(msgrepl);
			}
			else
			{
			    unsigned int i=0,count=0;
				if(laststatus_input_[devid]!=0)
				{
					for(i=0;commandorder[i]!=laststatus_input_[devid] && i<commandorder.size(); i++);
				}
				while(commandorder[i]!=cmd)
				{
					if((int)commandorder[i]==(int)0)
					{
						i=0;
						continue;
					}
					count++;			
					if(count == commandorder.size()) 
					{
						count = 0;
						break;
					}
					i++;
				}
				if(count)
				{
					for(i=0;i<count;i++)
					{
						MessageReplicator msgrepl(
								Message(inrepl.getMessage().m_dwPK_Device_From, inrepl.getMessage().m_dwPK_Device_To, 
												PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Input_Select_CONST, 0),1,IR_ModeDelay);
						outrepls.push_back(msgrepl);
					}
					g_pPlutoLogger->Write(LV_STATUS, "Command <%d> translated <%d> translated to %d input selects",pmsg->m_dwID,cmd,count);
				}
				else
				{
					g_pPlutoLogger->Write(LV_STATUS, "Input select was not sent");
				}
			}
			laststatus_input_[devid] = cmd;
			return true;			
		} else {
			g_pPlutoLogger->Write(LV_WARNING, "PK_Command_Input parameter not found.");
		}
	}
	if((pRowAV->ToggleInput_get() == 1) && (find(commandorder.begin(),commandorder.end(),pmsg->m_dwID) != commandorder.end()))
	{
	    unsigned int i=0,count=0;
		g_pPlutoLogger->Write(LV_STATUS, "Got command <%d>, Last was <%d>, need translation to input selects",pmsg->m_dwID,laststatus_input_[devid]);
		
		if(laststatus_input_[devid]!=0)
		{
			for(i=0;commandorder[i]!=laststatus_input_[devid] && i<commandorder.size(); i++);
		}
		while(commandorder[i]!=pmsg->m_dwID)
		{
			if((int)commandorder[i]==(int)0)
			{
				i=0;
				continue;
			}
			count++;			
			if(count == commandorder.size()) 
			{
				count = 0;
				break;
			}
			i++;
		}
		if(count)
		{
			for(i=0;i<count;i++)
			{
				MessageReplicator msgrepl(
						Message(inrepl.getMessage().m_dwPK_Device_From, inrepl.getMessage().m_dwPK_Device_To, 
										PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Input_Select_CONST, 0),1,IR_ModeDelay);
				outrepls.push_back(msgrepl);
			}
			g_pPlutoLogger->Write(LV_STATUS, "Command <%d> translated to %d input selects",pmsg->m_dwID,count);
		}
		else
		{
			g_pPlutoLogger->Write(LV_STATUS, "Input select was not sent");
		}
		laststatus_input_[devid] = pmsg->m_dwID;		
		return true;
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
	g_pPlutoLogger->Write(LV_STATUS,"AVMessageTranslator::Translate end");
	
	return false;
}

};
