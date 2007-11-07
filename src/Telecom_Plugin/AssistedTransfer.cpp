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
		SetState(TelecomTask::Failed);
		SetJobState(TelecomTask::Done);
		PrivateProcessJob("cancel");
	}
	
	return bRun;
}

bool AssistedTransfer::ProcessEvent(class Message * pMessage)
{
//	SetState(TelecomTask::Completed);
	if( pMessage->m_MessageID == EVENT_PBX_Link_CONST )
	{
		string sSource_Channel = pMessage->m_mapParameters[EVENTPARAMETER_Source_Channel_CONST];
		string sDestination_Channel = pMessage->m_mapParameters[EVENTPARAMETER_Destination_Channel_CONST];
		string sSource_Caller_ID = pMessage->m_mapParameters[EVENTPARAMETER_Source_Caller_ID_CONST];
		string sDestination_Caller_ID = pMessage->m_mapParameters[EVENTPARAMETER_Destination_Caller_ID_CONST];
		
		switch( step )
		{
			case AssistedTransfer::Init_MyCall2Conference :
				if( sSource_Channel == sMyChannelID )
				{
					CallStatus * pCallStatus = telecom->FindCallStatusForChannel( sMyChannelID );
					if( pCallStatus )
					{
						sMyCallID = pCallStatus->GetID();
					}
					else
					{
						// error
					}
				}
				break;
				
			case AssistedTransfer::Init_MyChannel2DestCall :
				if( sSource_Channel == sMyChannelID )
				{
					CallStatus * pCallStatus = telecom->FindCallStatusForChannel( sMyChannelID );
					if( pCallStatus )
					{
						sCallID_Dest = pCallStatus->GetID();
					}
					else
					{
						// error
					}
				}
				break;
			
			default:
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
	
	if( TelecomTask::Completed == GetState() )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "AssistedTransfer : job completed");
		return false;
	}
	
	if( TelecomTask::Running == GetJobState() )
	{
		// try later
		sNextJob = job;
		return true;
	}
	else
	{
		sNextJob = "";
	}
	
	SetState(TelecomTask::Processing);
	
	CallStatus * pCallStatus = NULL;
	CallStatus * pMyCallStatus = NULL;
	if( !sCallID_Dest.empty() )
	{
		CallStatus * pCallStatus = telecom->FindCallStatusForID( sCallID_Dest );
		if( pCallStatus == NULL )
		{
			// error
			return false;
		}
	}
	if( !sMyCallID.empty() )
	{
		CallStatus * pCallStatus = telecom->FindCallStatusForID( sMyCallID );
		if( pCallStatus == NULL )
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
		// be sure your current call is conference
		// you don't want the other channel to be hang up
		if( !pMyCallStatus->IsConference() )
		{
			// TODO: the new ID will be saved into sMyCallID
			string sNewConfID = telecom->DirectCall2Conference(pCallStatus);
			if( sNewConfID.empty() )
			{
				// error
				return false;
			}
			step = AssistedTransfer::Init_MyCall2Conference;
		}
		
		if( !sExtDest.empty() )
		{
			// TODO: the new ID will be saved into sCallID_Dest
			telecom->CMD_PL_Transfer(0, 0, sExtDest, sMyChannelID, "");
			step = AssistedTransfer::Init_MyChannel2DestCall;
		}
		else if( pCallStatus != NULL )
		{
			if( pCallStatus->IsConference() )
			{
				// transfer my channel to the {B,C} conference
				telecom->CMD_PL_Transfer
					(0, 0, CallStatus::GetStringConferenceID(pCallStatus->GetConferenceID()), sMyChannelID, "");
				step = AssistedTransfer::Init_MyChannel2DestConf;
			}
			else
			{
				// transform the direct call {B,C} into the conference {B,C}
				// TODO: the new ID will be saved into sCallID_Dest
				string sNewConfID = telecom->DirectCall2Conference(pCallStatus);
				if( !sNewConfID.empty() )
				{
					step = AssistedTransfer::Init_DestCall2Conference;
					// transfer my channel to the new conference
					telecom->CMD_PL_Transfer(0, 0, sNewConfID, sMyChannelID, "");
					step = AssistedTransfer::Init_MyChannel2DestConf;
				}
				else
				{
					// error
				}
			}
		}
		else
		{
			// error
		}
	}
	//job "transfer"
	//1) {X,Y,A} & B => {X,Y} & {A,B} -> drop A, {X,Y,B}
	//or
	//2) {X,Y,A} & {B,C} => {X,Y} & {A,B,C} ->  drop A, {X,Y,B,C}
	else if( job == "transfer" )
	{
		if( pCallStatus->IsConference() )
		{
			telecom->CMD_Merge_Calls(sMyCallID ,sCallID_Dest);
			step = AssistedTransfer::MergeCalls;
			telecom->CMD_PL_Cancel(0, sMyChannelID);
			step = AssistedTransfer::Cancel_MyChannel;
		}
		else
		{
			string sChannel = telecom->FindChannelForExt(pCallStatus, sExtDest);
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
		switch( step )
		{
			case AssistedTransfer::Transfer_MyChannel :
			default:
				break;
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
