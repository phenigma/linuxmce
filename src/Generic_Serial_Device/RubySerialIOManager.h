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

#include "RubyDCECodeSupplier.h"
#include "RubySerialIOPool.h"

class Database_pluto_main;

namespace DCE {

class DeviceData_Base;
class DeviceData_Impl;

/**
@author Igor Spac,,,
*/

/*manages instances of serial pools*/
class RubySerialIOManager {
public:
    RubySerialIOManager();
    ~RubySerialIOManager();

public:
	void setDatabase(Database_pluto_main* pdb) {
		pdb_ = pdb;
	}
	Database_pluto_main* getDatabase() {
		return pdb_;
	}
	
	int addDevice(DeviceData_Impl* pdevdata);
	int removeDevice(DeviceData_Impl* pdevdata);
	bool hasDevice(DeviceData_Base* pdevdata);
	
public:
	int RouteMessageToDevice(DeviceData_Base* pdevdata, Message *pMessage);

public:
	void RunDevices();
	void WaitDevices();
	
private:
	RubyDCECodeSupplier cs_;
	Database_pluto_main* pdb_;
	
	typedef std::map<std::string, RubySerialIOPool*> POOLMAP;
	POOLMAP pools_; /*[serial port <--> pool] map*/
};

};

#endif
