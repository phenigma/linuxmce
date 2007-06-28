/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
//
// C++ Implementation: asteriskmanager
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "calldata.h"
#include "DCE/Logger.h"

namespace DCE {

CallData::CallData()
	:  state_(STATE_NOTDEFINED),
	   pendingCmdID_(0),
	   ownerDevID_(0),
	   m_PK_Device_Remote(0),
	   m_PK_Users(0)
{
}

CallData::~CallData() {
}

};
