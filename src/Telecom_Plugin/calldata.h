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
#ifndef DCECALLDATA_H
#define DCECALLDATA_H

#include <string>

namespace DCE {

/**
@author 
*/
class CallData
{
public:
    CallData();
    virtual ~CallData();

public:
	    enum CALLSTATE {
			STATE_NOTDEFINED = 0,
			STATE_ORIGINATING,
			STATE_TRANSFERING,
			STATE_OPENED
		};
		
public:
	int getState() { 
			return state_; }
	void setState(CALLSTATE state) {
			state_ = state; }
	
	int getPendingCmdID() {
			return pendingCmdID_; }
	void setPendingCmdID(int id) {
			pendingCmdID_ = id; }

	int getOwnerDevID() {
			return ownerDevID_; }
	void setOwnerDevID(int id) {
			ownerDevID_ = id; }

	std::string getID() {
			return id_; }
	void setID(std::string id) {
			id_ = id; }

	/*state info*/
private:
	CALLSTATE state_;
	std::string id_;

	int pendingCmdID_;
	int ownerDevID_;
};

};

#endif
