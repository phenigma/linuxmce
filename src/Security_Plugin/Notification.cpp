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
 
//<-dceag-d-b->
#include "Notification.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include <sstream>

#include "pluto_security/Table_Alert.h"
#include "pluto_security/Table_Alert_Device.h"
#include "pluto_security/Table_AlertType.h"
#include "pluto_security/Table_ModeChange.h"

// The primary constructor when the class is created as a stand-alone device
Notification::Notification(Security_Plugin *pSecurity_Plugin,Router *pRouter,Row_Alert *pRow_Alert)
	: m_AlertMutex("alert")
{
	m_pSecurity_Plugin=pSecurity_Plugin;
	m_pRouter=pRouter;
	m_pRow_Alert=pRow_Alert;
	m_AlertMutex.Init(NULL);

}

