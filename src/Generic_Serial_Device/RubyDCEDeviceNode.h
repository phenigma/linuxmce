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
#ifndef DCERUBYDCEDEVICENODE_H
#define DCERUBYDCEDEVICENODE_H

#include <list>

#include "IOPool.h"
#include "RubyDeviceWrapper.h"
#include "RubyDCECodeSupplier.h"
#include "RubyDCEEmbededClass.h"
#include "RubyDCEConnector.h"
#include "RubyDCEDeviceNode.h"

namespace DCE {

/**
@author Igor Spac,,,
*/
class RubyDCEDeviceNode {
	friend class RubyIOManager;
public:
    RubyDCEDeviceNode();
    virtual ~RubyDCEDeviceNode();

public:
	virtual bool Init(RubyDCECodeSupplier* pcs, Database_pluto_main* pdb);
	virtual void Cleanup();
	
	void setDeviceData(DeviceData_Impl* pdevdata) {
		pdevdata_ = pdevdata;
	}
	DeviceData_Impl* getDeviceData() {
		return pdevdata_;
	}
	
	RubyDCEDeviceNode* getParent() {
		return parent_;
	}
	void setParent(RubyDCEDeviceNode* parent) {
		parent_ = parent;
	}
	
	int addChild(RubyDCEDeviceNode* pnode) {
		pnode->setParent(this);
		children_.push_back(pnode);
		return children_.size();
	}
	std::list<RubyDCEDeviceNode*>& getChildren() {
		return children_;
	}
	RubyDCEDeviceNode* findChild(unsigned devid);

protected:
	virtual bool handleNoMessage();
	virtual bool handleMessage(Message* pmsg);
	virtual bool handleStartup();
	virtual void handleTerminate();

protected:
	RubyDCEEmbededClass* getEmbClass() {
		return pembclass_;
	}

private:
	void PopulateDevice(DeviceData_Impl* pdevdata, Database_pluto_main* pdb, RubyDeviceWrapper& devwrap);
	
private:
	RubyDCEDeviceNode* parent_;
	std::list<RubyDCEDeviceNode*> children_;
	
private:
	DeviceData_Impl* pdevdata_;
	RubyDCEEmbededClass* pembclass_; // Ruby class instance
};

};

#endif
