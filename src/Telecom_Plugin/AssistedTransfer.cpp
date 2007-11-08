//
// C++ Implementation: AssistedTransfer
//
// Description:
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "AssistedTransfer.h"
#include "Telecom_Plugin.h"

using namespace DCE;

AssistedTransfer::AssistedTransfer(	const string & ext,
									const string & callid,
									const string & channelid,
									const string & mycallid )
	: TelecomTask(),
	  sExtDest(ext),
	  sCallID_Dest(callid),
	  sMyChannelID(channelid),
	  sMyCallID(mycallid),
	  step(AssistedTransfer::Step_Zero)
{
}

AssistedTransfer::~AssistedTransfer()
{
}

bool AssistedTransfer::ProcessJob(const string & job)
{
	bool bRun = PrivateProcessJob(job);
	
	if( !bRun )
	{
		PrivateProcessJob("cancel");
	}
	
	return bRun;
}

bool AssistedTransfer::ProcessEvent(class Message * pMessage)
{
	if( pMessage->m_dwID == EVENT_PBX_Link_CONST )
	{
		string sSource_Channel = pMessage->m_mapParameters[EVENTPARAMETER_Source_Channel_CONST];
		string sDestination_Channel = pMessage->m_mapParameters[EVENTPARAMETER_Destination_Channel_CONST];
		string sSource_Caller_ID = pMessage->m_mapParameters[EVENTPARAMETER_Source_Caller_ID_CONST];
		string sDestination_Caller_ID = pMessage->m_mapParameters[EVENTPARAMETER_Destination_Caller_ID_CONST];
		
		switch( step )
		{
			case AssistedTransfer::Init_MyCall2Conference :
				if( sSource_Channel == sMyChannelID || sSource_Channel == sMyChannel2_ID )
				{
					CallStatus * pCallStatus1 = telecom->FindCallStatusForChannel( sMyChannelID );
					CallStatus * pCallStatus2 = telecom->FindCallStatusForChannel( sMyChannel2_ID );
					if( pCallStatus1 != NULL && pCallStatus1 == pCallStatus2 )
					{
						sMyCallID = pCallStatus->GetID();
						ProcessJob("initialize");
					}
					else
					{
						// error
						SetState(TelecomTask::Failed);
					}
				}
				break;
				
			case AssistedTransfer::Init_DestCall2Conference :
				if( sSource_Channel == sChannel1_Dest || sSource_Channel == sChannel2_Dest )
				{
					CallStatus * pCallStatus1 = telecom->FindCallStatusForChannel( sChannel1_Dest );
					CallStatus * pCallStatus2 = telecom->FindCallStatusForChannel( sChannel2_Dest );
					if( pCallStatus1 != NULL && pCallStatus1 == pCallStatus2 )
					{
						sCallID_Dest = pCallStatus1->GetID();
						ProcessJob("initialize");
					}
					else
					{
						// error
						SetState(TelecomTask::Failed);
					}
				}
				break;
				
			case AssistedTransfer::Init_MyChannel2DestCall :
				if( sSource_Channel == sMyChannelID )
				{
					// todo: do more checking, be paranoic
					CallStatus * pCallStatus = telecom->FindCallStatusForChannel( sMyChannelID );
					if( pCallStatus )
					{
						sCallID_Dest = pCallStatus->GetID();
						SetJobState( TelecomTask::Done );
						if( !sNextJob.empty() )
						{
							ProcessJob(sNextJob);
						}
					}
					else
					{
						// error
						SetState(TelecomTask::Failed);
					}
				}
				break;
			
			case AssistedTransfer::Init_MyChannel2DestConf :
				if( sSource_Channel == sMyChannelID )
				{
					// todo: do more checking, be paranoic
					CallStatus * pCallStatus = telecom->FindCallStatusForChannel( sMyChannelID );
					if( pCallStatus && sCallID_Dest == pCallStatus->GetID() )
					{
						SetJobState( TelecomTask::Done );
						if( !sNextJob.empty() )
						{
							ProcessJob(sNextJob);
						}
					}
					else
					{
						// error
						SetState(TelecomTask::Failed);
					}
				}
				break;
				
			case AssistedTransfer::Transfer_MyChannel :
				SetJobState(TelecomTask::Done);
				SetState(TelecomTask::Completed);
				break;
				
			case AssistedTransfer::JoinDestination :
			case AssistedTransfer::MergeCalls :
				if( sSource_Channel == sMyChannelID )
				{
					CallStatus * pCallStatus = telecom->FindCallStatusForChannel( sMyChannelID );
					if( pCallStatus )
					{
						SetJobState(TelecomTask::Done);
						SetState(TelecomTask::Completed);
					}
					else
					{
						// error
						SetState(TelecomTask::Failed);
					}
				}
				break;
			
			default:
				// todo, debug info ?
				break;
		}
		
		return true;
	}
	else if( pMessage->m_MessageID == EVENT_PBX_Hangup_CONST )
	{
		string sChannel_ID = pMessage->m_mapParameters[EVENTPARAMETER_Channel_ID_CONST];
		string sReason = pMessage->m_mapParameters[EVENTPARAMETER_Reason_CONST];
		
		switch( step )
		{
			case AssistedTransfer::Init_MyChannel2DestCall :
			case AssistedTransfer::Init_MyCall2Conference :
				if( sChannel_ID == sMyChannelID )
				{
					SetJobState(TelecomTask::Done);
					ProcessJob("cancel");
				}
				break;
				
			default:
				break;
		}
		
		return true;
	}
	
	return false;
}

