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
class RubySerialIOManager : protected RubyDCEConnector {
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
	void RunDevices();
	void WaitDevices();
	int RouteMessageToDevice(DeviceData_Base* pdevdata, Message *pMessage);
	
protected:
	/*methods for comunicating with DCE, accessed by wrappers */
	virtual void SendCommand(RubyCommandWrapper* pcmd);
	

private:
	RubyDCECodeSupplier cs_;
	Database_pluto_main* pdb_;
	Event_Impl* pevdisp_;
	
	typedef std::map<std::string, RubySerialIOPool*> POOLMAP;
	POOLMAP pools_; /*[serial port <--> pool] map*/
};

};

#endif
