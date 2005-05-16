/*
 Notification
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

/**
 *
 * @file  Notification.cpp
 * @brief source file for the Notification class
 *
 */

#define MAX_TIMEOUT_FOR_PHONES		180
#define TIMEOUT_REDIAL				30

#define TYPE_ORBITER		1
#define TYPE_OTHER			2

#include "Notification.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "Security_Plugin.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"

#include <sstream>

#include "pluto_security/Table_Alert.h"
#include "pluto_security/Table_Alert_Device.h"
#include "pluto_security/Table_AlertType.h"
#include "pluto_security/Table_ModeChange.h"

void* StartNotificationInfo( void* param ) 
{
	NotificationInfo *pNotificationInfo = (NotificationInfo *)param;
	if( pNotificationInfo->m_Type==TYPE_ORBITER )
		pNotificationInfo->m_pNotification->NotifyOrbiter(pNotificationInfo->m_sPhoneNumber,0);
	else
		pNotificationInfo->m_pNotification->NotifyOther(pNotificationInfo->m_sPhoneNumber,0);

	delete pNotificationInfo;
	return NULL;
}

// The primary constructor when the class is created as a stand-alone device
Notification::Notification(Security_Plugin *pSecurity_Plugin,Router *pRouter,Row_Alert *pRow_Alert)
	: m_AlertMutex("alert")
{
	m_pSecurity_Plugin=pSecurity_Plugin;
	m_pRouter=pRouter;
	m_pRow_Alert=pRow_Alert;
    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
	m_AlertMutex.Init(&m_MutexAttr);

}

Notification::~Notification()
{
    pthread_mutexattr_destroy(&m_MutexAttr);
}

NotificationInfo::~NotificationInfo() 
{
	PLUTO_SAFETY_LOCK(am,m_pNotification->m_AlertMutex);
	m_pNotification->m_listNotificationInfo.remove(this);
}

void Notification::DoIt()
{
	PLUTO_SAFETY_LOCK(am,m_AlertMutex);
	m_sOther_Phone_Notifications=m_pSecurity_Plugin->DATA_Get_Other_Phone_Notifications();
	m_sOrbiterNotifications=m_pSecurity_Plugin->DATA_Get_Mobile_Orbiter_Notification();

	// First figure out the order.  Other phone notifications are like this:
	// [seconds delay],[order],[phone number],[4 flags], ....  repeating number/flags
	string::size_type pos=m_sOther_Phone_Notifications.find(',');
	int iOrder = 1;
	if( pos!=string::npos && pos<m_sOther_Phone_Notifications.length()-1 )
		iOrder = atoi( m_sOther_Phone_Notifications.substr(pos+1).c_str() );
	
	am.Release();
	if( iOrder==0 )
	{
		if( !NotifyLoop(TYPE_ORBITER,false) )
			NotifyLoop(TYPE_OTHER,false);
	}
	else if( iOrder==1 )
	{
		if( !NotifyLoop(TYPE_OTHER,false) )
			NotifyLoop(TYPE_ORBITER,false);
	}
	else
	{
		if( !NotifyLoop(TYPE_OTHER,true) )
			NotifyLoop(TYPE_ORBITER,true);
	}
	am.Relock();

	// Wait for any pending calls to complete before we exit
	while( m_listNotificationInfo.size() )
	{
		am.Release();
		Sleep(1000);  // They may need to use the lock to add/delete themselves from m_listNotificationInfo
		am.Relock();
	}
}

bool Notification::NotifyLoop(int iType,bool bProcessInBackground)
{
	PLUTO_SAFETY_LOCK(am,m_AlertMutex);
	if( m_pRow_Alert->ResetTime_isNull()==false )  // If it's not null we handled it
		return true;

	string *s;
	if( iType==TYPE_ORBITER )
		s=&m_sOrbiterNotifications;
	else
		s=&m_sOther_Phone_Notifications;

	string::size_type pos=0;
	int iDelay = atoi(StringUtils::Tokenize((*s),",",pos).c_str());

	if( iType==TYPE_OTHER )
		StringUtils::Tokenize((*s),",",pos); // Pop the execution order off--we don't need it here

	while( pos<(*s).size() && pos!=string::npos )
	{
        string sPhoneNumber = StringUtils::Tokenize((*s),",",pos);
		bool bMonitor = StringUtils::Tokenize((*s),",",pos)=="1";
		bool bSecurity = StringUtils::Tokenize((*s),",",pos)=="1";
		bool bFire = StringUtils::Tokenize((*s),",",pos)=="1";
		bool bAirQuality = StringUtils::Tokenize((*s),",",pos)=="1";
		bool bDoorbell = StringUtils::Tokenize((*s),",",pos)=="1";

		if( 	(m_pRow_Alert->FK_AlertType_get()==ALERTTYPE_Monitor_mode_CONST && !bMonitor) ||
				(m_pRow_Alert->FK_AlertType_get()==ALERTTYPE_Security_CONST && !bSecurity) ||
				(m_pRow_Alert->FK_AlertType_get()==ALERTTYPE_Fire_CONST && !bFire) ||
				(m_pRow_Alert->FK_AlertType_get()==ALERTTYPE_Air_Quality_CONST && !bAirQuality) ||
				(m_pRow_Alert->FK_AlertType_get()==ALERTTYPE_Doorbell_CONST && !bDoorbell) )
{
g_pPlutoLogger->Write(LV_STATUS,"Skipping orbiter %s",sPhoneNumber.c_str());
			continue;
}

		if( iDelay>0 )
		{
			if( NotifyOrbiter(sPhoneNumber,iDelay) )  // Function will give that phone iDelay seconds
				return true;  // Stop notifying others
		}
		else
		{
			NotificationInfo *pNotificationInfo = new NotificationInfo(this,TYPE_ORBITER,sPhoneNumber);
			pthread_t pthread_id; 
			if(pthread_create( &pthread_id, NULL, StartNotificationInfo, (void*)pNotificationInfo)==0 )
				m_listNotificationInfo.push_back(pNotificationInfo);
		}
	}

	// Wait for any pending calls to complete before we exit if we're not processing in the background
	while( !bProcessInBackground && m_listNotificationInfo.size() && m_pRow_Alert->ResetTime_isNull() )
	{
		am.Release();
		Sleep(1000);  // They may need to use the lock to add/delete themselves from m_listNotificationInfo
		am.Relock();
	}

	return m_pRow_Alert->ResetTime_isNull()==false;  // If it's not null we handled it
}

bool Notification::NotifyOrbiter(string sPhoneNumber,int iDelay)
{
	// iDelay = How many seconds to wait for the phone to respond and acknowledge
	if( iDelay==0 )
		iDelay = MAX_TIMEOUT_FOR_PHONES;

	g_pPlutoLogger->Write(LV_CRITICAL,"Notifying orbiter %s for delay %d",sPhoneNumber.c_str(),iDelay);
	return false;
}

bool Notification::NotifyOther(string sPhoneNumber,int iDelay)
{
	g_pPlutoLogger->Write(LV_CRITICAL,"Notifying phone %s for delay %d",sPhoneNumber.c_str(),iDelay);
	return false;
}