bool AssistedTransfer::PrivateProcessJob(const string & job)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "AssistedTransfer : job %s", job.c_str());
	
	if( job == "cancel" )
	{
		SetState(TelecomTask::Failed);
	}
	else
	{
		if( TelecomTask::Completed == GetState() || TelecomTask::Failed == GetState() )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "AssistedTransfer : job completed/failed !");
			return false;
		}
	
		if( TelecomTask::Running == GetJobState() && job != sJobID )
		{
			// try later
			sNextJob = job;
			return true;
		}
		else
		{
			sNextJob = "";
		}
	
	}
	SetState(TelecomTask::Processing);
	
	CallStatus * pDestCallStatus = NULL;
	CallStatus * pMyCallStatus = NULL;
	if( !sCallID_Dest.empty() )
	{
		pDestCallStatus = telecom->FindCallStatusForID( sCallID_Dest );
		if( pDestCallStatus == NULL )
		{
			// error
			return false;
		}
	}
	if( !sMyCallID.empty() )
	{
		pMyCallStatus = telecom->FindCallStatusForID( sMyCallID );
		if( pMyCallStatus == NULL )
		{
			// error
			return false;
		}
	}
	else
	{
		// error
		return false;
	}
	
	//job "initialize" (internal)
	//1) {X,Y,A} & B => {X,Y} & {A,B}
	//or 
	//2) {X,Y,A} & {B,C} => {X,Y} & {A,B,C} 
	if( job == "initialize" )
	{
		if( step == AssistedTransfer::Step_Zero )
		{
			// be sure your current call is conference
			// you don't want the other channel to be hang up
			if( !pMyCallStatus->IsConference() )
			{
				const map<string, string> & channels = pMyCallStatus->GetChannels();
				if( 2 != channels.size() )
				{
					// error
					return false;
				}
				
				// find the second channel from my call
				for(map<string, string>::const_iterator it=channels.begin(); it!=channels.end(); ++it)
				{
					if( sMyChannelID != (*it).first )
					{
						sMyChannel2_ID = (*it).first;
						break;
					}
				}
				
				// the new ID will be saved into sMyCallID
				string sNewConfID = telecom->DirectCall2Conference(pMyCallStatus);
				if( sNewConfID.empty() )
				{
					// error
					return false;
				}
				
				step = AssistedTransfer::Init_MyCall2Conference;
				return true;
			}
			
			step = AssistedTransfer::Init_MyCall2Conference;
		}
		
		if( step == AssistedTransfer::Init_MyCall2Conference )
		{
			if( !sExtDest.empty() )
			{
				// the new ID will be saved into sCallID_Dest
				telecom->CMD_PL_Transfer(0, 0, sExtDest, sMyChannelID, "");
				step = AssistedTransfer::Init_MyChannel2DestCall;
			}
			else if( pDestCallStatus != NULL )
			{
				if( pDestCallStatus->IsConference() )
				{
					// transfer my channel to the {B,C} conference
					telecom->CMD_PL_Transfer
						(0, 0, CallStatus::GetStringConferenceID(pDestCallStatus->GetConferenceID()), sMyChannelID, "");
					step = AssistedTransfer::Init_MyChannel2DestConf;
				}
				else
				{
					const map<string, string> & channels = pDestCallStatus->GetChannels();
					if( 2 == channels.size() )
					{
						sChannel1_Dest = (*channels.begin()).first;
						sChannel2_Dest = (*(++channels.begin())).first;
						
						// transform the direct call {B,C} into the conference {B,C}
						// TODO: the new ID will be saved into sCallID_Dest
						string sNewConfID = telecom->DirectCall2Conference(pDestCallStatus);
						if( !sNewConfID.empty() )
						{
							step = AssistedTransfer::Init_DestCall2Conference;
						}
						else
						{
							// error
							return false;
						}
					}
					else
					{
						// error
					}
				}
			}
			return true;
		}
		
		if( step == AssistedTransfer::Init_DestCall2Conference )
		{
			// transfer my channel to the new conference
			telecom->CMD_PL_Transfer(0, 0, sCallID_Dest, sMyChannelID, "");
			step = AssistedTransfer::Init_MyChannel2DestConf;
		}
		else
		{
			// error
			return false;
		}
	}
	//job "transfer"
	//1) {X,Y,A} & B => {X,Y} & {A,B} -> drop A, {X,Y,B}
	//or
	//2) {X,Y,A} & {B,C} => {X,Y} & {A,B,C} ->  drop A, {X,Y,B,C}
	else if( job == "transfer" )
	{
		//TODO: check pDestCallStatus ?
		if( pDestCallStatus->IsConference() )
		{
			telecom->CMD_Merge_Calls(sMyCallID ,sCallID_Dest);
			step = AssistedTransfer::MergeCalls;
			telecom->CMD_PL_Cancel(0, sMyChannelID);
			step = AssistedTransfer::Cancel_MyChannel;
		}
		else
		{
			string sChannel = telecom->FindChannelForExt(pDestCallStatus, sExtDest);
			if( !sChannel.empty() )
			{
				telecom->CMD_PL_Transfer
					(0, 0, CallStatus::GetStringConferenceID( pMyCallStatus->GetConferenceID() ), sChannel, "");
				step = AssistedTransfer::JoinDestination;
			}
			else
			{
				// error
				return false;
			}
		}
	}
	//job "conference"
	//1) {X,Y,A} & B => {X,Y} & {A,B} -> {X,Y,A,B}
	else if( job == "conference" )
	{
		telecom->CMD_Merge_Calls(sMyCallID ,sCallID_Dest);
		step = AssistedTransfer::MergeCalls;
	}
	//job "merge calls"
	//2) {X,Y,A} & {B,C} => {X,Y} & {A,B,C} -> {X,Y,A,B,C}
	else if( job == "merge calls" )
	{
		telecom->CMD_Merge_Calls(sMyCallID ,sCallID_Dest);
		step = AssistedTransfer::MergeCalls;
	}
	//job "cancel"
	//1) {X,Y,A} & B => {X,Y} & {A,B} -> {X,Y,A} & B
	//or 
	//2) {X,Y,A} & {B,C} => {X,Y,A} & {B,C} 
	else if( job == "cancel" )
	{
		CallStatus * pMyChannel_CallStatus = telecom->FindCallStatusForChannel( sMyChannelID );
		// the channel is still available, let's try to do the previous configuration
		if( pMyChannel_CallStatus != NULL )
		{
			// check if MY channel was transfered to another call
			if( pMyChannel_CallStatus != pMyCallStatus )
			{
				telecom->CMD_PL_Transfer(0, 0, sMyCallID, sMyChannelID, "");
			}
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "AssistedTransfer : No such job %s", job.c_str());
		return false;
	}
	
	sJobID = job;
	
	return true;
}
