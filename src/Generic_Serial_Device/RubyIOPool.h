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
#ifndef DCERUBYIOPOOLMANAGER_H
#define DCERUBYIOPOOLMANAGER_H

#include "IOPool.h"
#include "RubyDeviceWrapper.h"
#include "RubyDCECodeSupplier.h"
#include "RubyDCEEmbededClass.h"
#include "RubyDCEConnector.h"

class Database_pluto_main;

namespace DCE {

class DeviceData_Impl;

/**
@author Igor Spac,,,
*/
class RubyIOPool : protected IOPool {
	friend class RubyIOManager;

public:
	RubyIOPool(IOPool* ppool, bool relpool = true);
    virtual ~RubyIOPool();

public:
	bool Init(RubyDCECodeSupplier* pcs, Database_pluto_main* pdb);
	void Cleanup();
	
	void setDeviceData(DeviceData_Impl* pdevdata) {
		pdevdata_ = pdevdata;
	}
	DeviceData_Impl* getDeviceData() {
		return pdevdata_;
	}
	
public:
	int RouteMessage(Message* pMessage);

protected:
	/*delegated methods*/
	IOConnection* getConnection() {
		return ppool_->getConnection();
	}
	bool handleIteration();
	
	class RubyIOState : public IOPoolState {
		friend class RubyIOPool;
	protected:
		RubyIOState(RubyIOPool* psm) 
			: IOPoolState(psm) {};
		virtual void handleIdle();
		virtual void handleRead(IOConnection* pconn);
	} defstate_;
		
private:
	void PopulateDevice(DeviceData_Impl* pdevdata, Database_pluto_main* pdb, RubyDeviceWrapper& devwrap);
	
private:
	IOPool* ppool_;
	bool relpool_;
	
	DeviceData_Impl* pdevdata_;
	RubyDCEEmbededClass* pembclass_; // Ruby class instance
};

};
#endif
