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
	calls_.push_back(pCallData);
}

void 
CallManager::removeCall(CallData* pCallData, bool free) {
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
		COMPARE_OWNERDEVID
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


};
