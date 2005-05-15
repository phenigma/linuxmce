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
	m_AlertMutex.Init(NULL);

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
	
	if( iOrder==1 )
	{
		NotifyOrbiters(false);
		NotifyOthers(false);
	}
	else if( iOrder==2 )
	{
		NotifyOthers(false);
		NotifyOrbiters(false);
	}
	else
	{
		NotifyOthers(true);
		NotifyOrbiters(true);
	}

	// Wait for any pending calls to complete before we exit
	while( m_listNotificationInfo.size() )
	{
		am.Release();
		Sleep(1000);  // They may need to use the lock to add/delete themselves from m_listNotificationInfo
		am.Relock();
	}
}

void Notification::NotifyOrbiters(bool bProcessInBackground)
{
	PLUTO_SAFETY_LOCK(am,m_AlertMutex);
	string::size_type pos=0;
	int iDelay = atoi(StringUtils::Tokenize(m_sOrbiterNotifications,",",pos).c_str());
	while( pos<m_sOrbiterNotifications.size() && pos!=string::npos )
	{
        string sPhoneNumber = StringUtils::Tokenize(m_sOrbiterNotifications,",",pos);
		bool bMonitor = StringUtils::Tokenize(m_sOrbiterNotifications,",",pos)=="1";
		bool bSecurity = StringUtils::Tokenize(m_sOrbiterNotifications,",",pos)=="1";
		bool bFire = StringUtils::Tokenize(m_sOrbiterNotifications,",",pos)=="1";
		bool bAirQuality = StringUtils::Tokenize(m_sOrbiterNotifications,",",pos)=="1";
		bool bDoorbell = StringUtils::Tokenize(m_sOrbiterNotifications,",",pos)=="1";

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
			NotifyOrbiter(sPhoneNumber,iDelay);  // Function will give that phone iDelay seconds
		else
		{
			NotificationInfo *pNotificationInfo = new NotificationInfo(this,TYPE_ORBITER,sPhoneNumber);
			pthread_t pthread_id; 
			if(pthread_create( &pthread_id, NULL, StartNotificationInfo, (void*)pNotificationInfo)==0 )
				m_listNotificationInfo.push_back(pNotificationInfo);
		}
	}

	// Wait for any pending calls to complete before we exit if we're not processing in the background
	while( !bProcessInBackground && m_listNotificationInfo.size() )
	{
		am.Release();
		Sleep(1000);  // They may need to use the lock to add/delete themselves from m_listNotificationInfo
		am.Relock();
	}
}

void Notification::NotifyOthers(bool bProcessInBackground)
{
}

void Notification::NotifyOrbiter(string sPhoneNumber,int iDelay)
{
	// iDelay = How many seconds to wait for the phone to respond and acknowledge
	if( iDelay==0 )
		iDelay = MAX_TIMEOUT_FOR_PHONES;
}

void Notification::NotifyOther(string sPhoneNumber,int iDelay)
{
}

