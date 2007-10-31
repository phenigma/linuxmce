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
	  sMyCallID(mycallid)
{
	//1) {X,Y,A} & B => {X,Y} & {A,B}
	//or 
	//2) {X,Y,A} & {B,C} => {X,Y} & {A,B,C} 

	//struct Task
	//{
	//	ext dest,
	//	call id dest,
	//	my channel,
	//	>> my call id
	//}

	//create "private chat"
	//daca failed -> msg to orbiter si refacere stare
}

AssistedTransfer::~AssistedTransfer()
{
}

bool AssistedTransfer::ProcessJob(const string & job)
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "AssistedTransfer : No such job %s", job.c_str());
	
	//job "initialize" (internal)
	//1) {X,Y,A} & B => {X,Y} & {A,B}
	//or 
	//2) {X,Y,A} & {B,C} => {X,Y} & {A,B,C} 
	if( job == "initialize" )
	{
		telecom->CMD_PL_Transfer(0, 0, sExtDest, sMyChannelID, "");
	}
	//job "transfer"
	//1) {X,Y,A} & B => {X,Y} & {A,B} -> drop A, {X,Y,B}
	//or 
	//2) {X,Y,A} & {B,C} => {X,Y} & {A,B,C} 
	else if( job == "transfer" )
	{
	}
	//job "conference"
	//1) {X,Y,A} & B => {X,Y} & {A,B} -> {X,Y,A,B}
	else if( job == "conference" )
	{
	}
	//job "merge calls"
	//2) {X,Y,A} & {B,C} => {X,Y,A,B,C} 
	else if( job == "merge calls" )
	{
	}
	//job "cancel"
	//1) {X,Y,A} & B => {X,Y} & {A,B} -> {X,Y,A} & B
	//or 
	//2) {X,Y,A} & {B,C} => {X,Y,A} & {B,C} 
	else if( job == "cancel" )
	{
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "AssistedTransfer : No such job %s", job.c_str());
		return false;
	}
	
	sJobID = job;
	
	return true;
}

bool AssistedTransfer::ProcessEvent(class Message * pMessage)
{
	if( pMessage->m_MessageID == EVENT_PBX_Link_CONST )
	{
		string sSource_Channel = pMessage->m_mapParameters[EVENTPARAMETER_Source_Channel_CONST];
		string sDestination_Channel = pMessage->m_mapParameters[EVENTPARAMETER_Destination_Channel_CONST];
		string sSource_Caller_ID = pMessage->m_mapParameters[EVENTPARAMETER_Source_Caller_ID_CONST];
		string sDestination_Caller_ID = pMessage->m_mapParameters[EVENTPARAMETER_Destination_Caller_ID_CONST];
		
		return true;
	}
	else if( pMessage->m_MessageID == EVENT_PBX_Hangup_CONST )
	{
		string sChannel_ID = pMessage->m_mapParameters[EVENTPARAMETER_Channel_ID_CONST];
		string sReason = pMessage->m_mapParameters[EVENTPARAMETER_Reason_CONST];
		
		return true;
	}
	
	return false;
}
