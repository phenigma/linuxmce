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

	CallData* findCallByPendingCmdID(int id);
	CallData* findCallByOwnerDevID(int id);
	
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
