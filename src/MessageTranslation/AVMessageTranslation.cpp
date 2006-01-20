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
#include "PlutoUtils/MySQLHelper.h"

#include <cmath>
using namespace std;

namespace DCE {

bool 
AVMessageTranslator::Translate(MessageReplicator& inrepl, MessageReplicatorList& outrepls) {
	if(DefaultMessageTranslator::Translate(inrepl, outrepls)) {
		return true;
	}
	
	Message* pmsg = &inrepl.getMessage();
	static char sql_buff[1024];
	
	DeviceData_Base* pTargetDev = DefaultMessageTranslator::FindTargetDevice(inrepl.getMessage().m_dwPK_Device_To);
	if(!pTargetDev) {
		g_pPlutoLogger->Write(LV_WARNING, "Target Device %d Not Found.", inrepl.getMessage().m_dwPK_Device_To);
		return false;
	}
	int IR_ModeDelay = 0;
	int TogglePower = 0;
	int ToggleInput = 0;
	int DigitDelay = 0;
	int IR_PowerDelay = 0;
	string sNumDigits;
	long devtemplid = pTargetDev->m_dwPK_DeviceTemplate, devid = pTargetDev->m_dwPK_Device;
	int retransmit = 0;
	map<long, string>::iterator retransmit_param = inrepl.getMessage().m_mapParameters.find(COMMANDPARAMETER_Retransmit_CONST);
	if (retransmit_param != inrepl.getMessage().m_mapParameters.end()) {
		retransmit = atoi((*retransmit_param).second.c_str());
		g_pPlutoLogger->Write(LV_STATUS,"WE HAVE RETRANSMIT=%d",retransmit);		
	}
	
	if(map_ModeDelay.find(devtemplid) == map_ModeDelay.end())
	{
		DCEConfig dceconf;
		MySqlHelper mySqlHelper(dceconf.m_sDBHost, dceconf.m_sDBUser, dceconf.m_sDBPassword, dceconf.m_sDBName,dceconf.m_iDBPort);
		PlutoSqlResult result_set;
		MYSQL_ROW row=NULL;
		sprintf(sql_buff,"SELECT IR_ModeDelay, TogglePower, ToggleInput, DigitDelay, NumericEntry FROM DeviceTemplate_AV WHERE FK_DeviceTemplate='%d'",devtemplid);
		if( (result_set.r=mySqlHelper.mysql_query_result(sql_buff)) && (row = mysql_fetch_row(result_set.r)) )
		{
			map_ModeDelay[devtemplid] = IR_ModeDelay = atoi(row[0]);
			map_TogglePower[devtemplid] = TogglePower = atoi(row[1]);
			map_ToggleInput[devtemplid] = ToggleInput = atoi(row[2]);
			map_DigitDelay[devtemplid] = DigitDelay = atoi(row[3]);
			if(row[4])
			{
				map_NumericEntry[devtemplid] = sNumDigits = row[4];
			}
			else
			{
				map_NumericEntry[devtemplid] = sNumDigits = "";
			}
		}
		else
		{
			g_pPlutoLogger->Write(LV_STATUS, "Device has no AV properties");
		}
	}
	else
	{
		IR_ModeDelay = map_ModeDelay[devtemplid];
		TogglePower = map_TogglePower[devtemplid];
		ToggleInput = map_ToggleInput[devtemplid];
		DigitDelay = map_DigitDelay[devtemplid];
		sNumDigits = map_NumericEntry[devtemplid];
		IR_PowerDelay=map_PowerDelay[devtemplid];		
	}
	if (input_commands_.empty())
	{
		DCEConfig dceconf;
		MySqlHelper mySqlHelper(dceconf.m_sDBHost, dceconf.m_sDBUser, dceconf.m_sDBPassword, dceconf.m_sDBName,dceconf.m_iDBPort);
		PlutoSqlResult result_set;
		MYSQL_ROW row=NULL;
		sprintf(sql_buff,"SELECT PK_Command FROM Command WHERE FK_CommandCategory='22'");
		if( (result_set.r=mySqlHelper.mysql_query_result(sql_buff)) == NULL )
		{
			g_pPlutoLogger->Write(LV_WARNING, "SQL FAILED : %s",sql_buff);
			return false;
		}	
		while((row = mysql_fetch_row(result_set.r)))
		{
			input_commands_.push_back(atoi(row[0]));
		}
		for(list<int>::iterator it=input_commands_.begin(); it!=input_commands_.end();it++ )
		{
			int cmd=*it;
			sprintf(sql_buff,"SELECT FK_DeviceTemplate, OrderNo FROM DeviceTemplate_Input WHERE FK_Command='%d'",cmd);
			if( (result_set.r=mySqlHelper.mysql_query_result(sql_buff)) == NULL )
			{
				g_pPlutoLogger->Write(LV_WARNING, "SQL FAILED : %s",sql_buff);
				return false;
			}	
			while((row = mysql_fetch_row(result_set.r)))
			{
				vector<int> *v;
				if(device_input_command_order_.find(atoi(row[0])) == device_input_command_order_.end())
				{
					v=new vector<int>(input_commands_.size()+1,0); /* +1 for ending 0 */
					device_input_command_order_[atoi(row[0])]=v;
				}
				(*device_input_command_order_[atoi(row[0])])[atoi(row[1])]=cmd;
			}
		}
	}
	
	g_pPlutoLogger->Write(LV_STATUS,"AVMessageTranslator::Translate begin");
	
	g_pPlutoLogger->Write(LV_STATUS,"    Status : CMD=%d, TP=%d, TI=%d",pmsg->m_dwID,TogglePower,ToggleInput);
	
	if((TogglePower == 1) && ((pmsg->m_dwID == COMMAND_Generic_On_CONST) || (pmsg->m_dwID == COMMAND_Generic_Off_CONST)))
	{	
		if(retransmit || ((laststatus_power_[devid] && (pmsg->m_dwID == COMMAND_Generic_Off_CONST)) ||
		   (!laststatus_power_[devid] && (pmsg->m_dwID == COMMAND_Generic_On_CONST))))
		{
			MessageReplicator msgrepl(
					Message(inrepl.getMessage().m_dwPK_Device_From, inrepl.getMessage().m_dwPK_Device_To, 
									PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Toggle_Power_CONST, 0),1,0,IR_PowerDelay);
			outrepls.push_back(msgrepl);
			if(!retransmit)
			{
				laststatus_power_[devid] = !laststatus_power_[devid];
			}
			g_pPlutoLogger->Write(LV_STATUS, "Command <%d> translated to <%d>",pmsg->m_dwID,COMMAND_Toggle_Power_CONST);
			return true;
		}
	}
	if((TogglePower == 0) && (pmsg->m_dwID == COMMAND_Toggle_Power_CONST))
	{
		int cmd=0;
		if(laststatus_power_[devid])
			cmd=COMMAND_Generic_Off_CONST;
		else
			cmd=COMMAND_Generic_On_CONST;
		
		MessageReplicator msgrepl(
				Message(inrepl.getMessage().m_dwPK_Device_From, inrepl.getMessage().m_dwPK_Device_To, 
								PRIORITY_NORMAL, MESSAGETYPE_COMMAND,cmd, 0),1,0,IR_PowerDelay);
		outrepls.push_back(msgrepl);
		if(!retransmit)
		{
			laststatus_power_[devid] = (cmd==COMMAND_Generic_On_CONST);
		}
		g_pPlutoLogger->Write(LV_STATUS, "Command <%d> translated to <%d>",pmsg->m_dwID,cmd);
		return true;
	}
	/**************************************************************************************
	COMMAND_Input_Select_CONST
	**************************************************************************************/
	vector<int> commandorder;
	if(device_input_command_order_[devtemplid])
	{
		commandorder=*(device_input_command_order_[devtemplid]);
	}
	if(pmsg->m_dwID == COMMAND_Input_Select_CONST ) {
		map<long, string>::iterator param = inrepl.getMessage().m_mapParameters.find(COMMANDPARAMETER_PK_Command_Input_CONST);
		if (param != inrepl.getMessage().m_mapParameters.end()) {
			int cmd = atoi((*param).second.c_str());
			if(ToggleInput == 0)
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
					if(!retransmit)
					{
						g_pPlutoLogger->Write(LV_STATUS, "Input select was not sent");
					}
					else
					{
						g_pPlutoLogger->Write(LV_STATUS, "Will send one Input select");
						MessageReplicator msgrepl(
								Message(inrepl.getMessage().m_dwPK_Device_From, inrepl.getMessage().m_dwPK_Device_To, 
												PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Input_Select_CONST, 0),1,IR_ModeDelay);
						outrepls.push_back(msgrepl);
					}
				}
			}
			if(!retransmit)
			{
				laststatus_input_[devid] = cmd;
			}
			return true;			
		} else {
			g_pPlutoLogger->Write(LV_WARNING, "PK_Command_Input parameter not found.");
		}
	}
	if((ToggleInput == 1) && (find(commandorder.begin(),commandorder.end(),pmsg->m_dwID) != commandorder.end()))
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
			if(!retransmit)
			{
				g_pPlutoLogger->Write(LV_STATUS, "Input select was not sent");
			}
			else
			{
				g_pPlutoLogger->Write(LV_STATUS, "Will send one Input select");
				MessageReplicator msgrepl(
						Message(inrepl.getMessage().m_dwPK_Device_From, inrepl.getMessage().m_dwPK_Device_To, 
										PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Input_Select_CONST, 0),1,IR_ModeDelay);
				outrepls.push_back(msgrepl);
			}
		}
		if(!retransmit)
		{
			laststatus_input_[devid] = pmsg->m_dwID;
		}
		
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
		if(retransmit || (lastcmdwaspause_[devid] == true)) {
			ret = true;
			MessageReplicator msgrepl(
				Message(inrepl.getMessage().m_dwPK_Device_From, inrepl.getMessage().m_dwPK_Device_To, 
								PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Play_CONST, 0));
			outrepls.push_back(msgrepl);
			g_pPlutoLogger->Write(LV_STATUS, "Pause translated to Play.");
		}
		if(!retransmit)
		{
			lastcmdwaspause_[devid] = !lastcmdwaspause_[devid];
		}
		return ret;
	} else 
	/********************************************************************************************************
	COMMAND_Play_Media_CONST
	********************************************************************************************************/
	if( pmsg->m_dwID == COMMAND_Play_Media_CONST) {
		if(!retransmit)
		{
			lastcmdwaspause_[devid] = false;
		}
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
			if(iValue != 1000)
			{
				g_pPlutoLogger->Write(LV_STATUS,"Translate playback speed into scan value: %d", iValue);
				MessageReplicator msgrepl(
					Message(inrepl.getMessage().m_dwPK_Device_From, inrepl.getMessage().m_dwPK_Device_To, 
									PRIORITY_NORMAL, MESSAGETYPE_COMMAND,
									iValue > 0 ? COMMAND_Scan_FwdFast_Fwd_CONST : COMMAND_Scan_BackRewind_CONST, 0),
							abs(iValue));
				outrepls.push_back(msgrepl);
			}
			else
			{
				g_pPlutoLogger->Write(LV_STATUS,"Translate playback speed(1000) into Play");
				MessageReplicator msgrepl(
					Message(inrepl.getMessage().m_dwPK_Device_From, inrepl.getMessage().m_dwPK_Device_To, 
									PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Play_CONST, 0));
				outrepls.push_back(msgrepl);
			}
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
	/********************************************************************************************************
	COMMAND_Tune_to_channel_CONST
	********************************************************************************************************/
	if( pmsg->m_dwID == COMMAND_Tune_to_channel_CONST) {
		if(sNumDigits=="X") /* Send AS IS if you need so */
		{
			g_pPlutoLogger->Write(LV_STATUS, "Tune to channel will be sent untranslated");
			return false;
		}
		static const int DigitCmd[] = { COMMAND_0_CONST, COMMAND_1_CONST, COMMAND_2_CONST, COMMAND_3_CONST, COMMAND_4_CONST,
			COMMAND_5_CONST, COMMAND_6_CONST, COMMAND_7_CONST, COMMAND_8_CONST, COMMAND_9_CONST };

		long ChannelNumber = atoi(pmsg->m_mapParameters[COMMANDPARAMETER_ProgramID_CONST].c_str());
		long NumDigits = 0 ;
		bool bSendEnter = true;
		if (sNumDigits.size()) {
			string::size_type pos = 0;
	
			NumDigits = atoi(StringUtils::Tokenize(sNumDigits, ",", pos).c_str());
			string tok = StringUtils::ToUpper(StringUtils::Tokenize(sNumDigits, ",", pos));
	
			bSendEnter = (tok=="E" || tok=="e");
		} else {
			g_pPlutoLogger->Write(LV_WARNING, "Device id %ld has no number digits parameter. Sending as is with an enter.\n", devid);
		}
		
		long TotalDigits = long(StringUtils::ltos(ChannelNumber).length());
		if (NumDigits < TotalDigits) {
			if (NumDigits > 0) {
				g_pPlutoLogger->Write(LV_WARNING, "Warning, number of digits specified as %d but channel is %d!", NumDigits, ChannelNumber);
			}
			NumDigits = TotalDigits;
		}
		for(long i = NumDigits; i>0; i--) {
			unsigned char digit = (unsigned char)((ChannelNumber % (long) pow((double) 10, (double) i)) / (long) pow((double) 10, (double) (i-1)));
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
	
	g_pPlutoLogger->Write(LV_STATUS,"AVMessageTranslator::Translate end");
	
	return false;
}

};
