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
#include "callmanager.h"
#include "DCE/Logger.h"

#include <algorithm>

using namespace std;

namespace DCE {

CallManager* CallManager::sinstance = 0;


CallManager::CallManager() {
}

CallManager::~CallManager() {
	for(ListCallData_Manager::iterator it = calls_.begin();
						it != calls_.end(); it++) {
		delete *it;
	}
}


CallManager* CallManager::getInstance() {
	static CallManager manager;
	
	if(sinstance == 0) {
		sinstance = &manager;
	}
	return sinstance;
}

void 
CallManager::addCall(CallData* pCallData) {
/*	LoggerWrapper::GetInstance()->Write(LV_WARNING, "+++++++ CallManager::Add: %s| %s| %d| %d",
		pCallData->getID().c_str(),
		pCallData->getCallerID().c_str(),
		pCallData->getOwnerDevID(),
		pCallData->getPendingCmdID() );*/
	calls_.push_back(pCallData);
}

void 
CallManager::printCalls() {
	CallData* pCallData = NULL;
	for(ListCallData_Manager::iterator it = calls_.begin(); it != calls_.end(); it++) {
		pCallData = (*it);
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "### CallManager::Print: %s| %s| %d| %d",
			pCallData->getID().c_str(),
			pCallData->getCallerID().c_str(),
			pCallData->getOwnerDevID(),
			pCallData->getPendingCmdID() );
	}
}

void 
CallManager::removeCall(CallData* pCallData, bool free) {
/*	LoggerWrapper::GetInstance()->Write(LV_WARNING, "------- CallManager::Remove: %s| %s| %d| %d",
		pCallData->getID().c_str(),
		pCallData->getCallerID().c_str(),
		pCallData->getOwnerDevID(),
		pCallData->getPendingCmdID() );*/
    for(ListCallData_Manager::iterator it = calls_.begin();
                      it != calls_.end(); it++) {
		if((*it) == pCallData) {
			calls_.erase(it);
			if(free) {
				delete pCallData;
			}
			return;
       	}
	}
}

class _CallDataComparator {
public:
	enum COMPARETYPE {
		COMPARE_PENDINGCMDID = 0,
		COMPARE_OWNERDEVID,
		COMPARE_CALLID,
		COMPARE_CALLERID
	};
	
public:
	_CallDataComparator(void* data, COMPARETYPE type) 
			: data_(data), type_(type) {}
	
	bool operator()(CallData* pCallData) {
		switch(type_) {
		case COMPARE_PENDINGCMDID:
				return pCallData->getPendingCmdID() == (int)data_;
			break;
		case COMPARE_OWNERDEVID:
				return pCallData->getOwnerDevID() == (int)data_;
			break;
		case COMPARE_CALLID:
			return pCallData->getID() == *((string *) data_);
			break;
		case COMPARE_CALLERID:
			return pCallData->getCallerID() == *((string *) data_);
			break;
			
		default:
			return false;
		}
	}

private:
	void* data_;
	COMPARETYPE type_;
};

CallData* 
CallManager::findCallByPendingCmdID(int id) {
	ListCallData_Manager::iterator it = 
			find_if(calls_.begin(), calls_.end(), 
				_CallDataComparator((void*)id, _CallDataComparator::COMPARE_PENDINGCMDID));
					
	return (it != calls_.end()) ? *it : 0;
}

CallData*
CallManager::findCallByOwnerDevID(int id) {
	ListCallData_Manager::iterator it = 
			find_if(calls_.begin(), calls_.end(), 
				_CallDataComparator((void*)id, _CallDataComparator::COMPARE_OWNERDEVID));
					
	return (it != calls_.end()) ? *it : 0;
}

CallData*
CallManager::findCallByCallId(string id) {
	ListCallData_Manager::iterator it = 
			find_if(calls_.begin(), calls_.end(), 
				_CallDataComparator((void*)&id, _CallDataComparator::COMPARE_CALLID));
					
	return (it != calls_.end()) ? *it : 0;
}

CallData*
CallManager::findCallByCallerId(string id) {
	ListCallData_Manager::iterator it =
			find_if(calls_.begin(), calls_.end(),
				_CallDataComparator((void*)&id, _CallDataComparator::COMPARE_CALLERID));
	
	return (it != calls_.end()) ? *it : 0;
}

ListCallData_Manager* CallManager::getCallList() {
	return &calls_;
}


};
