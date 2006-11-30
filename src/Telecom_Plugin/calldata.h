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

	int m_PK_Device_Remote_get() {
			return m_PK_Device_Remote; }
	void m_PK_Device_Remote_set(int PK_Device_Remote) {
			m_PK_Device_Remote = PK_Device_Remote; }

	int m_PK_Users_get() {
			return m_PK_Users; }
	void m_PK_Users_set(int PK_Users) {
			m_PK_Users = PK_Users; }

	std::string getID() {
			return id_; }
	void setID(std::string id) {
			id_ = id; }
			
	std::string getCallerID() {
			return callerId_; }
	void setCallerID(std::string callerId) {
			callerId_ = callerId; }

	/*state info*/
private:
	CALLSTATE state_;
	std::string id_;
	std::string callerId_;

	int pendingCmdID_;
	int ownerDevID_;
	int m_PK_Device_Remote; // The remote control that started this call
	int m_PK_Users; // The user that originated this call
};

};

#endif
