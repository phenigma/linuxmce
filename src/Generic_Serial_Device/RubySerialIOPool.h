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
#include "RubyDCEConnector.h"

class Database_pluto_main;

namespace DCE {

class Message;
class DeviceData_Impl;
class Event_Impl;

/**
@author Igor Spac,,,
*/

class RubySerialIOPool : public SerialIOPool {
public:
    RubySerialIOPool(RubyDCECodeSupplier* pcs, Database_pluto_main* pdb, DeviceData_Impl* pdevdata);
    ~RubySerialIOPool();

public:
	RubyDCECodeSupplier* getCodeSupplier() 
	{ return pcs_;}
	Database_pluto_main* getDatabase() 
	{ return pdb_;}
	DeviceData_Impl* getDeviceData() 
	{ return pdevdata_;}
	
public:
	void setDCEConnector(RubyDCEConnector* pdceconn) {
		pdceconn_ = pdceconn;
	}
	RubyDCEConnector* getDCEConnector() {
		return pdceconn_;
	}
			
public:
	int RouteMessage(Message* pMessage);

protected:
	virtual bool handleStartup();
	virtual void handleTerminate();

public:
	void Test();

private:
	void PopulateDevice(DeviceData_Impl* pdevdata, RubyDeviceWrapper& devwrap);
	
private:
	Database_pluto_main* pdb_;
	DeviceData_Impl* pdevdata_;
	RubyDCECodeSupplier* pcs_;
	RubyDCEConnector* pdceconn_;
	
	RubyDCEEmbededClass* pembclass_; // Ruby class instance
};

};

#endif
