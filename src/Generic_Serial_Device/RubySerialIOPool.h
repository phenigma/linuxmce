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
#ifndef DCERUBYSERIALIOPOOL_H
#define DCERUBYSERIALIOPOOL_H

#include <map>

#include "SerialIOPool.h"
#include "RubyDeviceWrapper.h"
#include "RubyDCECodeSupplier.h"
#include "RubyDCEEmbededClass.h"

class Database_pluto_main;

namespace DCE {

class Message;
class DeviceData_Impl;

/**
@author Igor Spac,,,
*/
class RubySerialIOPool : public SerialIOPool {
public:
    RubySerialIOPool();
    ~RubySerialIOPool();

public:
	void setDatabase(Database_pluto_main* pdb) {
 		pdb_ = pdb;
	}
	Database_pluto_main* getDatabase() {
		return pdb_;
	}
	
	void setDeviceData(DeviceData_Impl* pdevdata) {
		pdevdata_ = pdevdata;
	}
	DeviceData_Impl* getDeviceData() {
		return pdevdata_;
	}

	void setCodeSupplier(RubyDCECodeSupplier* pcs) {
		pcs_ = pcs;
	}
	RubyDCECodeSupplier* getCodeSupplier() {
		return pcs_;
	}
			
public:
	int RouteMessage(Message* pMessage);
	void PopulateDevice(DeviceData_Impl* pdevdata, RubyDeviceWrapper& devwrap);

protected:
	virtual bool handleStartup();
	virtual void handleTerminate();

public:
	void Test();

private:
	Database_pluto_main* pdb_;
	DeviceData_Impl* pdevdata_;
	RubyDCECodeSupplier* pcs_;
	
	RubyDCEEmbededClass* pembclass_; // Ruby class instance
};

};

#endif
