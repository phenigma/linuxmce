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
#ifndef DCERUBYSERIALWRAPPER_H
#define DCERUBYSERIALWRAPPER_H

#include "RubyDeviceWrapper.h"
#include "RubyCommandWrapper.h"
#include "RubyDCEConnector.h"
#include "RubySerialIOConnectionWrapper.h"

#include "PlutoUtils/StringUtils.h"

#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_DeviceData.h"


#include <list>
#include <ruby.h>

namespace DCE {

/**
@author Igor Spac,,,
*/
class RubySerialWrapper{
public:
    RubySerialWrapper();
    ~RubySerialWrapper();

public:
	void setDevice(const RubyDeviceWrapper& device) {
		device_ = device;
	}
	RubyDeviceWrapper& getDevice() {
		return device_;
	}
	
	void setDCEConnector(RubyDCEConnector* pdce) {
		pdce_ = pdce;
	}
	RubyDCEConnector* getDCEConnector() {
		return pdce_;
	}
	
	void setParent(VALUE parent) {
	    parent_ = parent;
	}

	VALUE getParent() {
	    return parent_;
	}

public:
	void SendCommand(RubyCommandWrapper* pcmd);
	
	/*accessed from ruby code*/
public:
	VALUE parent_;
	std::list<VALUE> children_;
	RubyDeviceWrapper device_; /*our device*/

	void DisableDevice(int device,bool bDisable);

	void SetDeviceDataInDB( int device, int PK_DeviceData, string value );
	string GetCurrentDeviceData( int PK_Device, int PK_DeviceData );
	
private:
	RubyDCEConnector* pdce_;
};

};

#endif
