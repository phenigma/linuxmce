//
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DCERUBYSERIALIOMANAGER_H
#define DCERUBYSERIALIOMANAGER_H

#include "RubyDCEConnector.h"
#include "RubyDCECodeSupplier.h"
#include "RubySerialIOPool.h"

class Database_pluto_main;

namespace DCE {

class DeviceData_Base;
class DeviceData_Impl;
class Event_Impl;

/**
@author Igor Spac,,,
*/

/*manages instances of serial pools*/
/* ----!!!!!!!!!!!!!
	manager was added because of limitation of ruby to run only in process context and not in thread context,
	because of this we will not use each Pool's state machine, intead will use this class to manage comunication between ruby and DCE
	*/
class RubySerialIOManager : protected RubyDCEConnector, public IOThread {
public:
    RubySerialIOManager();
    virtual ~RubySerialIOManager();

public:
	void setDatabase(Database_pluto_main* pdb) {
		pdb_ = pdb;
	}
	Database_pluto_main* getDatabase() {
		return pdb_;
	}

	void setEventDispatcher(Event_Impl* pevdisp) {
		pevdisp_ = pevdisp;
	}
	Event_Impl* getEventDispatcher() {
		return pevdisp_;
	}
		
	int addDevice(DeviceData_Impl* pdevdata);
	int removeDevice(DeviceData_Impl* pdevdata);
	bool hasDevice(DeviceData_Base* pdevdata);
	
public:
	int RouteMessageToDevice(DeviceData_Base* pdevdata, Message *pMessage);
	
protected:
	/*methods for comunicating with DCE, accessed by wrappers */
	virtual void SendCommand(RubyCommandWrapper* pcmd);

protected:
	virtual bool handleStartup();
	virtual void handleTerminate();
	virtual void* _Run();
		
private:
	void ProcessIdle();
	bool DispatchMessageToDevice(Message *pmsg, unsigned deviceid);
	
private:
	RubyDCECodeSupplier cs_;
	Database_pluto_main* pdb_;
	Event_Impl* pevdisp_;
	
	typedef std::list< std::pair<unsigned, Message> > MESSAGEQUEUE;
	MESSAGEQUEUE msgqueue_;
	IOMutex mmsg_;
	IOEvent emsg_;
	
	typedef std::map<std::string, RubySerialIOPool*> POOLMAP;
	POOLMAP pools_; /*[serial port <--> pool] map*/
};

};

#endif
