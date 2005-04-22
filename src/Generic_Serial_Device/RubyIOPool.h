/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

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
	bool handleStartup();
	bool handleIteration();
	void handleTerminate();
	
protected:
	class RubyIOState : public IOPoolState {
		friend class RubyIOPool;
	protected:
		RubyIOState(RubyIOPool* psm) 
			: IOPoolState(psm) {};
		virtual void handleStart();
		virtual void handleStop();
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
