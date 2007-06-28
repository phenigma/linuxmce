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
// C++ Interface: asteriskmanager
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DCECALLANAGER_H
#define DCECALLMANAGER_H

#include <string>
#include <list>
#include "calldata.h"

using namespace std;

namespace DCE {

typedef std::list<CallData*> ListCallData_Manager;
		
/**
@author 
*/
class CallManager
{
private:
    CallManager();
    ~CallManager();

public:
    static CallManager* getInstance();

public:
	void addCall(CallData* pCallData);
	void removeCall(CallData* pCallData, bool free = true);
	void printCalls();

	CallData* findCallByPendingCmdID(int id);
	CallData* findCallByOwnerDevID(int id);
	CallData* findCallByCallId(string id);
	CallData* findCallByCallerId(string id);
	ListCallData_Manager* getCallList();
	
private:
	ListCallData_Manager calls_;

	static bool equalsPendingCmdID_(CallData* pData1, CallData* pData2) {
		return pData1->getPendingCmdID() == pData2->getPendingCmdID(); }
	static bool equalsOwnerDevID_(CallData* pData1, CallData* pData2) {
        return pData1->getOwnerDevID() == pData2->getOwnerDevID(); }
	
    // manager instance
private:
    static CallManager* sinstance;
};

};

#endif
